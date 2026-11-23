#include <std_include.hpp>
#include "iw7.hpp"
#include "havok_builder.hpp"

// IW7/stdafx.hpp only pulls in <vector>, <memory>, <iostream> and <sstream>.
#include <array>
#include <algorithm>
#include <tuple>
#include <utility>
#include <cmath>
#include <cfloat>
#include <cstring>
#include <map>
#include <functional>

// Writes hknpCompressedMeshShape and the other IW7 physics objects into hk_2014.2.5-r1 binary packfiles.
// Layouts come from IW7's runtime hkClass tables and the shipped .hkx files.

namespace zonetool::iw7
{
	namespace havok
	{
		namespace builder
		{
			namespace
			{
				constexpr auto HK_MAGIC0 = 0x57E0E057u;
				constexpr auto HK_MAGIC1 = 0x10C0C010u;
				constexpr auto HK_FILE_VERSION = 11;
				constexpr auto HK_HEADER_SIZE = 64;
				constexpr auto HK_SECTION_HEADER_SIZE = 64;
				constexpr auto HK_MAX_PREDICATE = 21;

				// Both the shared-index and packed-vertex counters in a section are uint8.
				constexpr auto MAX_SHARED_INDICES_PER_SECTION = 255;
				// hkcdStaticMeshTreeBasePrimitive::m_indices is uint8[4].
				constexpr auto MAX_VERTEX_INDEX = 255;
				// A primitive key is (section << 8) | (localPrimitive << 1) | triangleInQuad, so a section holds at most
				// 127 primitives.
				constexpr auto KEY_SECTION_SHIFT = 8;

				// From shipped IW7 world blobs.
				constexpr auto WORLD_SHAPE_CONTENTS = 0x28033ED1u;

				// The material CRC shipped world blobs use for their common surfaces.
				constexpr auto DEFAULT_MATERIAL_CRC = 0x1AB7BC33u;
				constexpr auto MAX_PRIMITIVES_PER_SECTION = 127;

				// Object sizes, straight from IW7 reflection.
				constexpr auto SIZEOF_COMPRESSED_MESH_SHAPE = 160;
				constexpr auto SIZEOF_COMPRESSED_MESH_SHAPE_DATA = 256;
				constexpr auto SIZEOF_MESH_TREE = 160;
				constexpr auto SIZEOF_SECTION = 96;
				constexpr auto SIZEOF_PRIMITIVE = 4;
				constexpr auto SIZEOF_DATA_RUN = 4;
				constexpr auto SIZEOF_NODE_TOP = 5; // hkcdStaticTreeCodec3Axis5
				constexpr auto SIZEOF_NODE_SECTION = 4; // hkcdStaticTreeCodec3Axis4

				// Class signatures, read out of the __classnames__ table of shipped files.
				constexpr auto SIG_HK_CLASS = 0x33D42383u;
				constexpr auto SIG_HK_CLASS_MEMBER = 0xB0EFA719u;
				constexpr auto SIG_HK_CLASS_ENUM = 0x8A3609CFu;
				constexpr auto SIG_HK_CLASS_ENUM_ITEM = 0xCE6F8A6Cu;
				constexpr auto SIG_SHAPE_LIST = 0xC909A395u;
				constexpr auto SIG_COMPRESSED_MESH_SHAPE = 0x1318CC9Fu;
				constexpr auto SIG_COMPRESSED_MESH_SHAPE_DATA = 0x54FD8D57u;
				constexpr auto SIG_DYNAMIC_COMPOUND_SHAPE = 0x408A0623u;
				constexpr auto SIG_CONVEX_POLYTOPE_SHAPE = 0x6097F158u;
				constexpr auto SIG_CONVEX_POLYTOPE_CONNECTIVITY = 0xB51806FDu;
				constexpr auto SIG_DYNAMIC_COMPOUND_SHAPE_DATA = 0xF33DC3CCu;
				constexpr auto SIG_PHYSICS_ASSET = 0x0DFB2195u;
				constexpr auto SIG_PHYSICS_SYSTEM_DATA = 0xB26317A4u;
				constexpr auto SIG_XMODEL_LOD = 0x93FC3CBDu;
				constexpr auto SIG_REF_COUNTED_PROPERTIES = 0x7C574867u;
				constexpr auto SIG_SHAPE_MASS_PROPERTIES = 0xE9191728u;

				// Object sizes for the ents-side shapes, from IW7 reflection.
				constexpr auto SIZEOF_DYNAMIC_COMPOUND_SHAPE = 208;
				constexpr auto SIZEOF_DYNAMIC_COMPOUND_SHAPE_DATA = 56;
				constexpr auto SIZEOF_CONVEX_POLYTOPE_SHAPE = 96;
				constexpr auto SIZEOF_CONVEX_POLYTOPE_CONNECTIVITY = 48;
				constexpr auto SIZEOF_SHAPE_INSTANCE = 128;
				constexpr auto SIZEOF_DYNAMIC_TREE_NODE = 32;

				// hknp stores small integers in the w lane as 0.5f with the value in the low 24 mantissa bits
				// (hkVector4::setInt24W).
				constexpr auto INT24_W_BASE = 0x3F000000u;
				constexpr auto SHAPE_INSTANCE_FLAGS_W = INT24_W_BASE | 0x40u;

				// Havok reads convex vertices four at a time: vertices and vertexEdges are padded to a multiple of four
				// by repeating the last entry.
				constexpr std::size_t padded_vertex_count(const std::size_t count)
				{
					return (count + 3) / 4 * 4;
				}

				// hknpShape header values, copied from every shipped ents blob.
				constexpr auto CONVEX_SHAPE_FLAGS = 0x0143u;
				constexpr auto CONVEX_DISPATCH_TYPE = 1;
				constexpr auto COMPOUND_SHAPE_FLAGS = 0x0004u;
				constexpr auto COMPOUND_DISPATCH_TYPE = 2;

				struct byte_buffer
				{
					std::vector<std::uint8_t> data;

					std::size_t size() const { return this->data.size(); }

					void align(const std::size_t alignment, const std::uint8_t fill = 0)
					{
						while (this->data.size() % alignment)
						{
							this->data.push_back(fill);
						}
					}

					void write(const void* src, const std::size_t count)
					{
						const auto* p = static_cast<const std::uint8_t*>(src);
						this->data.insert(this->data.end(), p, p + count);
					}

					template <typename T> void write(const T& value)
					{
						this->write(&value, sizeof(T));
					}

					void fill(const std::size_t count, const std::uint8_t value)
					{
						this->data.insert(this->data.end(), count, value);
					}

					// Reserve space and return the offset, for fields patched later.
					std::size_t reserve(const std::size_t count)
					{
						const auto offset = this->data.size();
						this->fill(count, 0);
						return offset;
					}

					template <typename T> void patch(const std::size_t offset, const T& value)
					{
						std::memcpy(this->data.data() + offset, &value, sizeof(T));
					}
				};

				// hkArray<T> is { T* m_data; int m_size; int m_capacityAndFlags; }. m_data is patched by a local fixup
				// and the flags word always carries DONT_DEALLOCATE.
				constexpr auto HK_ARRAY_DONT_DEALLOCATE = 0x80000000u;

				struct hk_array_ref
				{
					std::size_t field_offset = 0; // where the hkArray lives
					std::size_t data_offset = 0; // where its payload lives
					int count = 0;
				};

				void write_hk_array_header(byte_buffer& buf, const int count)
				{
					buf.reserve(8); // m_data, patched by a local fixup
					buf.write<std::int32_t>(count);
					buf.write<std::uint32_t>(count | HK_ARRAY_DONT_DEALLOCATE);
				}

				// Packed vertices are 11/11/10 bits in the owning section's codec space:
				//   pos[i] = codecParms[i] + raw[i] * codecParms[3 + i]
				constexpr int PACKED_BITS[3] = {11, 11, 10};

				// Shared vertices are quantised over the tree domain at 21/21/22 bits, the form stock world blobs use.
				constexpr int SHARED_BITS[3] = {21, 21, 22};

				std::uint64_t pack_shared_vertex(const float* pos, const float* mins,
					const float* maxs)
				{
					std::uint64_t packed = 0;
					auto shift = 0;

					for (auto i = 0; i < 3; i++)
					{
						const auto extent = maxs[i] - mins[i];
						const auto max_value = (1u << SHARED_BITS[i]) - 1u;
						// Havok's decoder scales by extent * 2^-bits, so encode against the
						// same step rather than extent / (2^bits - 1).
						const auto step = extent * std::ldexp(1.0f, -SHARED_BITS[i]);

						auto raw = 0u;
						if (step > 0.0f)
						{
							const auto q = std::lround((pos[i] - mins[i]) / step);
							raw = static_cast<std::uint32_t>(
								std::clamp<long>(q, 0, static_cast<long>(max_value)));
						}

						packed |= static_cast<std::uint64_t>(raw) << shift;
						shift += SHARED_BITS[i];
					}

					return packed;
				}

				// What the runtime reads back for a shared vertex. Flatness of a quad is judged on these positions.
				void unpack_shared_vertex(const std::uint64_t packed, const float* mins,
					const float* maxs, float(&out)[3])
				{
					auto shift = 0;
					for (auto i = 0; i < 3; i++)
					{
						const auto max_value = (1ull << SHARED_BITS[i]) - 1ull;
						const auto raw = (packed >> shift) & max_value;
						const auto step = (maxs[i] - mins[i]) * std::ldexp(1.0f, -SHARED_BITS[i]);
						out[i] = mins[i] + static_cast<float>(raw) * step;
						shift += SHARED_BITS[i];
					}
				}

				std::uint32_t pack_vertex(const float* pos, const float* codec_parms)
				{
					std::uint32_t packed = 0;
					int shift = 0;

					for (auto i = 0; i < 3; i++)
					{
						const auto scale = codec_parms[3 + i];
						const auto max_value = (1 << PACKED_BITS[i]) - 1;

						auto raw = 0;
						if (scale > 0.0f)
						{
							raw = static_cast<int>(std::lround((pos[i] - codec_parms[i]) / scale));
						}

						raw = std::clamp(raw, 0, max_value);
						packed |= static_cast<std::uint32_t>(raw) << shift;
						shift += PACKED_BITS[i];
					}

					return packed;
				}

				void unpack_vertex(const std::uint32_t packed, const float* codec_parms,
					float(&out)[3])
				{
					auto shift = 0;
					for (auto i = 0; i < 3; i++)
					{
						const auto mask = (1u << PACKED_BITS[i]) - 1u;
						const auto raw = (packed >> shift) & mask;
						out[i] = codec_parms[i]
							+ static_cast<float>(raw) * codec_parms[3 + i];
						shift += PACKED_BITS[i];
					}
				}

				enum class vertex_storage
				{
					shared,
					packed,
				};

				vertex_storage selected_vertex_storage()
				{
					const auto* env = std::getenv("ZT_HAVOK_VERTEX_STORAGE");
					if (!env || !env[0] || !std::strcmp(env, "shared"))
					{
						return vertex_storage::shared;
					}

					if (!std::strcmp(env, "packed"))
					{
						return vertex_storage::packed;
					}

					ZONETOOL_WARNING("havok: ZT_HAVOK_VERTEX_STORAGE=\"%s\" is invalid; "
						"using shared vertices", env);
					return vertex_storage::shared;
				}

				// Low nibble of a custom primitive's descriptor word. 2 is the convex form, whose vertex run follows in
				// the next word.
				constexpr auto CUSTOM_PRIMITIVE_CONVEX = 0x2u;
				constexpr auto MAX_CONVEX_VERTICES = 255u; // numVertices is the descriptor's high byte
				constexpr auto CONVEX_FLAT_TOLERANCE = 1e-4f; // Havok units
				// A section is addressed through ONE 65,536-vertex page of sharedVertices.
				constexpr std::size_t SHARED_VERTEX_PAGE_SIZE = 0x10000;

				// One local slot of a section's sharedVerticesIndex range.
				enum class slot_kind : std::uint8_t
				{
					vertex,        // a listed vertex; verts[slot] is its position
					convex_record, // word 0 of a convex record: the descriptor
					convex_start,  // word 1 of a convex record: the run's page-relative start
				};

				struct build_section
				{
					// One entry per local slot, record words included. A record slot holds a placeholder position that is
					// never welded onto or written to the pool.
					std::vector<std::array<float, 3>> verts;
					std::vector<slot_kind> slot_kinds;
					std::vector<std::array<std::uint8_t, 4>> primitives;
					std::vector<std::uint16_t> tags;
					std::vector<int> contents;
					std::vector<std::uint32_t> material_crcs;
					std::vector<std::uint64_t> user_data;
					std::vector<bool> quads;
					// Per primitive: -1 for a triangle or quad, otherwise the index into
					// `convexes` of the convex custom primitive it is.
					std::vector<int> custom_index;
					// The vertex runs of this section's convex customs, in primitive order.
					std::vector<std::vector<std::array<float, 3>>> convexes;
					float codec_parms[6] = {};
					float mins[3] = {};
					float maxs[3] = {};

					std::size_t listed_vertex_count() const
					{
						return static_cast<std::size_t>(std::count(slot_kinds.begin(),
							slot_kinds.end(), slot_kind::vertex));
					}

					// Listed vertices plus every convex run: this is what has to fit inside one page.
					std::size_t pool_vertex_count() const
					{
						auto count = listed_vertex_count();
						for (const auto& run : convexes)
						{
							count += run.size();
						}
						return count;
					}
				};

				void finalise_section_codec(build_section& section)
				{
					for (auto i = 0; i < 3; i++)
					{
						section.mins[i] = FLT_MAX;
						section.maxs[i] = -FLT_MAX;
					}

					// Explicit compares through one helper on purpose: std::min/std::max in a slot loop that skips record
					// slots is miscompiled by MSVC 2022 at /O2 (x86).
					const auto grow = [&section](const std::array<float, 3>& v)
					{
						for (auto i = 0; i < 3; i++)
						{
							if (v[i] < section.mins[i])
							{
								section.mins[i] = v[i];
							}
							if (v[i] > section.maxs[i])
							{
								section.maxs[i] = v[i];
							}
						}
					};

					for (auto s = 0u; s < section.verts.size(); s++)
					{
						if (section.slot_kinds[s] == slot_kind::vertex)
						{
							grow(section.verts[s]);
						}
					}

					// Convex runs are quantised over the tree domain, which is built from the section domains.
					for (const auto& run : section.convexes)
					{
						for (const auto& v : run)
						{
							grow(v);
						}
					}

					for (auto i = 0; i < 3; i++)
					{
						const auto extent = section.maxs[i] - section.mins[i];
						const auto max_value = static_cast<float>((1 << PACKED_BITS[i]) - 1);

						section.codec_parms[i] = section.mins[i];
						section.codec_parms[3 + i] = (extent > 0.0f) ? (extent / max_value) : 0.0f;
					}
				}

				// Every vertex goes in as a shared vertex with numPackedVertices 0, as most stock sections do.
				std::vector<build_section> split_into_sections(const mesh_input& input)
				{
					std::vector<build_section> sections;
					build_section current{};

					const auto flush = [&]
					{
						if (!current.primitives.empty())
						{
							finalise_section_codec(current);
							sections.emplace_back(std::move(current));
							current = {};
						}
					};

					auto degenerate_dropped = 0;

					for (const auto& tri : input.triangles)
					{
						// Two uint8 limits: the shared-index count and the primitive count. The +4 is the worst case for one quad.
						if (current.verts.size() + 4 > MAX_SHARED_INDICES_PER_SECTION ||
							current.primitives.size() >= MAX_PRIMITIVES_PER_SECTION)
						{
							flush();
						}

						// A primitive whose indices[1..3] are all equal is a custom primitive to Havok, and a triangle is emitted
						// as [a, b, c, c], so a degenerate triangle would be read as one. Tested on positions so no orphan vertex
						// is added.
						const auto same = [](const float* a, const float* b)
						{
							return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
						};
						if (same(tri.verts[0], tri.verts[1]) || same(tri.verts[1], tri.verts[2])
							|| same(tri.verts[0], tri.verts[2]))
						{
							degenerate_dropped++;
							continue;
						}

						// A quad's fourth corner must be distinct, or it would weld down to a triangle or the custom pattern.
						auto quad = tri.is_quad;
						if (quad && (same(tri.vert3, tri.verts[0]) || same(tri.vert3, tri.verts[1])
							|| same(tri.vert3, tri.verts[2])))
						{
							quad = false;
						}

						const float* corners[4] = {
							tri.verts[0], tri.verts[1], tri.verts[2], tri.vert3
						};

						std::array<std::uint8_t, 4> indices{};
						for (auto i = 0; i < (quad ? 4 : 3); i++)
						{
							auto found = -1;
							for (auto v = 0u; v < current.verts.size(); v++)
							{
								// Never weld onto a convex record's slot: it only carries a
								// placeholder position, and its word is not a vertex index.
								if (current.slot_kinds[v] == slot_kind::vertex &&
									current.verts[v][0] == corners[i][0] &&
									current.verts[v][1] == corners[i][1] &&
									current.verts[v][2] == corners[i][2])
								{
									found = static_cast<int>(v);
									break;
								}
							}

							if (found < 0)
							{
								found = static_cast<int>(current.verts.size());
								std::array<float, 3> v{};
								std::memcpy(v.data(), corners[i], sizeof(float[3]));
								current.verts.emplace_back(v);
								current.slot_kinds.emplace_back(slot_kind::vertex);
							}

							indices[i] = static_cast<std::uint8_t>(found);
						}

						// indices[2] == indices[3] marks a triangle; anything else is a quad.
						if (!quad)
						{
							indices[3] = indices[2];
						}

						current.primitives.emplace_back(indices);
						current.quads.emplace_back(quad);
						current.custom_index.emplace_back(-1);
						current.tags.emplace_back(tri.surface_tag);
						current.contents.emplace_back(tri.contents);
						current.material_crcs.emplace_back(tri.material_crc);
						current.user_data.emplace_back(tri.user_data);
					}

					// Convex custom primitives share sections with the triangles. Each costs one primitive and two local
					// shared-index slots (descriptor and start); its vertex run costs pool vertices but no slots.
					auto convexes_rejected = 0;
					std::map<std::string, int> rejection_reasons;

					for (const auto& cvx : input.convexes)
					{
						if (const auto* reason = convex_rejection(cvx.verts))
						{
							convexes_rejected++;
							rejection_reasons[reason]++;
							continue;
						}

						if (current.verts.size() + 2 > MAX_SHARED_INDICES_PER_SECTION ||
							current.primitives.size() >= MAX_PRIMITIVES_PER_SECTION ||
							current.pool_vertex_count() + cvx.verts.size() > SHARED_VERTEX_PAGE_SIZE)
						{
							flush();
						}

						const auto r = current.verts.size();
						if (r > MAX_VERTEX_INDEX)
						{
							ZONETOOL_ERROR("havok: convex record slot %zu does not fit a uint8", r);
							continue;
						}

						current.verts.emplace_back(cvx.verts.front());
						current.slot_kinds.emplace_back(slot_kind::convex_record);
						current.verts.emplace_back(cvx.verts.front());
						current.slot_kinds.emplace_back(slot_kind::convex_start);

						const auto rb = static_cast<std::uint8_t>(r);
						current.primitives.push_back({rb, rb, rb, rb});
						current.quads.emplace_back(false);
						current.custom_index.emplace_back(static_cast<int>(current.convexes.size()));
						current.convexes.emplace_back(cvx.verts);
						current.tags.emplace_back(cvx.surface_tag);
						current.contents.emplace_back(cvx.contents);
						current.material_crcs.emplace_back(cvx.material_crc);
						current.user_data.emplace_back(cvx.user_data);
					}

					flush();

					if (degenerate_dropped)
					{
						ZONETOOL_INFO("havok: dropped %d degenerate triangle(s); emitting them "
							"would have produced primitives the runtime reads as custom "
							"primitives", degenerate_dropped);
					}

					if (convexes_rejected)
					{
						std::string reasons;
						for (const auto& [reason, count] : rejection_reasons)
						{
							reasons += (reasons.empty() ? "" : ", ") + std::to_string(count)
								+ " " + reason;
						}
						ZONETOOL_WARNING("havok: rejected %d of %zu convex(es), which are NOT in "
							"the mesh (%s)", convexes_rejected, input.convexes.size(),
							reasons.c_str());
					}

					return sections;
				}

				// Both BVH levels are binary trees in preorder with numNodes == 2 * numLeaves - 1.
				//   section (4 bytes): internal when (data & 1), rightChild = n + (data & 0xFE)
				//   top (5 bytes): internal when (hiData & 0x80), rightChild = n + 2 * (((hiData & 0x7F) << 8) | loData);
				//                  leaf: section index in (hiData << 8) | loData

				// Each node's xyz bytes hold two 4-bit insets from the parent box, quadratic in the nibble:
				//   childMin = parentMin + hi*hi * extent / 226, childMax = parentMax - lo*lo * extent / 226
				// The divisor is 226, not 225. Each nibble is the largest value that keeps the decoded box around the child.

				// IW7 only stores contents bits inside 0xC7FFBFFF. CONTENTS_DETAIL (0x08000000), set on most IW3/IW5
				// brushes, makes its collision filter reject the surface. The world shape's own contents is a fixed
				// constant and is not masked.
				constexpr auto CONTENTS_FILTER_MASK = 0xC7FFBFFFu;

				inline std::uint32_t filter_contents(const std::uint32_t contents)
				{
					return contents & CONTENTS_FILTER_MASK;
				}

				// XModel LOD tag tables are the one place stock keeps 0x20000000.
				inline std::uint32_t filter_lod_contents(const std::uint32_t contents)
				{
					return contents & (CONTENTS_FILTER_MASK | 0x20000000u);
				}

				constexpr auto NIBBLE_MAX = 15;
				constexpr auto NIBBLE_SCALE = 226.0f; // Havok's divisor; see the note above

				// Diagnostic: zero-inset nodes decode to their parent box, which turns tree culling off without changing
				// the mesh.
				bool loose_bvh_enabled()
				{
					const auto* env = std::getenv("ZT_HAVOK_BVH_LOOSE");
					return env && env[0] == '1';
				}

				// Largest n in [0,15] with (n*n / 226) * extent <= inset.
				int quantise_inset(const float inset, const float extent)
				{
					if (!(extent > 0.0f) || !(inset > 0.0f))
					{
						return 0;
					}

					const auto ratio = std::clamp(inset / extent, 0.0f, 1.0f);
					auto n = static_cast<int>(std::floor(std::sqrt(ratio * NIBBLE_SCALE)));
					n = std::clamp(n, 0, NIBBLE_MAX);

					// Round down until the inset genuinely fits, guarding float error.
					while (n > 0 && (static_cast<float>(n * n) / NIBBLE_SCALE) * extent > inset)
					{
						n--;
					}
					return n;
				}

				float decode_inset(const int nibble, const float extent)
				{
					return (static_cast<float>(nibble * nibble) / NIBBLE_SCALE) * extent;
				}

				struct aabb
				{
					float lo[3];
					float hi[3];

					void reset()
					{
						for (auto i = 0; i < 3; i++)
						{
							lo[i] = FLT_MAX;
							hi[i] = -FLT_MAX;
						}
					}

					void add(const aabb& other)
					{
						for (auto i = 0; i < 3; i++)
						{
							lo[i] = std::min(lo[i], other.lo[i]);
							hi[i] = std::max(hi[i], other.hi[i]);
						}
					}
				};

				// Writes the three xyz bytes for `child` relative to `parent`, and returns the
				// box the decoder will actually reconstruct -- which is what the child's own
				// children must then be encoded against.
				aabb encode_node_aabb(std::uint8_t* xyz, const aabb& parent, const aabb& child)
				{
					if (loose_bvh_enabled())
					{
						xyz[0] = xyz[1] = xyz[2] = 0;
						return parent;
					}

					aabb decoded{};
					for (auto i = 0; i < 3; i++)
					{
						const auto extent = parent.hi[i] - parent.lo[i];
						const auto hi_n = quantise_inset(child.lo[i] - parent.lo[i], extent);
						const auto lo_n = quantise_inset(parent.hi[i] - child.hi[i], extent);

						xyz[i] = static_cast<std::uint8_t>((hi_n << 4) | lo_n);
						decoded.lo[i] = parent.lo[i] + decode_inset(hi_n, extent);
						decoded.hi[i] = parent.hi[i] - decode_inset(lo_n, extent);
					}
					return decoded;
				}

				// leaf_nodes[sectionIndex] is the node holding that section's leaf, which is what the section's leafIndex
				// stores.
				void emit_preorder_tree_top(byte_buffer& buf, const int first_leaf,
					const int leaf_count, std::vector<int>& leaf_nodes,
					const std::vector<aabb>& leaf_boxes, const aabb& parent)
				{
					const auto self = static_cast<int>(buf.size() / SIZEOF_NODE_TOP);

					aabb box{};
					box.reset();
					for (auto i = 0; i < leaf_count; i++)
					{
						box.add(leaf_boxes[first_leaf + i]);
					}

					std::uint8_t xyz[3] = {};
					const auto decoded = encode_node_aabb(xyz, parent, box);

					if (leaf_count <= 1)
					{
						// Leaf: section index in (hiData << 8) | loData.
						leaf_nodes[first_leaf] = self;
						buf.write(xyz, sizeof(xyz));
						buf.write<std::uint8_t>(static_cast<std::uint8_t>((first_leaf >> 8) & 0x7F));
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(first_leaf & 0xFF));
						return;
					}

					const auto left_count = leaf_count / 2;
					const auto right_count = leaf_count - left_count;

					// Left subtree occupies (2 * left_count - 1) nodes immediately after
					// this one, so the right child sits at that offset + 1.
					const auto right_offset = 2 * left_count;
					const auto encoded = right_offset / 2;

					buf.write(xyz, sizeof(xyz));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(0x80 | ((encoded >> 8) & 0x7F)));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(encoded & 0xFF));

					emit_preorder_tree_top(buf, first_leaf, left_count, leaf_nodes,
						leaf_boxes, decoded);
					emit_preorder_tree_top(buf, first_leaf + left_count, right_count, leaf_nodes,
						leaf_boxes, decoded);
				}

				void emit_preorder_tree_section(byte_buffer& buf, const int first_leaf,
					const int leaf_count, const std::vector<aabb>& leaf_boxes, const aabb& parent)
				{
					aabb box{};
					box.reset();
					for (auto i = 0; i < leaf_count; i++)
					{
						box.add(leaf_boxes[first_leaf + i]);
					}

					std::uint8_t xyz[3] = {};
					const auto decoded = encode_node_aabb(xyz, parent, box);

					if (leaf_count <= 1)
					{
						// A section leaf's data byte is (primitiveIndex << 1): bit 0 clear marks the leaf.
						if (first_leaf > 0x7F)
						{
							ZONETOOL_ERROR("havok: section BVH leaf primitive index %d does "
								"not fit in the 7-bit field", first_leaf);
						}

						buf.write(xyz, sizeof(xyz));
						buf.write<std::uint8_t>(
							static_cast<std::uint8_t>((first_leaf << 1) & 0xFE));
						return;
					}

					const auto left_count = leaf_count / 2;
					const auto right_count = leaf_count - left_count;
					const auto right_offset = 2 * left_count;

					if (right_offset > 0xFE)
					{
						// split_into_sections keeps sections small enough for one byte.
						ZONETOOL_ERROR("havok: section BVH right-child offset %d exceeds the "
							"8-bit field", right_offset);
					}

					buf.write(xyz, sizeof(xyz));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>((right_offset & 0xFE) | 1));

					emit_preorder_tree_section(buf, first_leaf, left_count, leaf_boxes, decoded);
					emit_preorder_tree_section(buf, first_leaf + left_count, right_count,
						leaf_boxes, decoded);
				}

				// hknpCompressedMeshShapeTreeDataRun is { uint16 value; uint8 index; uint8 count }: value applies to
				// count primitives starting at index within the section.
				void emit_data_runs(byte_buffer& buf, const std::vector<std::uint16_t>& tags,
					int& runs_written)
				{
					runs_written = 0;

					for (auto i = 0u; i < tags.size();)
					{
						auto j = i;
						while (j < tags.size() && tags[j] == tags[i] && (j - i) < 0xFF)
						{
							j++;
						}

						buf.write<std::uint16_t>(tags[i]);
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(i));
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(j - i));

						runs_written++;
						i = j;
					}
				}
			}

			// Shared by the world blob and per-model physics assets, which differ only in what wraps the mesh.
			const char* convex_rejection(const std::vector<std::array<float, 3>>& verts)
			{
				if (verts.size() < 4)
				{
					return "fewer than 4 vertices";
				}
				if (verts.size() > MAX_CONVEX_VERTICES)
				{
					return "more than 255 vertices";
				}

				for (auto i = 0u; i < verts.size(); i++)
				{
					for (auto j = i + 1; j < verts.size(); j++)
					{
						if (verts[i] == verts[j])
						{
							return "repeated vertices";
						}
					}
				}

				// Flat: every vertex within CONVEX_FLAT_TOLERANCE of one plane through a vertex triple. Each candidate
				// plane gives an upper bound on the true minimum width, so anything reported flat is flat.
				const auto sub = [](const std::array<float, 3>& a, const std::array<float, 3>& b)
				{
					return std::array<double, 3>{
						static_cast<double>(a[0]) - b[0],
						static_cast<double>(a[1]) - b[1],
						static_cast<double>(a[2]) - b[2]};
				};
				const auto cross = [](const std::array<double, 3>& a, const std::array<double, 3>& b)
				{
					return std::array<double, 3>{
						a[1] * b[2] - a[2] * b[1],
						a[2] * b[0] - a[0] * b[2],
						a[0] * b[1] - a[1] * b[0]};
				};
				const auto dot = [](const std::array<double, 3>& a, const std::array<double, 3>& b)
				{
					return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
				};

				// Max distance of any vertex from the plane through a, b, c; negative if the
				// triple is collinear and defines no plane.
				const auto width_for = [&](const std::size_t a, const std::size_t b,
					const std::size_t c) -> double
				{
					auto n = cross(sub(verts[b], verts[a]), sub(verts[c], verts[a]));
					const auto length = std::sqrt(dot(n, n));
					if (length < 1e-12)
					{
						return -1.0;
					}
					auto width = 0.0;
					for (const auto& v : verts)
					{
						width = std::max(width, std::fabs(dot(sub(v, verts[a]), n)) / length);
					}
					return width;
				};

				auto any_plane = false;
				if (verts.size() <= 32)
				{
					for (auto a = 0u; a < verts.size(); a++)
					{
						for (auto b = a + 1; b < verts.size(); b++)
						{
							for (auto c = b + 1; c < verts.size(); c++)
							{
								const auto width = width_for(a, b, c);
								if (width < 0.0)
								{
									continue;
								}
								any_plane = true;
								if (width <= CONVEX_FLAT_TOLERANCE)
								{
									return "flat within 1e-4";
								}
							}
						}
					}
				}
				else
				{
					std::size_t b = 0, c = 0;
					auto best = -1.0;
					for (auto i = 1u; i < verts.size(); i++)
					{
						const auto d = sub(verts[i], verts[0]);
						if (dot(d, d) > best)
						{
							best = dot(d, d);
							b = i;
						}
					}
					best = -1.0;
					const auto axis = sub(verts[b], verts[0]);
					for (auto i = 1u; i < verts.size(); i++)
					{
						const auto n = cross(axis, sub(verts[i], verts[0]));
						if (dot(n, n) > best)
						{
							best = dot(n, n);
							c = i;
						}
					}
					const auto width = width_for(0, b, c);
					if (width >= 0.0)
					{
						any_plane = true;
						if (width <= CONVEX_FLAT_TOLERANCE)
						{
							return "flat within 1e-4";
						}
					}
				}

				if (!any_plane)
				{
					return "collinear";
				}

				return nullptr;
			}

			std::vector<std::uint8_t> build_mesh_blob(const mesh_input& input,
				const physics_asset_input* physics_asset, const std::string* xmodel_lod_bone = nullptr,
				std::vector<shape_tag>* out_tags = nullptr)
			{
				if (input.triangles.empty() && input.convexes.empty())
				{
					ZONETOOL_ERROR("havok: refusing to build a mesh blob from 0 triangles "
						"and 0 convexes");
					return {};
				}

				auto sections = split_into_sections(input);
				if (sections.empty())
				{
					ZONETOOL_ERROR("havok: no sections produced");
					return {};
				}

				std::size_t total_customs = 0;
				for (const auto& section : sections)
				{
					total_customs += section.convexes.size();
				}

				auto vertex_format = selected_vertex_storage();
				if (vertex_format == vertex_storage::packed && total_customs)
				{
					// Convex records and runs live in the shared vertex arrays, which the packed path does not have.
					ZONETOOL_WARNING("havok: ZT_HAVOK_VERTEX_STORAGE=packed cannot carry %zu "
						"convex custom primitive(s); using shared vertices for this blob",
						total_customs);
					vertex_format = vertex_storage::shared;
				}
				ZONETOOL_INFO("havok: vertex storage %s (ZT_HAVOK_VERTEX_STORAGE)",
					vertex_format == vertex_storage::packed ? "packed" : "shared");

				// primitiveDataRuns.value indexes shapeTagData. A tag is (contents, materialCRC, userData): surfaces
				// differing in any of the three must not share one.
				std::vector<std::tuple<int, std::uint32_t, std::uint64_t>> palette;

				for (auto& section : sections)
				{
					for (auto i = 0u; i < section.tags.size(); i++)
					{
						const auto key = std::make_tuple(section.contents[i],
							section.material_crcs[i], section.user_data[i]);

						auto it = std::find(palette.begin(), palette.end(), key);
						if (it == palette.end())
						{
							palette.emplace_back(key);
							it = palette.end() - 1;
						}
						section.tags[i] = static_cast<unsigned short>(
							std::distance(palette.begin(), it));
					}
				}

				// The palette as the 24-byte records written below. out_tags hands back the same vector, so contents
				// are masked once.
				std::vector<shape_tag> tag_records;
				tag_records.reserve(palette.size());
				for (const auto& entry : palette)
				{
					tag_records.emplace_back(shape_tag{
						filter_contents(static_cast<std::uint32_t>(std::get<0>(entry))),
						std::get<1>(entry), std::get<2>(entry)});
				}

				// A primitive key packs (section, primitive, triangle within primitive). triangleIsInterior has one bit
				// per key, quadIsFlat one per primitive slot.
				const auto section_count = static_cast<int>(sections.size());

				// The largest key comes from the widest section, not the last one.
				auto max_key = 0;
				for (auto si = 0u; si < sections.size(); si++)
				{
					const auto last = static_cast<int>(sections[si].primitives.size()) - 1;
					const auto key = static_cast<int>(si << KEY_SECTION_SHIFT) | (last << 1) | 1;
					max_key = std::max(max_key, key);
				}

				auto bits_per_key = 1;
				while ((1 << bits_per_key) <= max_key)
				{
					bits_per_key++;
				}

				std::size_t total_prims = 0;
				std::size_t total_verts = 0;
				for (const auto& section : sections)
				{
					total_prims += section.primitives.size();
					// Pool vertices this mesh actually emits -- listed vertices plus convex
					// runs, excluding page padding. Record slots are not vertices.
					total_verts += section.pool_vertex_count();
				}

				// A triangle owns one key, a quad two, a convex custom one. mesh_triangles counts triangles only (quads
				// as two).
				auto num_primitive_keys = 0;
				auto mesh_triangles = 0;
				for (const auto& section : sections)
				{
					for (auto pi = 0u; pi < section.primitives.size(); pi++)
					{
						if (section.custom_index[pi] >= 0)
						{
							num_primitive_keys += 1;
							continue;
						}
						const auto keys = section.quads[pi] ? 2 : 1;
						num_primitive_keys += keys;
						mesh_triangles += keys;
					}
				}

				float world_min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
				float world_max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
				for (const auto& section : sections)
				{
					for (auto i = 0; i < 3; i++)
					{
						world_min[i] = std::min(world_min[i], section.mins[i]);
						world_max[i] = std::max(world_max[i], section.maxs[i]);
					}
				}

				aabb world_box{};
				for (auto i = 0; i < 3; i++)
				{
					world_box.lo[i] = world_min[i];
					world_box.hi[i] = world_max[i];
				}

				// Per-section boxes drive the top-level tree's quantisation.
				std::vector<aabb> section_boxes(sections.size());
				for (auto i = 0u; i < sections.size(); i++)
				{
					for (auto c = 0; c < 3; c++)
					{
						section_boxes[i].lo[c] = sections[i].mins[c];
						section_boxes[i].hi[c] = sections[i].maxs[c];
					}
				}

				byte_buffer top_nodes;
				std::vector<int> leaf_nodes(sections.size(), 0);
				emit_preorder_tree_top(top_nodes, 0, section_count, leaf_nodes,
					section_boxes, world_box);

				std::vector<byte_buffer> section_nodes(sections.size());
				for (auto i = 0u; i < sections.size(); i++)
				{
					const auto& section = sections[i];

					// Per-primitive boxes drive the section tree's quantisation.
					std::vector<aabb> prim_boxes(section.primitives.size());
					for (auto p = 0u; p < section.primitives.size(); p++)
					{
						prim_boxes[p].reset();
						// A convex custom's leaf box is the AABB of its vertex run, from the
						// same source floats the triangle boxes use.
						if (section.custom_index[p] >= 0)
						{
							for (const auto& v : section.convexes[section.custom_index[p]])
							{
								aabb point{};
								for (auto c = 0; c < 3; c++)
								{
									point.lo[c] = v[c];
									point.hi[c] = v[c];
								}
								prim_boxes[p].add(point);
							}
							continue;
						}
						// Four corners for a quad, three for a triangle -- a quad's fourth
						// corner is outside the box its first three describe.
						for (auto k = 0; k < (section.quads[p] ? 4 : 3); k++)
						{
							const auto& v = section.verts[section.primitives[p][k]];
							aabb point{};
							for (auto c = 0; c < 3; c++)
							{
								point.lo[c] = v[c];
								point.hi[c] = v[c];
							}
							prim_boxes[p].add(point);
						}
					}

					emit_preorder_tree_section(section_nodes[i], 0,
						static_cast<int>(section.primitives.size()),
						prim_boxes, section_boxes[i]);
				}

				byte_buffer primitives;
				byte_buffer packed_vertices;
				byte_buffer shared_vertices;
				byte_buffer shared_vertex_index;
				byte_buffer data_runs;
				std::vector<int> section_first_prim(sections.size());
				std::vector<int> section_first_packed(sections.size());
				std::vector<int> section_first_vert(sections.size());
				std::vector<int> section_page(sections.size());
				std::vector<int> section_first_run(sections.size());
				std::vector<int> section_run_count(sections.size());
				auto quantised_duplicate_runs = 0;

				for (auto i = 0u; i < sections.size(); i++)
				{
					auto& section = sections[i];

					section_first_prim[i] = static_cast<int>(primitives.size() / SIZEOF_PRIMITIVE);
					for (const auto& prim : section.primitives)
					{
						primitives.write(prim.data(), 4);
					}

					if (vertex_format == vertex_storage::packed)
					{
						// Packed vertices use the section-local 11/11/10 codec, with no shared-index table or page.
						section_first_packed[i] =
							static_cast<int>(packed_vertices.size() / sizeof(std::uint32_t));
						for (const auto& vertex : section.verts)
						{
							packed_vertices.write<std::uint32_t>(
								pack_vertex(vertex.data(), section.codec_parms));
						}
						section_first_vert[i] = 0;
						section_page[i] = 0;
					}
					else
					{
						// The runtime resolves a shared vertex as sharedVertices[0x10000 * section.page + sharedVerticesIndex[..]],
						// so a section's vertices and convex runs must sit inside one 65,536-vertex page. The pool is padded up
						// to the next page when a section would cross one.
						auto vert_base = shared_vertices.size() / 8;
						const auto vert_needed = section.pool_vertex_count();

						if (vert_needed > 0
							&& ((vert_base + vert_needed - 1) >> 16) != (vert_base >> 16))
						{
							const auto next_page = ((vert_base >> 16) + 1) << 16;
							while (shared_vertices.size() / 8 < next_page)
							{
								shared_vertices.write<std::uint64_t>(0);
							}
							vert_base = next_page;
						}

						section_page[i] = static_cast<int>(vert_base >> 16);

						if (section_page[i] > 0xFF)
						{
							ZONETOOL_ERROR("havok: section %u needs vertex page %d, but the page "
								"field is a uint8 -- the mesh has more than %u shared vertices",
								i, section_page[i], 0x100u << 16);
							return {};
						}

					// firstSharedVertexIndex indexes sharedVerticesIndex, not the vertex pool,
					// and is packed into 24 bits alongside numPackedVertices.
						section_first_vert[i] = static_cast<int>(shared_vertex_index.size() / 2);

						if (section_first_vert[i] > 0xFFFFFF)
						{
							ZONETOOL_ERROR("havok: sharedVerticesIndex has %d entries, which does "
								"not fit the section's 24-bit first-index field",
								section_first_vert[i]);
							return {};
						}

						const auto page_base = static_cast<std::size_t>(section_page[i]) << 16;

						// Where each convex run lands: straight after the listed vertices, in
						// primitive order.
						const auto listed = section.listed_vertex_count();
						std::vector<std::size_t> run_start(section.convexes.size());
						{
							auto next = vert_base + listed;
							for (auto c = 0u; c < section.convexes.size(); c++)
							{
								run_start[c] = next;
								next += section.convexes[c].size();
							}
						}

						// sharedVerticesIndex in local slot order. Record slots map to the
						// custom that owns them through the primitive whose indices name them.
						std::vector<int> slot_custom(section.verts.size(), -1);
						for (auto p = 0u; p < section.primitives.size(); p++)
						{
							const auto custom = section.custom_index[p];
							if (custom >= 0)
							{
								slot_custom[section.primitives[p][0]] = custom;
								slot_custom[section.primitives[p][0] + 1u] = custom;
							}
						}

						auto listed_written = 0u;
						for (auto v = 0u; v < section.verts.size(); v++)
						{
							switch (section.slot_kinds[v])
							{
							case slot_kind::vertex:
								shared_vertices.write<std::uint64_t>(
									pack_shared_vertex(section.verts[v].data(), world_min, world_max));
								shared_vertex_index.write<std::uint16_t>(
									static_cast<std::uint16_t>(vert_base + listed_written - page_base));
								listed_written++;
								break;
							case slot_kind::convex_record:
								shared_vertex_index.write<std::uint16_t>(static_cast<std::uint16_t>(
									(section.convexes[slot_custom[v]].size() << 8)
									| CUSTOM_PRIMITIVE_CONVEX));
								break;
							case slot_kind::convex_start:
								shared_vertex_index.write<std::uint16_t>(static_cast<std::uint16_t>(
									run_start[slot_custom[v]] - page_base));
								break;
							}
						}

						// Listed vertices are in the pool; now the runs, which nothing in
						// sharedVerticesIndex lists beyond their record.
						for (const auto& run : section.convexes)
						{
							std::vector<std::uint64_t> packed_run;
							packed_run.reserve(run.size());
							for (const auto& vertex : run)
							{
								packed_run.emplace_back(
									pack_shared_vertex(vertex.data(), world_min, world_max));
								shared_vertices.write<std::uint64_t>(packed_run.back());
							}

							// Stock runs are distinct points. The input is (convex_rejection), but
							// quantisation over a very large domain could still merge two.
							std::sort(packed_run.begin(), packed_run.end());
							if (std::adjacent_find(packed_run.begin(), packed_run.end())
								!= packed_run.end())
							{
								quantised_duplicate_runs++;
							}
						}

						if (shared_vertices.size() / 8 != vert_base + vert_needed)
						{
							ZONETOOL_ERROR("havok: section %u wrote %zu pool vertices, expected %zu",
								i, shared_vertices.size() / 8 - vert_base, vert_needed);
							return {};
						}
					}

					section_first_run[i] = static_cast<int>(data_runs.size() / SIZEOF_DATA_RUN);
					auto runs = 0;
					if (physics_asset)
					{
						// A physics asset has no shapeTagData and IW7 decodes tags against the map's table, so the tag is 0xFFFF
						// (none).
						const std::vector<std::uint16_t> untagged(section.tags.size(), 0xFFFF);
						emit_data_runs(data_runs, untagged, runs);
					}
					else
					{
						emit_data_runs(data_runs, section.tags, runs);
					}
					section_run_count[i] = runs;
				}

				if (quantised_duplicate_runs)
				{
					ZONETOOL_WARNING("havok: %d convex vertex run(s) have two vertices that "
						"quantise to the same shared vertex over this tree domain",
						quantised_duplicate_runs);
				}

				// simdTree is a mandatory 4-wide BVH over primitives; IW7 raycasts go through it. A node is hkcdFourAabb
				// (lx, hx, ly, hy, lz, hz) then uint32 data[4]: leaf when (data & 1), with the key or child index in
				// data >> 1. Node 0 is an inverted sentinel and node 1 the root.

				struct simd_node
				{
					float lo[3][4];
					float hi[3][4];
					std::uint32_t data[4];
				};

				struct simd_item
				{
					std::uint32_t key;
					float lo[3];
					float hi[3];
				};

				std::vector<simd_item> items;
				items.reserve(total_prims);
				for (auto si = 0u; si < sections.size(); si++)
				{
					const auto& section = sections[si];
					for (auto pi = 0u; pi < section.primitives.size(); pi++)
					{
						// One leaf per primitive, not per triangle, with the low key bit clear.
						simd_item item{};
						item.key = static_cast<std::uint32_t>(
							(si << KEY_SECTION_SHIFT) | (pi << 1));
						for (auto c = 0; c < 3; c++)
						{
							item.lo[c] = FLT_MAX;
							item.hi[c] = -FLT_MAX;
						}
						const auto grow = [&item](const std::array<float, 3>& v)
						{
							for (auto c = 0; c < 3; c++)
							{
								item.lo[c] = std::min(item.lo[c], v[c]);
								item.hi[c] = std::max(item.hi[c], v[c]);
							}
						};
						if (section.custom_index[pi] >= 0)
						{
							// One leaf per convex custom, boxed by its vertex run.
							for (const auto& v : section.convexes[section.custom_index[pi]])
							{
								grow(v);
							}
						}
						else
						{
							for (auto k = 0; k < (section.quads[pi] ? 4 : 3); k++)
							{
								grow(section.verts[section.primitives[pi][k]]);
							}
						}
						items.emplace_back(item);
					}
				}

				std::vector<simd_node> simd_nodes;
				simd_nodes.reserve(items.size());

				const auto clear_node = [](simd_node& node)
				{
					for (auto c = 0; c < 3; c++)
					{
						for (auto s = 0; s < 4; s++)
						{
							node.lo[c][s] = FLT_MAX;
							node.hi[c][s] = -FLT_MAX;
						}
					}
					node.data[0] = node.data[1] = node.data[2] = node.data[3] = 0;
				};

				simd_nodes.emplace_back();
				clear_node(simd_nodes.back()); // node 0: sentinel

				// Recursive 4-way split on the longest axis. Returns the node index.
				const std::function<int(std::vector<simd_item>&)> build_simd =
					[&](std::vector<simd_item>& group) -> int
				{
					const auto self = static_cast<int>(simd_nodes.size());
					simd_nodes.emplace_back();
					clear_node(simd_nodes.back());

					const auto set_slot = [&](const int slot, const float* lo, const float* hi,
						const std::uint32_t data)
					{
						auto& node = simd_nodes[self];
						for (auto c = 0; c < 3; c++)
						{
							node.lo[c][slot] = lo[c];
							node.hi[c][slot] = hi[c];
						}
						node.data[slot] = data;
					};

					if (group.size() <= 4)
					{
						for (auto i = 0u; i < group.size(); i++)
						{
							set_slot(static_cast<int>(i), group[i].lo, group[i].hi,
								(group[i].key << 1) | 1);
						}
						return self;
					}

					float lo[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
					float hi[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
					for (const auto& item : group)
					{
						for (auto c = 0; c < 3; c++)
						{
							lo[c] = std::min(lo[c], item.lo[c]);
							hi[c] = std::max(hi[c], item.hi[c]);
						}
					}

					auto axis = 0;
					for (auto c = 1; c < 3; c++)
					{
						if ((hi[c] - lo[c]) > (hi[axis] - lo[axis]))
						{
							axis = c;
						}
					}

					std::sort(group.begin(), group.end(),
						[axis](const simd_item& a, const simd_item& b)
						{
							return (a.lo[axis] + a.hi[axis]) < (b.lo[axis] + b.hi[axis]);
						});

					const auto total = group.size();
					for (auto slot = 0; slot < 4; slot++)
					{
						const auto begin = total * slot / 4;
						const auto end = total * (slot + 1) / 4;
						if (begin >= end)
						{
							continue;
						}

						std::vector<simd_item> sub(group.begin() + begin, group.begin() + end);

						float slo[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
						float shi[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
						for (const auto& item : sub)
						{
							for (auto c = 0; c < 3; c++)
							{
								slo[c] = std::min(slo[c], item.lo[c]);
								shi[c] = std::max(shi[c], item.hi[c]);
							}
						}

						if (sub.size() == 1)
						{
							set_slot(slot, slo, shi, (sub[0].key << 1) | 1);
						}
						else
						{
							const auto child = build_simd(sub);
							set_slot(slot, slo, shi, static_cast<std::uint32_t>(child) << 1);
						}
					}

					return self;
				};

				build_simd(items); // becomes node 1

				byte_buffer buf;

				struct pending_fixup
				{
					std::size_t src;
					std::size_t dst;
				};
				std::vector<pending_fixup> local_fixups;
				std::vector<std::array<int, 3>> global_fixups;
				std::vector<std::pair<std::size_t, int>> virtual_fixups;

				const auto align16 = [&] { buf.align(16, 0); };

				// The mesh is reached through one of two containers. Slots that must end up
				// pointing at the shape are collected here so the mesh emitter below can patch
				// them without caring which container it is inside.
				std::vector<std::size_t> shape_ptr_slots;
				std::size_t shape_list_offset = 0;

				// Class indices for the virtual fixup table, in the order the name table below
				// writes them.
				const int class_shape = physics_asset ? 2 : 1;
				const int class_shape_data = physics_asset ? 3 : 2;

				// Reported at the end; only the shape-list container actually writes them.
				const auto tag_count = static_cast<int>(palette.size());
				// shapeContents becomes the body's collisionFilterInfo, and a query filters on the body before any
				// surface tag. It has to be a superset of the tags inside it, so their contents are OR-ed in
				// (ZT_HAVOK_WORLD_CONTENTS overrides).
				auto world_contents = WORLD_SHAPE_CONTENTS;
				for (const auto& tag : tag_records)
				{
					world_contents |= tag.collision_filter;
				}

				if (const auto* env = std::getenv("ZT_HAVOK_WORLD_CONTENTS"))
				{
					world_contents = static_cast<std::uint32_t>(std::strtoul(env, nullptr, 16));
					ZONETOOL_INFO("havok: world shape contents overridden to 0x%08X",
						world_contents);
				}

				if (!physics_asset && !xmodel_lod_bone)
				{
					// --- HavokPhysicsShapeList (152 bytes) ---
					// Assigns the outer shape_list_offset, which the virtual fixup below reads.
					shape_list_offset = buf.size();

					const std::array<int, 10> member_counts = {
						1, 1, 1, 1, 1, 2, 0, tag_count, 1, 1
					};

					std::array<std::size_t, 10> member_field{};
					for (auto i = 0; i < 6; i++)
					{
						member_field[i] = buf.size();
						write_hk_array_header(buf, member_counts[i]);
					}
					// numWorldGeoShapes is 0 in every shipped blob.
					buf.write<std::int32_t>(0); // numWorldGeoShapes
					buf.reserve(4); // pad to +104
					for (auto i = 7; i < 10; i++)
					{
						member_field[i] = buf.size();
						write_hk_array_header(buf, member_counts[i]);
					}

					if (buf.size() - shape_list_offset != 152)
					{
						ZONETOOL_ERROR("havok: HavokPhysicsShapeList is %zu bytes, expected 152",
							buf.size() - shape_list_offset);
						return {};
					}

					align16();

					const auto record_array = [&](const int index, const std::size_t payload)
					{
						local_fixups.push_back({member_field[index], payload});
					};

					const auto shapes_payload = buf.size();
					record_array(0, shapes_payload);
					const auto shape_ptr_slot = buf.reserve(8);
					align16();

					record_array(1, buf.size());
					buf.write<std::int32_t>(0);
					align16();

					// shapeNames is an hkStringPtr: a pointer to the string bytes.
					record_array(2, buf.size());
					const auto name_ptr_slot = buf.reserve(8);
					align16();
					const auto name_payload = buf.size();
					const char* world_name = "World Entity 0";
					buf.write(world_name, std::strlen(world_name) + 1);
					align16();
					local_fixups.push_back({name_ptr_slot, name_payload});

					record_array(3, buf.size());
					// What vertCounts counts in stock is not known. This writes the vertices the mesh emits, page padding
					// excluded.
					buf.write<std::int32_t>(static_cast<std::int32_t>(total_verts));
					align16();
					// Triangle count (quads as two), without convex customs, which convexCounts carries.
					record_array(4, buf.size());
					buf.write<std::int32_t>(static_cast<std::int32_t>(mesh_triangles));
					align16();

					// minMaxes: two hkVector4f, min then max.
					record_array(5, buf.size());
					for (auto i = 0; i < 3; i++) buf.write<float>(world_min[i]);
					buf.write<float>(0.0f);
					for (auto i = 0; i < 3; i++) buf.write<float>(world_max[i]);
					buf.write<float>(0.0f);
					align16();

					record_array(7, buf.size());
					for (auto i = 0u; i < palette.size(); i++)
					{
						// materialId is 0xFFFF in every shipped entry: materialCRC identifies the material. collisionFilterInfo
						// is the CoD contents mask. userData holds the surface flags in the low 32 bits, a 1-based glass piece
						// index in bits 32..39 and the brush basis in bit 48; without that bit the player's cast takes the
						// non-brush path and passes through walls.
						buf.write<std::uint32_t>(tag_records[i].collision_filter);
						buf.write<std::uint32_t>(tag_records[i].material_crc);
						buf.write<std::uint16_t>(0xFFFF); // materialId -- "none"
						buf.reserve(6); // pad, userData is at +16
						buf.write<std::uint64_t>(tag_records[i].user_data);
					}
					align16();

					// The world shape's contents is a fixed value in stock, not the OR of the map's surfaces: an OR of
					// IW3/IW5 contents lacks bits the player query filters on.

					record_array(8, buf.size());
					buf.write<std::uint32_t>(world_contents);
					align16();
					record_array(9, buf.size());
					// Number of convex custom primitives emitted, not the mesh's numConvexShapes member.
					buf.write<std::int32_t>(static_cast<std::int32_t>(total_customs));
					align16();
					shape_ptr_slots.push_back(shape_ptr_slot);
				}
				else if (xmodel_lod_bone)
				{
					// --- HavokPhysicsXModelLOD (48 bytes) ---
					// shapes at +0, bodyNames at +16, shapeTagData (8 bytes each) at +32. One shape on one bone. The tag
					// table is the mesh's own palette in data run order; the loader registers it and the codec decodes
					// tags against it.
					virtual_fixups.emplace_back(static_cast<std::size_t>(0), 0);
					const auto shapes_field = buf.size();
					write_hk_array_header(buf, 1);
					const auto names_field = buf.size();
					write_hk_array_header(buf, 1);
					const auto tag_data_field = buf.size();
					write_hk_array_header(buf, static_cast<int>(palette.size()));

					const auto shape_ptr_slot = buf.size();
					local_fixups.push_back({shapes_field, shape_ptr_slot});
					buf.reserve(8);
					align16();

					const auto name_ptr_slot = buf.size();
					local_fixups.push_back({names_field, name_ptr_slot});
					buf.reserve(8);
					const auto name_offset = buf.size();
					local_fixups.push_back({name_ptr_slot, name_offset});
					buf.write(xmodel_lod_bone->c_str(), xmodel_lod_bone->size() + 1);
					align16();

					local_fixups.push_back({tag_data_field, buf.size()});
					for (const auto& entry : palette)
					{
						// XModelCollisionTagData: surfaceFlags, then collisionFilterInfo.
						// From the unfiltered palette rather than tag_records, because LOD
						// tables keep one contents bit (0x20000000) world tables strip.
						buf.write<std::uint32_t>(static_cast<std::uint32_t>(std::get<2>(entry)));
						buf.write<std::uint32_t>(filter_lod_contents(
							static_cast<std::uint32_t>(std::get<0>(entry))));
					}
					align16();
					shape_ptr_slots.push_back(shape_ptr_slot);
				}
				else
				{
					// --- HavokPhysicsAsset (144) ---
					// Same wrapper build_physics_asset writes; only the shape differs.
					virtual_fixups.emplace_back(static_cast<std::size_t>(0), 0);
					buf.reserve(8); // isRagdoll + pad
					const auto system_data_slot = buf.reserve(8);

					std::array<std::size_t, 8> lookup_field{};
					const int lookup_count[8] = {1, 1, 0, 1, 1, 1, 0, 1};
					for (auto i = 0; i < 8; i++)
					{
						lookup_field[i] = buf.size();
						write_hk_array_header(buf, lookup_count[i]);
					}
					if (buf.size() != 144)
					{
						ZONETOOL_ERROR("havok: HavokPhysicsAsset is %zu bytes, expected 144",
							buf.size());
						return {};
					}

					const std::uint32_t lookup_value[8] = {
						physics_asset->body_quality_crc, physics_asset->material_crc,
						0, 0, 0, 0, 0, 0
					};
					for (auto i = 0; i < 8; i++)
					{
						if (!lookup_count[i]) continue;
						local_fixups.push_back({lookup_field[i], buf.size()});
						buf.write<std::uint32_t>(lookup_value[i]);
						align16();
					}

					// --- hknpPhysicsSystemData (120) ---
					const auto system_data_offset = buf.size();
					global_fixups.push_back({static_cast<int>(system_data_slot), 2,
						static_cast<int>(system_data_offset)});
					virtual_fixups.emplace_back(system_data_offset, 1);

					buf.reserve(16); // hkReferencedObject
					write_hk_array_header(buf, 0); // materials
					write_hk_array_header(buf, 0); // motionProperties
					write_hk_array_header(buf, 0); // motionCinfos
					const auto body_cinfos_field = buf.size();
					write_hk_array_header(buf, 1); // bodyCinfos
					write_hk_array_header(buf, 0); // constraintCinfos
					const auto referenced_field = buf.size();
					write_hk_array_header(buf, 1); // referencedObjects
					const auto system_name_slot = buf.reserve(8);
					if (buf.size() - system_data_offset != 120)
					{
						ZONETOOL_ERROR("havok: hknpPhysicsSystemData is %zu bytes, expected 120",
							buf.size() - system_data_offset);
						return {};
					}
					align16();

					// --- hknpBodyCinfo (160) ---
					const auto body_offset = buf.size();
					local_fixups.push_back({body_cinfos_field, body_offset});
					const auto body_shape_slot = buf.reserve(8);
					buf.write<std::int32_t>(0);            // flags
					buf.write<std::uint32_t>(physics_asset->body_contents); // collisionFilterInfo
					buf.write<std::uint16_t>(0xFFFF);      // materialId
					buf.write<std::uint8_t>(0xFF);         // qualityId
					buf.reserve(5);
					buf.write<std::uint64_t>(0);           // userData
					const auto body_name_slot = buf.reserve(8);
					buf.write<std::uint8_t>(0);            // motionType -- static
					buf.reserve(7);
					for (auto i = 0; i < 2; i++)
					{
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(1.0f);
					}
					buf.reserve(32);
					buf.write<float>(-1.0f);
					buf.reserve(12);
					buf.write<std::uint16_t>(0xFFFF);
					buf.write<std::uint16_t>(0x0000);
					buf.write<std::uint32_t>(0x7FFFFFFFu);
					buf.write<std::uint32_t>(0x7FFFFFFFu);
					buf.write<std::uint32_t>(0);
					buf.reserve(16);
					if (buf.size() - body_offset != 160)
					{
						ZONETOOL_ERROR("havok: hknpBodyCinfo is %zu bytes, expected 160",
							buf.size() - body_offset);
						return {};
					}

					local_fixups.push_back({body_name_slot, buf.size()});
					buf.write(physics_asset->body_name.c_str(),
						physics_asset->body_name.size() + 1);
					align16();

					local_fixups.push_back({referenced_field, buf.size()});
					const auto referenced_slot = buf.reserve(8);
					align16();

					local_fixups.push_back({system_name_slot, buf.size()});
					const char* system_name = "Default Physics System Data";
					buf.write(system_name, std::strlen(system_name) + 1);
					align16();

					// Both the body and the referencedObjects entry point at the mesh.
					shape_ptr_slots.push_back(body_shape_slot);
					shape_ptr_slots.push_back(referenced_slot);
				}

				// --- hknpCompressedMeshShape (160 bytes) ---
				const auto shape_offset = buf.size();
				if (!physics_asset && !xmodel_lod_bone)
				{
					virtual_fixups.emplace_back(shape_list_offset, 0);
				}
				virtual_fixups.emplace_back(shape_offset, class_shape);

				for (const auto slot : shape_ptr_slots)
				{
					global_fixups.push_back({static_cast<int>(slot), 2,
						static_cast<int>(shape_offset)});
				}

				buf.reserve(16); // hkReferencedObject
				buf.write<std::uint16_t>(4); // flags
				buf.write<std::uint8_t>(static_cast<std::uint8_t>(bits_per_key)); // numShapeKeyBits
				buf.write<std::uint8_t>(2); // dispatchType
				buf.write<float>(input.convex_radius);
				buf.write<std::uint64_t>(0); // userData
				buf.reserve(8); // properties*
				// hknpShape's objectSize is 48 but its last reflected member ends at +40;
				// the tail padding is part of the object and has to be written.
				buf.reserve(8);
				buf.write<std::uint32_t>(0xFFFFFFFF); // edgeWeldingMap.secondaryKeyMask
				buf.write<std::uint32_t>(0); // edgeWeldingMap.sencondaryKeyBits
				write_hk_array_header(buf, 0); // edgeWeldingMap.primaryKeyToIndex
				write_hk_array_header(buf, 0); // edgeWeldingMap.valueAndSecondaryKeys
				buf.write<std::uint32_t>(0xFFFFFFFF); // shapeTagCodecInfo
				buf.reserve(4); // pad to +96
				const auto shape_data_slot = buf.reserve(8); // data*

				// Both bitfields are addressed by primitive key, so they have to span the
				// whole key space rather than just the primitive count -- shipped files use
				// numBits == maxKey + 1 for triangleIsInterior and half that for quadIsFlat.
				const auto interior_bits = max_key + 1;
				const auto quad_bits = interior_bits / 2;
				const auto interior_words = (interior_bits + 31) / 32;
				const auto quad_words = (quad_bits + 31) / 32;

				const auto quad_field = buf.size();
				write_hk_array_header(buf, quad_words);
				buf.write<std::int32_t>(quad_bits);
				buf.reserve(4);
				const auto interior_field = buf.size();
				write_hk_array_header(buf, interior_words);
				buf.write<std::int32_t>(interior_bits);
				buf.reserve(4);
				buf.write<std::int32_t>(0); // numTriangles    (SERIALIZE_IGNORED)
				buf.write<std::int32_t>(0); // numConvexShapes (SERIALIZE_IGNORED)

				if (buf.size() - shape_offset != SIZEOF_COMPRESSED_MESH_SHAPE)
				{
					ZONETOOL_ERROR("havok: hknpCompressedMeshShape is %zu bytes, expected %d",
						buf.size() - shape_offset, SIZEOF_COMPRESSED_MESH_SHAPE);
					return {};
				}
				align16();

				// quadIsFlat (indexed by key >> 1) means the fourth corner lies on the plane of the first three. Judged
				// on the decoded positions, because quantisation bends a planar face.
				std::vector<std::uint32_t> quad_bitfield(quad_words, 0);
				for (auto si = 0u; si < sections.size(); si++)
				{
					const auto& section = sections[si];
					for (auto pi = 0u; pi < section.primitives.size(); pi++)
					{
						if (!section.quads[pi])
						{
							continue;
						}

						float corner[4][3];
						for (auto k = 0; k < 4; k++)
						{
							const auto& v = section.verts[section.primitives[pi][k]];
							if (vertex_format == vertex_storage::packed)
							{
								unpack_vertex(pack_vertex(v.data(), section.codec_parms),
									section.codec_parms, corner[k]);
							}
							else
							{
								unpack_shared_vertex(
									pack_shared_vertex(v.data(), world_min, world_max),
									world_min, world_max, corner[k]);
							}
						}

						float e1[3], e2[3], n[3];
						for (auto c = 0; c < 3; c++)
						{
							e1[c] = corner[1][c] - corner[0][c];
							e2[c] = corner[2][c] - corner[0][c];
						}
						n[0] = e1[1] * e2[2] - e1[2] * e2[1];
						n[1] = e1[2] * e2[0] - e1[0] * e2[2];
						n[2] = e1[0] * e2[1] - e1[1] * e2[0];

						const auto length = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
						if (length < 1e-12f)
						{
							continue;
						}

						auto deviation = 0.0f;
						for (auto c = 0; c < 3; c++)
						{
							deviation += (corner[3][c] - corner[0][c]) * n[c] / length;
						}

						// Stock's flat quads deviate by exactly zero; allow only the last bits
						// of float error on top of that.
						if (std::fabs(deviation) > 1e-5f)
						{
							continue;
						}

						const auto bit = static_cast<std::size_t>(
							((si << KEY_SECTION_SHIFT) | (pi << 1)) >> 1);
						if (bit / 32 < quad_bitfield.size())
						{
							quad_bitfield[bit / 32] |= 1u << (bit % 32);
						}
					}
				}

				local_fixups.push_back({quad_field, buf.size()});
				for (const auto word : quad_bitfield)
				{
					buf.write<std::uint32_t>(word);
				}
				align16();
				local_fixups.push_back({interior_field, buf.size()});
				buf.fill(static_cast<std::size_t>(interior_words) * 4, 0);
				align16();

				// --- hknpCompressedMeshShapeData (256 bytes) ---
				const auto data_offset = buf.size();
				virtual_fixups.emplace_back(data_offset, class_shape_data);
				global_fixups.push_back({static_cast<int>(shape_data_slot), 2,
					static_cast<int>(data_offset)});

				buf.reserve(16); // hkReferencedObject
				const auto tree_offset = buf.size();

				const auto tree_nodes_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(top_nodes.size() / SIZEOF_NODE_TOP));
				for (auto i = 0; i < 3; i++) buf.write<float>(world_min[i]);
				buf.write<float>(0.0f);
				for (auto i = 0; i < 3; i++) buf.write<float>(world_max[i]);
				buf.write<float>(0.0f);
				buf.write<std::int32_t>(num_primitive_keys);
				buf.write<std::int32_t>(bits_per_key);
				buf.write<std::uint32_t>(static_cast<std::uint32_t>(max_key));
				buf.reserve(4);
				const auto tree_sections_field = buf.size();
				write_hk_array_header(buf, section_count);
				const auto tree_primitives_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(total_prims));
				const auto tree_svi_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(shared_vertex_index.size() / 2));
				const auto tree_packed_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(packed_vertices.size() / 4));
				const auto tree_shared_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(shared_vertices.size() / 8));
				const auto tree_runs_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(data_runs.size() / SIZEOF_DATA_RUN));

				if (buf.size() - tree_offset != SIZEOF_MESH_TREE)
				{
					ZONETOOL_ERROR("havok: mesh tree is %zu bytes, expected %d",
						buf.size() - tree_offset, SIZEOF_MESH_TREE);
					return {};
				}

				// simdTree (24) then connectivity (48). Connectivity stays empty -- every
				// shipped world blob has it empty too.
				buf.reserve(8); // hkBaseObject
				const auto simd_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(simd_nodes.size()));
				write_hk_array_header(buf, 0); // connectivity.headers
				write_hk_array_header(buf, 0); // connectivity.localLinks
				write_hk_array_header(buf, 0); // connectivity.globalLinks
				// connectivity ends at +248; objectSize is 256.
				buf.reserve(8);

				if (buf.size() - data_offset != SIZEOF_COMPRESSED_MESH_SHAPE_DATA)
				{
					ZONETOOL_ERROR("havok: hknpCompressedMeshShapeData is %zu bytes, expected %d",
						buf.size() - data_offset, SIZEOF_COMPRESSED_MESH_SHAPE_DATA);
					return {};
				}
				align16();

				// --- simd tree payload ---
				local_fixups.push_back({simd_field, buf.size()});
				for (const auto& node : simd_nodes)
				{
					// hkcdFourAabb is SoA: lx, hx, ly, hy, lz, hz.
					for (auto c = 0; c < 3; c++)
					{
						buf.write(node.lo[c], sizeof(node.lo[c]));
						buf.write(node.hi[c], sizeof(node.hi[c]));
					}
					buf.write(node.data, sizeof(node.data));
				}
				align16();

				// --- mesh tree payloads ---
				local_fixups.push_back({tree_nodes_field, buf.size()});
				buf.write(top_nodes.data.data(), top_nodes.size());
				align16();

				local_fixups.push_back({tree_sections_field, buf.size()});
				std::vector<std::size_t> section_node_field(sections.size());
				for (auto i = 0u; i < sections.size(); i++)
				{
					const auto& section = sections[i];
					const auto base = buf.size();

					section_node_field[i] = buf.size();
					write_hk_array_header(buf,
						static_cast<int>(section_nodes[i].size() / SIZEOF_NODE_SECTION));
					for (auto c = 0; c < 3; c++) buf.write<float>(section.mins[c]);
					buf.write<float>(0.0f);
					for (auto c = 0; c < 3; c++) buf.write<float>(section.maxs[c]);
					buf.write<float>(0.0f);
					if (vertex_format == vertex_storage::packed)
					{
						for (auto c = 0; c < 6; c++)
						{
							buf.write<float>(section.codec_parms[c]);
						}
					}
					else
					{
						// A section with no packed vertices writes this sentinel codec, exactly
						// as every shipped numPackedVertices == 0 section does.
						for (auto c = 0; c < 3; c++) buf.write<float>(FLT_MAX);
						for (auto c = 0; c < 3; c++) buf.write<float>(-INFINITY);
					}
					buf.write<std::uint32_t>(
						static_cast<std::uint32_t>(section_first_packed[i]));
					// sharedVertices packs (firstSharedVertexIndex << 8) | numPackedVertices.
					// This descriptor is used even when the section has no shared indices:
					// stock packed-only physics assets store their packed count in its low byte.
					buf.write<std::uint32_t>(static_cast<std::uint32_t>(
						(section_first_vert[i] << 8) |
						(vertex_format == vertex_storage::packed ? section.verts.size() : 0)));
					buf.write<std::uint32_t>(static_cast<std::uint32_t>(
						(section_first_prim[i] << 8) | section.primitives.size()));
					buf.write<std::uint32_t>(static_cast<std::uint32_t>(
						(section_first_run[i] << 8) | section_run_count[i]));
					buf.write<std::uint8_t>(vertex_format == vertex_storage::packed
						? static_cast<std::uint8_t>(section.verts.size()) : 0); // numPackedVertices
					buf.write<std::uint8_t>(vertex_format == vertex_storage::shared
						? static_cast<std::uint8_t>(section.verts.size()) : 0); // numSharedIndices
					buf.write<std::uint16_t>(static_cast<std::uint16_t>(leaf_nodes[i])); // leafIndex
					// Which 65,536-vertex page of sharedVertices this section's indices are
					// relative to. See the paging note where sharedVerticesIndex is built.
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(section_page[i]));
					buf.write<std::uint8_t>(0); // flags
					buf.write<std::uint8_t>(0); // layerData
					buf.write<std::uint8_t>(0); // unusedData

					if (buf.size() - base != SIZEOF_SECTION)
					{
						ZONETOOL_ERROR("havok: section is %zu bytes, expected %d",
							buf.size() - base, SIZEOF_SECTION);
						return {};
					}
				}
				align16();

				for (auto i = 0u; i < sections.size(); i++)
				{
					local_fixups.push_back({section_node_field[i], buf.size()});
					buf.write(section_nodes[i].data.data(), section_nodes[i].size());
					align16();
				}

				local_fixups.push_back({tree_primitives_field, buf.size()});
				buf.write(primitives.data.data(), primitives.size());
				align16();

				if (!shared_vertex_index.data.empty())
				{
					local_fixups.push_back({tree_svi_field, buf.size()});
					buf.write(shared_vertex_index.data.data(), shared_vertex_index.size());
					align16();
				}

				if (!packed_vertices.data.empty())
				{
					local_fixups.push_back({tree_packed_field, buf.size()});
					buf.write(packed_vertices.data.data(), packed_vertices.size());
					align16();
				}

				if (!shared_vertices.data.empty())
				{
					local_fixups.push_back({tree_shared_field, buf.size()});
					buf.write(shared_vertices.data.data(), shared_vertices.size());
					align16();
				}

				local_fixups.push_back({tree_runs_field, buf.size()});
				buf.write(data_runs.data.data(), data_runs.size());
				align16();

				const auto data_size = buf.size();

				byte_buffer names;
				const auto write_name = [&](const std::uint32_t sig, const char* name)
				{
					names.write<std::uint32_t>(sig);
					names.write<std::uint8_t>(0x09);
					const auto offset = names.size();
					names.write(name, std::strlen(name) + 1);
					return offset;
				};

				write_name(SIG_HK_CLASS, "hkClass");
				write_name(SIG_HK_CLASS_MEMBER, "hkClassMember");
				write_name(SIG_HK_CLASS_ENUM, "hkClassEnum");
				write_name(SIG_HK_CLASS_ENUM_ITEM, "hkClassEnumItem");

				std::array<std::size_t, 4> name_offsets{};
				if (xmodel_lod_bone)
				{
					name_offsets[0] = write_name(SIG_XMODEL_LOD, "HavokPhysicsXModelLOD");
					name_offsets[1] = write_name(SIG_COMPRESSED_MESH_SHAPE,
						"hknpCompressedMeshShape");
					name_offsets[2] = write_name(SIG_COMPRESSED_MESH_SHAPE_DATA,
						"hknpCompressedMeshShapeData");
				}
				else if (!physics_asset)
				{
					name_offsets[0] = write_name(SIG_SHAPE_LIST, "HavokPhysicsShapeList");
					name_offsets[1] = write_name(SIG_COMPRESSED_MESH_SHAPE,
						"hknpCompressedMeshShape");
					name_offsets[2] = write_name(SIG_COMPRESSED_MESH_SHAPE_DATA,
						"hknpCompressedMeshShapeData");
				}
				else
				{
					name_offsets[0] = write_name(SIG_PHYSICS_ASSET, "HavokPhysicsAsset");
					name_offsets[1] = write_name(SIG_PHYSICS_SYSTEM_DATA,
						"hknpPhysicsSystemData");
					name_offsets[2] = write_name(SIG_COMPRESSED_MESH_SHAPE,
						"hknpCompressedMeshShape");
					name_offsets[3] = write_name(SIG_COMPRESSED_MESH_SHAPE_DATA,
						"hknpCompressedMeshShapeData");
				}
				names.align(16, 0xFF);

				// Local fixups are ordered by destination, as in shipped files.
				std::sort(local_fixups.begin(), local_fixups.end(),
					[](const pending_fixup& a, const pending_fixup& b)
					{
						return a.dst < b.dst;
					});

				// Each fixup table is padded to 16 bytes with 0xFF; the padding counts towards the table's size.
				byte_buffer fixups;
				for (const auto& fixup : local_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.src));
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.dst));
				}
				fixups.align(16, 0xFF);
				const auto local_size = fixups.size();

				std::sort(global_fixups.begin(), global_fixups.end(),
					[](const std::array<int, 3>& a, const std::array<int, 3>& b)
					{
						return a[0] < b[0];
					});
				for (const auto& fixup : global_fixups)
				{
					fixups.write<std::int32_t>(fixup[0]);
					fixups.write<std::int32_t>(fixup[1]);
					fixups.write<std::int32_t>(fixup[2]);
				}
				fixups.align(16, 0xFF);
				const auto global_size = fixups.size() - local_size;

				for (const auto& fixup : virtual_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(0);
					fixups.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[fixup.second]));
				}
				fixups.align(16, 0xFF);
				const auto virtual_size = fixups.size() - local_size - global_size;

				byte_buffer file;
				file.write<std::uint32_t>(HK_MAGIC0);
				file.write<std::uint32_t>(HK_MAGIC1);
				file.write<std::int32_t>(0); // userTag
				file.write<std::int32_t>(HK_FILE_VERSION);
				file.write<std::uint8_t>(8); // pointerSize
				file.write<std::uint8_t>(1); // littleEndian
				file.write<std::uint8_t>(0); // reuseBaseClassPadding
				file.write<std::uint8_t>(1); // emptyBaseClassOptimization
				file.write<std::int32_t>(3); // numSections
				file.write<std::int32_t>(2); // contentsSectionIndex
				file.write<std::int32_t>(0); // contentsSectionOffset
				file.write<std::int32_t>(0); // contentsClassNameSectionIndex
				file.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[0]));
				const char* version = "hk_2014.2.5-r1";
				const auto version_length = std::strlen(version) + 1;
				file.write(version, version_length);
				// hkPackfileHeader() memsets itself to -1 before assigning fields, so the
				// tail of contentsVersion stays 0xFF rather than 0.
				file.fill(16 - version_length, 0xFF);
				file.write<std::int32_t>(0); // flags
				file.write<std::uint16_t>(HK_MAX_PREDICATE);
				file.write<std::uint16_t>(0);

				const auto names_start = HK_HEADER_SIZE + 3 * HK_SECTION_HEADER_SIZE;
				const auto data_start = names_start + names.size();

				const auto write_section_header = [&](const char* tag, const std::size_t abs,
					const std::size_t payload, const std::size_t local, const std::size_t global,
					const std::size_t virt)
				{
					char name[19] = {};
					std::strncpy(name, tag, sizeof(name));
					file.write(name, sizeof(name));
					file.write<std::uint8_t>(0xFF); // m_nullByte
					file.write<std::int32_t>(static_cast<std::int32_t>(abs));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local + global));
					const auto end = payload + local + global + virt;
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.fill(16, 0xFF); // m_pad[4]
				};

				write_section_header("__classnames__", names_start, names.size(), 0, 0, 0);
				write_section_header("__types__", data_start, 0, 0, 0, 0);
				write_section_header("__data__", data_start, data_size, local_size, global_size,
					virtual_size);

				file.write(names.data.data(), names.size());
				file.write(buf.data.data(), buf.size());
				file.write(fixups.data.data(), fixups.size());

				// DIAGNOSTIC: every shapeTagData entry as it was written -- the filtered
				// collisionFilterInfo, the material CRC and the full userData word.
				for (auto i = 0u; i < palette.size(); i++)
				{
					ZONETOOL_INFO("  tag %2u  filter 0x%08X (raw 0x%08X)  crc 0x%08X  "
						"userData 0x%016llX", i,
						tag_records[i].collision_filter,
						static_cast<std::uint32_t>(std::get<0>(palette[i])),
						tag_records[i].material_crc,
						static_cast<unsigned long long>(tag_records[i].user_data));
				}

				ZONETOOL_INFO("havok: %s built -- %zu input triangles (%d mesh triangles), "
					"%zu convex custom primitives, %d sections, %d surface tags, "
					"contents 0x%08X, %zu bytes",
					physics_asset ? "model physics asset" : "world shape",
					input.triangles.size(), mesh_triangles, total_customs, section_count,
					tag_count, world_contents, file.size());

				if (out_tags)
				{
					*out_tags = tag_records;
				}

				return file.data;
			}

			// The ents blob holds one hknpDynamicCompoundShape of hknpConvexPolytopeShapes per brush model, indexed
			// by physicsShapeOverrideIdx. Per shape: compound (208) | instances (128*L) | per convex { object (96) |
			// verts | planes | faces | indices | connectivity (48) | vertexEdges | faceLinks } | compoundData (56) |
			// tree nodes (32 * (2L+1)), every array payload 16-byte aligned.
			std::vector<std::uint8_t> build_world_shape(const mesh_input& input,
				std::vector<shape_tag>* out_tags)
			{
				return build_mesh_blob(input, nullptr, nullptr, out_tags);
			}

			std::vector<std::uint8_t> build_model_physics_asset(const mesh_input& input,
				const physics_asset_input& physics_asset)
			{
				return build_mesh_blob(input, &physics_asset);
			}

			std::vector<std::uint8_t> build_model_physics_lod(const mesh_input& input,
				const std::string& bone_name)
			{
				return build_mesh_blob(input, nullptr, &bone_name);
			}

			std::vector<std::uint8_t> build_ents_shape_list(const ents_input& input,
				ents_tag_merge* out_merge)
			{
				// A half-edge, addressed the way Havok does it: which face it belongs to and
				// its position in that face's index run.
				struct half_edge
				{
					std::uint16_t face = 0;
					std::uint8_t edge = 0;
				};

				struct tree_node
				{
					float mn[3] = {0.0f, 0.0f, 0.0f};
					float mx[3] = {0.0f, 0.0f, 0.0f};
					std::uint16_t parent = 0;
					std::uint32_t data = 0;
				};

				byte_buffer buf;
				// A pointer to a registered object is a global fixup, a pointer to raw payload a local one. The loader
				// tracks objects through the distinction.
				std::vector<std::pair<std::size_t, std::size_t>> local_fixups;
				std::vector<std::pair<std::size_t, std::size_t>> global_fixups;
				// (object offset, class-name index) -- the virtual fixup table is the only
				// thing that tells the loader which class each object is.
				std::vector<std::pair<std::size_t, int>> virtual_fixups;

				const auto align16 = [&] { buf.align(16, 0); };

				// The caller stores a slot index per brush model before the blob is built, so
				// shapes are written one-for-one with the input. Dropping one here would
				// silently shift every index after it; refuse instead.
				std::vector<const ents_shape*> shapes;
				for (const auto& shape : input.shapes)
				{
					if (shape.convexes.empty())
					{
						ZONETOOL_ERROR("havok: ents shape %zu (\"%s\") has no convexes -- "
							"writing it would shift every later shape index",
							shapes.size(), shape.name.c_str());
						return {};
					}
					shapes.emplace_back(&shape);
				}

				const auto shape_count = static_cast<int>(shapes.size());

				// One ShapeTagData per distinct (contents, materialCRC, userData). The table starts as the world blob's,
				// verbatim: IW7 decodes every shape's tags against the one table registered by
				// HavokPhysics_SetMainShapeList, so world tag indices must mean the same thing here.
				std::vector<shape_tag> palette = input.world_tags;
				const auto prefix_size = palette.size();
				// Which prefix records the shapes actually land on. A flag per record rather
				// than a counter because tag_for runs twice over the same shapes -- once to
				// size the table, once while writing the instances.
				std::vector<bool> prefix_used(prefix_size, false);
				const auto tag_for = [&](const ents_shape& shape)
				{
					// Compare on the RECORD, not on the source fields: contents is masked on
					// the way in on both paths, so two shapes whose raw contents differ only
					// in a compile-only bit are the same 24 bytes and must share one entry.
					const shape_tag key{
						filter_contents(static_cast<std::uint32_t>(shape.contents)),
						shape.material_crc, shape.user_data};
					for (auto i = 0u; i < palette.size(); i++)
					{
						if (palette[i] == key)
						{
							if (i < prefix_size)
							{
								prefix_used[i] = true;
							}
							return static_cast<std::uint16_t>(i);
						}
					}
					// Anything the world table does not already carry lands after it, in
					// first-use order, so the prefix stays byte-identical and in place.
					palette.emplace_back(key);
					return static_cast<std::uint16_t>(palette.size() - 1);
				};

				for (const auto* shape : shapes)
				{
					tag_for(*shape);
				}

				// Keep at least one tag: the material table the runtime registers should not
				// be empty even when there is no geometry and no world table to inherit.
				if (palette.empty())
				{
					palette.emplace_back(shape_tag{filter_contents(1), DEFAULT_MATERIAL_CRC, 0});
				}

				// --- HavokPhysicsShapeList (152 bytes) ---
				// shapes, shapeIndices, shapeNames, vertCounts, triCounts, minMaxes at
				// +0..+80; numWorldGeoShapes at +96; shapeTagData, shapeContents and
				// convexCounts at +104..+136.
				const std::array<int, 10> member_counts = {
					shape_count, shape_count, shape_count, shape_count, shape_count,
					shape_count * 2, 0, static_cast<int>(palette.size()), shape_count,
					shape_count
				};

				std::array<std::size_t, 10> member_field{};
				for (auto i = 0; i < 6; i++)
				{
					member_field[i] = buf.size();
					write_hk_array_header(buf, member_counts[i]);
				}
				buf.write<std::int32_t>(0); // numWorldGeoShapes -- 0 in every shipped blob
				buf.reserve(4); // pad to +104
				for (auto i = 7; i < 10; i++)
				{
					member_field[i] = buf.size();
					write_hk_array_header(buf, member_counts[i]);
				}

				if (buf.size() != 152)
				{
					ZONETOOL_ERROR("havok: ents HavokPhysicsShapeList is %zu bytes, expected 152",
						buf.size());
					return {};
				}

				virtual_fixups.emplace_back(static_cast<std::size_t>(0), 0); // root at +0
				align16();

				const auto record_array = [&](const int index, const std::size_t payload)
				{
					// An empty hkArray keeps m_data null and takes no fixup, which is what
					// the shipped stub blobs do.
					if (member_counts[index] > 0)
					{
						local_fixups.emplace_back(member_field[index], payload);
					}
				};

				// shapes: one pointer per shape, patched once the objects are laid out.
				std::vector<std::size_t> shape_ptr_slots;
				record_array(0, buf.size());
				for (auto i = 0; i < shape_count; i++)
				{
					shape_ptr_slots.emplace_back(buf.reserve(8));
				}
				align16();

				record_array(1, buf.size());
				for (auto i = 0; i < shape_count; i++)
				{
					buf.write<std::int32_t>(i); // shapeIndices is the identity in stock data
				}
				align16();

				// shapeNames is an hkArray<hkStringPtr>: a pointer array, then the bytes.
				record_array(2, buf.size());
				std::vector<std::size_t> name_ptr_slots;
				for (auto i = 0; i < shape_count; i++)
				{
					name_ptr_slots.emplace_back(buf.reserve(8));
				}
				align16();
				for (auto i = 0; i < shape_count; i++)
				{
					local_fixups.emplace_back(name_ptr_slots[i], buf.size());
					const auto& name = shapes[i]->name;
					buf.write(name.c_str(), name.size() + 1);

					// hkStringPtr keeps an owned flag in bit 0 of the pointer, and a set bit makes zone unload free an
					// address inside the fastfile. Names are padded so each starts on an even offset.
					if (buf.size() & 1)
					{
						buf.write<std::uint8_t>(0);
					}
				}
				align16();

				record_array(3, buf.size());
				for (const auto* shape : shapes)
				{
					auto verts = 0;
					for (const auto& convex : shape->convexes)
					{
						verts += static_cast<int>(padded_vertex_count(convex.verts.size()));
					}
					buf.write<std::int32_t>(verts);
				}
				align16();

				record_array(4, buf.size());
				for (auto i = 0; i < shape_count; i++)
				{
					buf.write<std::int32_t>(0); // triCounts -- 0 for convex shapes
				}
				align16();

				// minMaxes is two hkVector4f per shape and reads all-zero in every shipped
				// ents blob, so leave it zeroed rather than inventing values.
				record_array(5, buf.size());
				buf.reserve(static_cast<std::size_t>(shape_count) * 32);
				align16();

				record_array(7, buf.size());
				for (const auto& entry : palette)
				{
					buf.write<std::uint32_t>(entry.collision_filter);
					buf.write<std::uint32_t>(entry.material_crc); // materialCRC
					buf.write<std::uint16_t>(0xFFFF); // materialId -- resolved at load
					buf.reserve(6);
					buf.write<std::uint64_t>(entry.user_data); // userData
				}
				align16();

				// shapeContents is what CM_ContentsOfBrushModel hands back as the entity's
				// contents mask, so it has to be the real CoD mask, not a wildcard.
				record_array(8, buf.size());
				for (const auto* shape : shapes)
				{
					buf.write<std::uint32_t>(
						filter_contents(shape->entity_contents));
				}
				align16();

				record_array(9, buf.size());
				for (const auto* shape : shapes)
				{
					buf.write<std::int32_t>(static_cast<std::int32_t>(shape->convexes.size()));
				}
				align16();

				// --- one compound per shape ---
				for (auto s = 0; s < shape_count; s++)
				{
					const auto& shape = *shapes[s];
					const auto instance_count = static_cast<int>(shape.convexes.size());

					const auto compound_offset = buf.size();
					global_fixups.emplace_back(shape_ptr_slots[s], compound_offset);
					virtual_fixups.emplace_back(compound_offset, 1);

					// hknpShape header.
					buf.reserve(16); // hkBaseObject vtable + hkReferencedObject
					buf.write<std::uint16_t>(static_cast<std::uint16_t>(COMPOUND_SHAPE_FLAGS));
					// numShapeKeyBits is the bit length of the instance count.
					auto key_bits = 0;
					for (auto n = instance_count; n > 0; n >>= 1)
					{
						key_bits++;
					}
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(key_bits));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(COMPOUND_DISPATCH_TYPE));
					buf.write<float>(0.0f); // convexRadius
					buf.write<std::uint64_t>(0); // userData
					buf.reserve(8); // properties
					buf.reserve(8); // pad -- hknpShape is 48 bytes, its fields end at 40
					// hknpCompositeShape: edgeWeldingMap (40 bytes, empty) then
					// shapeTagCodecInfo. An empty map still carries secondaryKeyMask = ~0.
					buf.write<std::uint32_t>(0xFFFFFFFFu); // secondaryKeyMask
					buf.write<std::uint32_t>(0); // secondaryKeyBits
					write_hk_array_header(buf, 0); // primaryKeyToIndex
					write_hk_array_header(buf, 0); // valueAndSecondaryKeys
					buf.write<std::uint32_t>(0xFFFFFFFFu); // shapeTagCodecInfo
					buf.reserve(4); // pad to +96

					if (buf.size() - compound_offset != 96)
					{
						ZONETOOL_ERROR("havok: compound header is %zu bytes, expected 96",
							buf.size() - compound_offset);
						return {};
					}

					// hkFreeListArray<hknpShapeInstance> at +96: the array, then firstFree,
					// padded out to its 24-byte object size.
					const auto instances_field = buf.size();
					write_hk_array_header(buf, instance_count);
					buf.write<std::int32_t>(-1); // firstFree
					buf.reserve(4);
					buf.reserve(8); // pad -- aabb is 16-aligned at +128

					// aabb at +128, filled in once the children are known.
					const auto aabb_slot = buf.reserve(32);
					buf.write<std::uint8_t>(1); // isMutable at +160
					buf.reserve(7);
					buf.reserve(16); // mutationSignals at +168 -- SERIALIZE_IGNORED
					buf.reserve(8); // pad to +192
					const auto bvd_slot = buf.reserve(8); // boundingVolumeData at +192
					buf.reserve(8); // pad to the 208-byte object size

					if (buf.size() - compound_offset != SIZEOF_DYNAMIC_COMPOUND_SHAPE)
					{
						ZONETOOL_ERROR("havok: hknpDynamicCompoundShape is %zu bytes, expected %d",
							buf.size() - compound_offset, SIZEOF_DYNAMIC_COMPOUND_SHAPE);
						return {};
					}

					// hknpShapeInstance[], immediately after the compound.
					local_fixups.emplace_back(instances_field, buf.size());
					std::vector<std::size_t> instance_shape_slots;
					std::vector<std::size_t> instance_offsets;
					for (auto i = 0; i < instance_count; i++)
					{
						const auto instance_offset = buf.size();
						instance_offsets.emplace_back(instance_offset);
						// Identity transform: the convexes are already in the compound's space. The w lanes are int24 values:
						// column 0 carries the instance flags and the translation the index of this instance's leaf in the
						// dynamic tree, patched in once the tree is built. Shape queries skip an instance without them.
						buf.write<float>(1.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<std::uint32_t>(SHAPE_INSTANCE_FLAGS_W);
						buf.write<float>(0.0f); buf.write<float>(1.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(1.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<std::uint32_t>(INT24_W_BASE);
						for (auto c = 0; c < 4; c++)
						{
							buf.write<float>(1.0f); // scale
						}
						instance_shape_slots.emplace_back(buf.reserve(8));
						buf.write<std::uint16_t>(tag_for(shape));
						buf.write<std::uint16_t>(0xFFFF); // destructionTag
						// padding[30] ends at +122; the object is 128, so it is padded out
						// again. Getting this wrong shifts every instance after the first.
						buf.reserve(36);

						if (buf.size() - instance_offset != SIZEOF_SHAPE_INSTANCE)
						{
							ZONETOOL_ERROR("havok: hknpShapeInstance is %zu bytes, expected %d",
								buf.size() - instance_offset, SIZEOF_SHAPE_INSTANCE);
							return {};
						}
					}
					align16();

					float shape_min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
					float shape_max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
					std::vector<std::array<float, 6>> convex_bounds;

					for (auto c = 0; c < instance_count; c++)
					{
						const auto& convex = shape.convexes[c];

						auto index_total = 0u;
						for (const auto& face : convex.faces)
						{
							index_total += static_cast<unsigned int>(face.indices.size());
						}

						const auto vertex_count = padded_vertex_count(convex.verts.size());

						if (vertex_count > 255 || convex.faces.size() > 0xFFFF
							|| index_total > 0xFFFF)
						{
							ZONETOOL_ERROR("havok: convex %d of shape %d exceeds the format "
								"limits (%zu verts, %zu faces, %u indices)", c, s,
								convex.verts.size(), convex.faces.size(), index_total);
							return {};
						}

						const auto convex_offset = buf.size();
						global_fixups.emplace_back(instance_shape_slots[c], convex_offset);
						virtual_fixups.emplace_back(convex_offset, 2);

						buf.reserve(16); // hkReferencedObject
						buf.write<std::uint16_t>(static_cast<std::uint16_t>(CONVEX_SHAPE_FLAGS));
						buf.write<std::uint8_t>(0); // numShapeKeyBits
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(CONVEX_DISPATCH_TYPE));
						buf.write<float>(0.0f); // convexRadius
						buf.write<std::uint64_t>(0); // userData
						buf.reserve(8); // properties
						buf.reserve(8); // pad -- hknpShape is 48 bytes, its fields end at 40
						// hkRelArray<T> is { uint16 size; uint16 offset }, the offset counted
						// from the address of the field itself. All four payloads follow the
						// 96-byte object in declaration order.
						const auto vertices_field = buf.size(); // +48
						buf.reserve(4);
						buf.reserve(12); // pad to +64
						const auto planes_field = buf.size();
						buf.reserve(4);
						const auto faces_field = buf.size();
						buf.reserve(4);
						const auto indices_field = buf.size();
						buf.reserve(4);
						buf.reserve(4); // pad to +80
						const auto connectivity_slot = buf.reserve(8);
						buf.reserve(8); // pad to +96

						if (buf.size() - convex_offset != SIZEOF_CONVEX_POLYTOPE_SHAPE)
						{
							ZONETOOL_ERROR("havok: hknpConvexPolytopeShape is %zu bytes, "
								"expected %d", buf.size() - convex_offset,
								SIZEOF_CONVEX_POLYTOPE_SHAPE);
							return {};
						}

						// vertexEdges[v] is any half-edge starting at vertex v; faceLinks[g] is the twin of half-edge g.
						std::vector<half_edge> vertex_edges(vertex_count);
						std::vector<bool> vertex_seen(vertex_count, false);
						std::vector<half_edge> face_links(index_total);
						std::map<std::pair<std::uint8_t, std::uint8_t>, half_edge> directed;

						for (auto f = 0u; f < convex.faces.size(); f++)
						{
							const auto& face = convex.faces[f];
							for (auto e = 0u; e < face.indices.size(); e++)
							{
								const auto from = face.indices[e];
								const auto to = face.indices[(e + 1) % face.indices.size()];
								const half_edge self{
									static_cast<std::uint16_t>(f),
									static_cast<std::uint8_t>(e)
								};

								if (from < vertex_seen.size() && !vertex_seen[from])
								{
									vertex_seen[from] = true;
									vertex_edges[from] = self;
								}

								directed[{from, to}] = self;
							}
						}

						auto global = 0u;
						auto unmatched = 0;
						for (auto f = 0u; f < convex.faces.size(); f++)
						{
							const auto& face = convex.faces[f];
							for (auto e = 0u; e < face.indices.size(); e++)
							{
								const auto from = face.indices[e];
								const auto to = face.indices[(e + 1) % face.indices.size()];
								const auto twin = directed.find({to, from});
								if (twin != directed.end())
								{
									face_links[global] = twin->second;
								}
								else
								{
									// A hull whose faces do not pair up is not closed. Point
									// the edge at itself rather than at an arbitrary face.
									face_links[global] = {
										static_cast<std::uint16_t>(f),
										static_cast<std::uint8_t>(e)
									};
									unmatched++;
								}
								global++;
							}
						}

						if (unmatched)
						{
							ZONETOOL_WARNING("havok: convex %d of shape %d has %d unpaired "
								"edge(s) -- hull is not closed", c, s, unmatched);
						}

						const auto write_rel_array = [&](const std::size_t field,
							const std::size_t count)
						{
							buf.patch<std::uint16_t>(field, static_cast<std::uint16_t>(count));
							buf.patch<std::uint16_t>(field + 2,
								static_cast<std::uint16_t>(buf.size() - field));
						};

						float mn[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
						float mx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

						write_rel_array(vertices_field, vertex_count);
						for (auto j = 0u; j < vertex_count; j++)
						{
							// past the real vertices, repeat the last one whole -- see
							// padded_vertex_count above
							const auto source = std::min<std::size_t>(
								j, convex.verts.size() - 1);
							const auto& v = convex.verts[source];
							for (auto k = 0; k < 3; k++)
							{
								const auto value = v[k] * input.scale;
								buf.write<float>(value);
								mn[k] = std::min(mn[k], value);
								mx[k] = std::max(mx[k], value);
							}
							// The w lane carries the vertex's own index in the low mantissa bits of 0.5f.
							buf.write<std::uint32_t>(
								0x3F000000u | static_cast<std::uint32_t>(source));
						}
						align16();

						// Havok stores dot(n, p) + d = 0 with an outward normal, so the CoD
						// plane distance is negated. It scales with the geometry.
						write_rel_array(planes_field, convex.faces.size());
						for (const auto& face : convex.faces)
						{
							for (auto k = 0; k < 3; k++)
							{
								buf.write<float>(face.plane[k]);
							}
							buf.write<float>(-face.plane[3] * input.scale);
						}
						align16();

						// minHalfAngle is the smallest support half-angle to an adjacent face, quantised as
						// floor(halfAngle / pi * 512) (a right angle gives 127). Faces with no paired neighbour keep the blunt
						// end of the range.
						write_rel_array(faces_field, convex.faces.size());
						auto first_index = 0u;
						for (auto f = 0u; f < convex.faces.size(); f++)
						{
							const auto& face = convex.faces[f];
							buf.write<std::uint16_t>(static_cast<std::uint16_t>(first_index));
							buf.write<std::uint8_t>(
								static_cast<std::uint8_t>(face.indices.size()));

							auto smallest = 45.0f;
							for (auto e = 0u; e < face.indices.size(); e++)
							{
								const auto& twin = face_links[first_index + e];
								if (twin.face == f || twin.face >= convex.faces.size())
								{
									continue; // unpaired edge -- nothing to measure against
								}

								const auto& other = convex.faces[twin.face].plane;
								auto dot = face.plane[0] * other[0] + face.plane[1] * other[1]
									+ face.plane[2] * other[2];
								dot = std::max(-1.0f, std::min(1.0f, dot));

								// The angle between two outward normals is the supplement of
								// the dihedral angle between the faces.
								const auto dihedral = 180.0f
									- static_cast<float>(std::acos(dot) * 57.29577951308232);
								smallest = std::min(smallest, dihedral * 0.5f);
							}

							const auto half_radians = smallest * 0.017453292519943295f;
							auto quantised = static_cast<int>(
								(half_radians - 1.1920929e-7f) * 41720.875f + 0.5f);
							quantised = std::max(0, std::min(65535, quantised)) >> 8;
							buf.write<std::uint8_t>(static_cast<std::uint8_t>(quantised));

							first_index += static_cast<unsigned int>(face.indices.size());
						}
						align16();

						write_rel_array(indices_field, index_total);
						for (const auto& face : convex.faces)
						{
							for (const auto index : face.indices)
							{
								buf.write<std::uint8_t>(index);
							}
						}
						align16();

						// the padded vertexEdges entries repeat the last real one, as stock does
						for (auto j = convex.verts.size(); j < vertex_count; j++)
						{
							vertex_edges[j] = vertex_edges[convex.verts.size() - 1];
						}

						const auto connectivity_offset = buf.size();
						global_fixups.emplace_back(connectivity_slot, connectivity_offset);
						virtual_fixups.emplace_back(connectivity_offset, 3);

						buf.reserve(16); // hkReferencedObject
						const auto vertex_edges_field = buf.size();
						write_hk_array_header(buf, static_cast<int>(vertex_edges.size()));
						const auto face_links_field = buf.size();
						write_hk_array_header(buf, static_cast<int>(face_links.size()));

						if (buf.size() - connectivity_offset
							!= SIZEOF_CONVEX_POLYTOPE_CONNECTIVITY)
						{
							ZONETOOL_ERROR("havok: connectivity is %zu bytes, expected %d",
								buf.size() - connectivity_offset,
								SIZEOF_CONVEX_POLYTOPE_CONNECTIVITY);
							return {};
						}

						const auto write_edges = [&](const std::size_t field,
							const std::vector<half_edge>& edges)
						{
							if (edges.empty())
							{
								return;
							}
							local_fixups.emplace_back(field, buf.size());
							for (const auto& edge : edges)
							{
								buf.write<std::uint16_t>(edge.face);
								buf.write<std::uint8_t>(edge.edge);
								buf.write<std::uint8_t>(0);
							}
							align16();
						};

						write_edges(vertex_edges_field, vertex_edges);
						write_edges(face_links_field, face_links);

						convex_bounds.push_back({mn[0], mn[1], mn[2], mx[0], mx[1], mx[2]});
						for (auto k = 0; k < 3; k++)
						{
							shape_min[k] = std::min(shape_min[k], mn[k]);
							shape_max[k] = std::max(shape_max[k], mx[k]);
						}
					}

					// The compound's own aabb, now that the children are known.
					for (auto k = 0; k < 3; k++)
					{
						buf.patch<float>(aabb_slot + k * 4, shape_min[k]);
						buf.patch<float>(aabb_slot + 16 + k * 4, shape_max[k]);
					}
					// min.w is 0x3F000000 (int24 0) on 189 of 232 shipped compounds and a
					// small count on the rest; max.w is always 0.
					buf.patch<std::uint32_t>(aabb_slot + 12, INT24_W_BASE);
					buf.patch<float>(aabb_slot + 28, 0.0f);

					// --- hknpDynamicCompoundShapeData + its dynamic AABB tree ---
					const auto bvd_offset = buf.size();
					global_fixups.emplace_back(bvd_slot, bvd_offset);
					virtual_fixups.emplace_back(bvd_offset, 4);

					buf.reserve(16); // hkReferencedObject
					const auto nodes_field = buf.size();
					const auto node_count = 2 * instance_count + 1;
					write_hk_array_header(buf, node_count);
					buf.write<std::int32_t>(2 * instance_count); // firstFree
					buf.reserve(4);
					buf.write<std::int32_t>(instance_count); // numLeaves
					buf.reserve(4);
					buf.write<std::int32_t>(1); // root
					buf.reserve(4);

					if (buf.size() - bvd_offset != SIZEOF_DYNAMIC_COMPOUND_SHAPE_DATA)
					{
						ZONETOOL_ERROR("havok: hknpDynamicCompoundShapeData is %zu bytes, "
							"expected %d", buf.size() - bvd_offset,
							SIZEOF_DYNAMIC_COMPOUND_SHAPE_DATA);
						return {};
					}

					align16();

					// Node 0 is an all-zero sentinel, node 1 the root and node 2L a free slot. Split on the widest axis at
					// the median.
					std::vector<tree_node> nodes(node_count);
					auto next_node = 1;

					std::vector<int> order(instance_count);
					for (auto i = 0; i < instance_count; i++)
					{
						order[i] = i;
					}

					std::function<int(int, int, std::uint16_t)> build_tree =
						[&](const int first, const int count, const std::uint16_t parent) -> int
					{
						const auto self = next_node++;
						auto& node = nodes[self];
						node.parent = parent;

						for (auto k = 0; k < 3; k++)
						{
							node.mn[k] = FLT_MAX;
							node.mx[k] = -FLT_MAX;
						}
						for (auto i = 0; i < count; i++)
						{
							const auto& b = convex_bounds[order[first + i]];
							for (auto k = 0; k < 3; k++)
							{
								node.mn[k] = std::min(node.mn[k], b[k]);
								node.mx[k] = std::max(node.mx[k], b[k + 3]);
							}
						}

						if (count == 1)
						{
							// Leaf: the low half stays zero, which is what distinguishes it
							// from an internal node -- a child index is always >= 2.
							node.data = static_cast<std::uint32_t>(order[first]) << 16;
							return self;
						}

						auto axis = 0;
						auto widest = node.mx[0] - node.mn[0];
						for (auto k = 1; k < 3; k++)
						{
							if (node.mx[k] - node.mn[k] > widest)
							{
								widest = node.mx[k] - node.mn[k];
								axis = k;
							}
						}

						const auto centre = [&](const int index)
						{
							const auto& b = convex_bounds[index];
							return (b[axis] + b[axis + 3]) * 0.5f;
						};
						std::sort(order.begin() + first, order.begin() + first + count,
							[&](const int a, const int b) { return centre(a) < centre(b); });

						const auto half = count / 2;
						const auto left = build_tree(first, half,
							static_cast<std::uint16_t>(self));
						const auto right = build_tree(first + half, count - half,
							static_cast<std::uint16_t>(self));
						nodes[self].data = static_cast<std::uint32_t>(left)
							| (static_cast<std::uint32_t>(right) << 16);
						return self;
					};

					if (instance_count > 0)
					{
						build_tree(0, instance_count, 0);
					}

					// Leaves are the nodes whose data has a zero low half; the high half is the instance they bound.
					for (auto n = 1; n < node_count - 1; n++)
					{
						const auto& node = nodes[n];
						if ((node.data & 0xFFFF) != 0)
						{
							continue;
						}
						const auto instance = static_cast<int>(node.data >> 16);
						if (instance < instance_count)
						{
							buf.patch<std::uint32_t>(instance_offsets[instance] + 48 + 12,
								INT24_W_BASE | static_cast<std::uint32_t>(n));
						}
					}

					local_fixups.emplace_back(nodes_field, buf.size());
					for (auto n = 0; n < node_count; n++)
					{
						const auto& node = nodes[n];
						const auto used = (n != 0 && n != node_count - 1);
						for (auto k = 0; k < 3; k++)
						{
							buf.write<float>(used ? node.mn[k] : 0.0f);
						}
						buf.write<std::uint16_t>(used ? node.parent : 0);
						// The upper half of this w lane is the top of 0.5f in every shipped
						// node; the links live in the low half and in the max w lane.
						buf.write<std::uint16_t>(used ? 0x3F00 : 0);
						for (auto k = 0; k < 3; k++)
						{
							buf.write<float>(used ? node.mx[k] : 0.0f);
						}
						buf.write<std::uint32_t>(used ? node.data : 0);
					}
					align16();
				}

				const auto data_size = buf.size();

				byte_buffer names;
				const auto write_name = [&](const std::uint32_t sig, const char* name)
				{
					names.write<std::uint32_t>(sig);
					names.write<std::uint8_t>(0x09);
					const auto offset = names.size();
					names.write(name, std::strlen(name) + 1);
					return offset;
				};

				write_name(SIG_HK_CLASS, "hkClass");
				write_name(SIG_HK_CLASS_MEMBER, "hkClassMember");
				write_name(SIG_HK_CLASS_ENUM, "hkClassEnum");
				write_name(SIG_HK_CLASS_ENUM_ITEM, "hkClassEnumItem");

				std::array<std::size_t, 5> name_offsets{};
				name_offsets[0] = write_name(SIG_SHAPE_LIST, "HavokPhysicsShapeList");
				if (shape_count > 0)
				{
					name_offsets[1] = write_name(SIG_DYNAMIC_COMPOUND_SHAPE,
						"hknpDynamicCompoundShape");
					name_offsets[2] = write_name(SIG_CONVEX_POLYTOPE_SHAPE,
						"hknpConvexPolytopeShape");
					name_offsets[3] = write_name(SIG_CONVEX_POLYTOPE_CONNECTIVITY,
						"hknpConvexPolytopeShapeConnectivity");
					name_offsets[4] = write_name(SIG_DYNAMIC_COMPOUND_SHAPE_DATA,
						"hknpDynamicCompoundShapeData");
				}
				names.align(16, 0xFF);

				// ordered by destination
				std::sort(local_fixups.begin(), local_fixups.end(),
					[](const std::pair<std::size_t, std::size_t>& a,
						const std::pair<std::size_t, std::size_t>& b)
					{
						return a.second < b.second;
					});

				byte_buffer fixups;
				for (const auto& fixup : local_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				// each table is padded to 16 with 0xFF
				fixups.align(16, 0xFF);
				const auto local_size = fixups.size();

				std::sort(global_fixups.begin(), global_fixups.end());
				for (const auto& fixup : global_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(2); // dstSectionIndex -- __data__
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				fixups.align(16, 0xFF);
				const auto global_size = fixups.size() - local_size;

				std::sort(virtual_fixups.begin(), virtual_fixups.end());
				for (const auto& fixup : virtual_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(0);
					fixups.write<std::int32_t>(
						static_cast<std::int32_t>(name_offsets[fixup.second]));
				}
				fixups.align(16, 0xFF);
				const auto virtual_size = fixups.size() - local_size - global_size;

				byte_buffer file;
				file.write<std::uint32_t>(HK_MAGIC0);
				file.write<std::uint32_t>(HK_MAGIC1);
				file.write<std::int32_t>(0); // userTag
				file.write<std::int32_t>(HK_FILE_VERSION);
				file.write<std::uint8_t>(8); // pointerSize
				file.write<std::uint8_t>(1); // littleEndian
				file.write<std::uint8_t>(0); // reuseBaseClassPadding
				file.write<std::uint8_t>(1); // emptyBaseClassOptimization
				file.write<std::int32_t>(3); // numSections
				file.write<std::int32_t>(2); // contentsSectionIndex
				file.write<std::int32_t>(0); // contentsSectionOffset
				file.write<std::int32_t>(0); // contentsClassNameSectionIndex
				file.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[0]));
				const char* version = "hk_2014.2.5-r1";
				const auto version_length = std::strlen(version) + 1;
				file.write(version, version_length);
				// hkPackfileHeader() memsets itself to -1 before assigning fields, so the tail
				// of contentsVersion stays 0xFF rather than 0.
				file.fill(16 - version_length, 0xFF);
				file.write<std::int32_t>(0); // flags
				file.write<std::uint16_t>(HK_MAX_PREDICATE);
				file.write<std::uint16_t>(0);

				const auto names_start = HK_HEADER_SIZE + 3 * HK_SECTION_HEADER_SIZE;
				const auto data_start = names_start + names.size();

				const auto write_section_header = [&](const char* tag, const std::size_t abs,
					const std::size_t payload, const std::size_t local, const std::size_t global,
					const std::size_t virt)
				{
					char name[19] = {};
					std::strncpy(name, tag, sizeof(name));
					file.write(name, sizeof(name));
					file.write<std::uint8_t>(0xFF); // m_nullByte
					file.write<std::int32_t>(static_cast<std::int32_t>(abs));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local + global));
					const auto end = payload + local + global + virt;
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.fill(16, 0xFF); // m_pad[4]
				};

				write_section_header("__classnames__", names_start, names.size(), 0, 0, 0);
				write_section_header("__types__", data_start, 0, 0, 0, 0);
				write_section_header("__data__", data_start, data_size, local_size, global_size,
					virtual_size);

				file.write(names.data.data(), names.size());
				file.write(buf.data.data(), buf.size());
				file.write(fixups.data.data(), fixups.size());

				auto convex_total = 0;
				for (const auto* shape : shapes)
				{
					convex_total += static_cast<int>(shape->convexes.size());
				}

				std::size_t reused = 0;
				for (const auto used : prefix_used)
				{
					reused += used ? 1 : 0;
				}

				ZONETOOL_INFO("havok: ents shape list built -- %d shapes, %d convexes, "
					"%zu surface tags (%zu inherited from the world table, %zu of those in "
					"use, %zu appended), %zu bytes", shape_count, convex_total, palette.size(),
					prefix_size, reused, palette.size() - prefix_size, file.size());

				if (out_merge)
				{
					out_merge->prefix = prefix_size;
					out_merge->total = palette.size();
					out_merge->reused = reused;
					out_merge->appended = palette.size() - prefix_size;
				}

				return file.data;
			}

			// The dummy physics asset: one static body around a placeholder box. IW7 attaches one to every script
			// brush model and trigger and physicsShapeOverrideIdx swaps the shape, but without the asset no body is
			// built.
			std::vector<std::uint8_t> build_physics_asset(const physics_asset_input& input)
			{
				// Stock value; the shape is overridden, so the size does not matter.
				constexpr auto BOX_CONVEX_RADIUS = 0.01f;

				// hknpShape::flags on this convex is 0x01C3, where the shipped ents convexes
				// use 0x0143. The extra 0x0080 tracks with this shape carrying a `properties`
				// pointer, which the ents ones do not.
				constexpr auto DUMMY_CONVEX_FLAGS = 0x01C3u;

				byte_buffer buf;
				std::vector<std::pair<std::size_t, std::size_t>> local_fixups;
				std::vector<std::pair<std::size_t, std::size_t>> global_fixups;
				std::vector<std::pair<std::size_t, int>> virtual_fixups;

				const auto align16 = [&] { buf.align(16, 0); };

				// --- HavokPhysicsAsset (144) ---
				virtual_fixups.emplace_back(static_cast<std::size_t>(0), 0);
				buf.reserve(8); // isRagdoll + pad
				const auto system_data_slot = buf.reserve(8);
				// Eight hkArrays at +16..+128. Six carry one entry each; the two
				// motionProperties/constraint lookups are empty.
				std::array<std::size_t, 8> lookup_field{};
				const int lookup_count[8] = {1, 1, 0, 1, 1, 1, 0, 1};
				for (auto i = 0; i < 8; i++)
				{
					lookup_field[i] = buf.size();
					write_hk_array_header(buf, lookup_count[i]);
				}

				if (buf.size() != 144)
				{
					ZONETOOL_ERROR("havok: HavokPhysicsAsset is %zu bytes, expected 144",
						buf.size());
					return {};
				}

				// Each non-empty lookup is a single 4-byte value in its own 16-byte slot.
				// bodyQualityNameCRCLookup and materialNameCRCLookup carry name CRCs; the
				// SFX/VFX/serverUsage/drivers entries are zero.
				const std::uint32_t lookup_value[8] = {
					input.body_quality_crc, input.material_crc, 0, 0, 0, 0, 0, 0
				};
				for (auto i = 0; i < 8; i++)
				{
					if (!lookup_count[i])
					{
						continue;
					}
					local_fixups.emplace_back(lookup_field[i], buf.size());
					buf.write<std::uint32_t>(lookup_value[i]);
					align16();
				}

				// --- hknpPhysicsSystemData (120) ---
				const auto system_data_offset = buf.size();
				global_fixups.emplace_back(system_data_slot, system_data_offset);
				virtual_fixups.emplace_back(system_data_offset, 1);

				buf.reserve(16); // hkReferencedObject
				write_hk_array_header(buf, 0); // materials
				write_hk_array_header(buf, 0); // motionProperties
				write_hk_array_header(buf, 0); // motionCinfos
				const auto body_cinfos_field = buf.size();
				write_hk_array_header(buf, 1); // bodyCinfos
				write_hk_array_header(buf, 0); // constraintCinfos
				const auto referenced_field = buf.size();
				write_hk_array_header(buf, 1); // referencedObjects
				const auto system_name_slot = buf.reserve(8); // name

				if (buf.size() - system_data_offset != 120)
				{
					ZONETOOL_ERROR("havok: hknpPhysicsSystemData is %zu bytes, expected 120",
						buf.size() - system_data_offset);
					return {};
				}

				align16();

				// --- hknpBodyCinfo (160) ---
				const auto body_offset = buf.size();
				local_fixups.emplace_back(body_cinfos_field, body_offset);

				const auto body_shape_slot = buf.reserve(8); // shape
				buf.write<std::int32_t>(0); // flags
				buf.write<std::uint32_t>(input.body_contents); // collisionFilterInfo
				buf.write<std::uint16_t>(0xFFFF); // materialId -- resolved at load
				buf.write<std::uint8_t>(0xFF); // qualityId -- likewise
				buf.reserve(5); // pad to +24
				buf.write<std::uint64_t>(0); // userData
				const auto body_name_slot = buf.reserve(8); // name
				buf.write<std::uint8_t>(0); // motionType -- 0 is static
				buf.reserve(7); // pad to +48
				// position and orientation, both identity: an hkVector4 and a quaternion,
				// each (0, 0, 0, 1).
				for (auto i = 0; i < 2; i++)
				{
					buf.write<float>(0.0f);
					buf.write<float>(0.0f);
					buf.write<float>(0.0f);
					buf.write<float>(1.0f);
				}
				buf.reserve(32); // linear and angular velocity, both zero
				buf.write<float>(-1.0f); // +112 -- unnamed in the reflection dump
				buf.reserve(12);
				// +128: four unnamed words, verbatim from the shipped dummy. The 0x7FFFFFFF
				// pair reads as "no limit" sentinels.
				buf.write<std::uint16_t>(0xFFFF);
				buf.write<std::uint16_t>(0x0000);
				buf.write<std::uint32_t>(0x7FFFFFFFu);
				buf.write<std::uint32_t>(0x7FFFFFFFu);
				buf.write<std::uint32_t>(0);
				buf.reserve(16);

				if (buf.size() - body_offset != 160)
				{
					ZONETOOL_ERROR("havok: hknpBodyCinfo is %zu bytes, expected 160",
						buf.size() - body_offset);
					return {};
				}

				// body name string
				local_fixups.emplace_back(body_name_slot, buf.size());
				buf.write(input.body_name.c_str(), input.body_name.size() + 1);
				align16();

				// referencedObjects payload: one pointer, back at the convex shape
				local_fixups.emplace_back(referenced_field, buf.size());
				const auto referenced_slot = buf.reserve(8);
				align16();

				// system name string
				local_fixups.emplace_back(system_name_slot, buf.size());
				const char* system_name = "Default Physics System Data";
				buf.write(system_name, std::strlen(system_name) + 1);
				align16();

				// --- hknpConvexPolytopeShape (96) + payloads ---
				const auto convex_offset = buf.size();
				global_fixups.emplace_back(body_shape_slot, convex_offset);
				global_fixups.emplace_back(referenced_slot, convex_offset);
				virtual_fixups.emplace_back(convex_offset, 2);

				buf.reserve(16); // hkReferencedObject
				buf.write<std::uint16_t>(static_cast<std::uint16_t>(DUMMY_CONVEX_FLAGS));
				buf.write<std::uint8_t>(0); // numShapeKeyBits
				buf.write<std::uint8_t>(static_cast<std::uint8_t>(CONVEX_DISPATCH_TYPE));
				buf.write<float>(BOX_CONVEX_RADIUS);
				buf.write<std::uint64_t>(0); // userData
				const auto properties_slot = buf.reserve(8);
				buf.reserve(8); // pad -- hknpShape is 48 bytes
				const auto vertices_field = buf.size();
				buf.reserve(4);
				buf.reserve(12); // pad to +64
				const auto planes_field = buf.size();
				buf.reserve(4);
				const auto faces_field = buf.size();
				buf.reserve(4);
				const auto indices_field = buf.size();
				buf.reserve(4);
				buf.reserve(4); // pad to +80
				buf.reserve(8); // connectivity -- null on the shipped dummies
				buf.reserve(8); // pad to +96

				if (buf.size() - convex_offset != SIZEOF_CONVEX_POLYTOPE_SHAPE)
				{
					ZONETOOL_ERROR("havok: dummy hknpConvexPolytopeShape is %zu bytes, "
						"expected %d", buf.size() - convex_offset,
						SIZEOF_CONVEX_POLYTOPE_SHAPE);
					return {};
				}

				const auto write_rel_array = [&](const std::size_t field,
					const std::size_t count)
				{
					buf.patch<std::uint16_t>(field, static_cast<std::uint16_t>(count));
					buf.patch<std::uint16_t>(field + 2,
						static_cast<std::uint16_t>(buf.size() - field));
				};

				// Bit 0 of the vertex index selects -x, bit 1 -y, bit 2 -z, and the w lane carries the index. Bit
				// patterns copied from the shipped dummy: half-extents 20/25/15 shrunk by the convex radius.
				static const std::uint32_t DUMMY_VERTS[32] = {
					0x3F1D701E, 0x3F456FFC, 0x3EEAE07E, 0x3F000000,
					0x3F1D701E, 0xBF456FFC, 0x3EEAE07E, 0x3F000001,
					0x3F1D701E, 0x3F456FFC, 0xBEEAE07E, 0x3F000002,
					0xBF1D701E, 0x3F456FFC, 0x3EEAE07E, 0x3F000003,
					0x3F1D701E, 0xBF456FFC, 0xBEEAE07E, 0x3F000004,
					0xBF1D701E, 0x3F456FFC, 0xBEEAE07E, 0x3F000005,
					0xBF1D701E, 0xBF456FFC, 0x3EEAE07E, 0x3F000006,
					0xBF1D701E, 0xBF456FFC, 0xBEEAE07E, 0x3F000007,
				};
				write_rel_array(vertices_field, 8);
				for (const auto word : DUMMY_VERTS)
				{
					buf.write<std::uint32_t>(word);
				}
				align16();

				// Planes in shipped order (+z, -z, +x, -x, +y, -y), bit-exact because several normal lanes carry -0.0f.
				static const std::uint32_t DUMMY_PLANES[24] = {
					0x00000000, 0x00000000, 0x3F800000, 0xBEEAE07E,
					0x80000000, 0x00000000, 0xBF800000, 0xBEEAE07E,
					0x3F800000, 0x00000000, 0x00000000, 0xBF1D701E,
					0xBF800000, 0x80000000, 0x00000000, 0xBF1D701E,
					0x00000000, 0x3F800000, 0x00000000, 0xBF456FFC,
					0x80000000, 0xBF800000, 0x00000000, 0xBF456FFC,
				};
				write_rel_array(planes_field, 6);
				for (const auto word : DUMMY_PLANES)
				{
					buf.write<std::uint32_t>(word);
				}
				align16();

				write_rel_array(faces_field, 6);
				for (auto i = 0; i < 6; i++)
				{
					buf.write<std::uint16_t>(static_cast<std::uint16_t>(i * 4));
					buf.write<std::uint8_t>(4);
					buf.write<std::uint8_t>(127); // every dihedral on a box is a right angle
				}
				align16();

				// Face index runs, verbatim from the shipped dummy -- counter-clockwise about
				// each outward normal given the vertex order above.
				static const std::uint8_t FACE_INDICES[24] = {
					0, 3, 6, 1,  7, 5, 2, 4,  0, 1, 4, 2,
					3, 5, 7, 6,  5, 3, 0, 2,  7, 4, 1, 6,
				};
				write_rel_array(indices_field, 24);
				for (const auto index : FACE_INDICES)
				{
					buf.write<std::uint8_t>(index);
				}
				align16();

				// --- hkRefCountedProperties (16) ---
				const auto properties_offset = buf.size();
				global_fixups.emplace_back(properties_slot, properties_offset);
				virtual_fixups.emplace_back(properties_offset, 3);
				const auto entries_field = buf.size();
				write_hk_array_header(buf, 1);

				local_fixups.emplace_back(entries_field, buf.size());
				const auto mass_slot = buf.reserve(8);
				buf.write<std::uint16_t>(0xF100); // key -- the mass-properties property id
				buf.write<std::uint16_t>(0); // flags
				buf.reserve(4);
				align16();

				// --- hknpShapeMassProperties (48) ---
				const auto mass_offset = buf.size();
				global_fixups.emplace_back(mass_slot, mass_offset);
				virtual_fixups.emplace_back(mass_offset, 4);
				buf.reserve(16); // hkReferencedObject
				// hkCompressedMassProperties, copied from the shipped dummy.
				static const std::uint8_t COMPRESSED_MASS[32] = {
					0x36, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x80, 0x22,
					0x68, 0x42, 0xD4, 0x30, 0x14, 0x50, 0x00, 0x31,
					0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x30, 0xF5,
					0x00, 0x00, 0xA0, 0x40, 0xD1, 0x5D, 0xEA, 0x3F,
				};
				buf.write(COMPRESSED_MASS, sizeof(COMPRESSED_MASS));

				if (buf.size() - mass_offset != 48)
				{
					ZONETOOL_ERROR("havok: hknpShapeMassProperties is %zu bytes, expected 48",
						buf.size() - mass_offset);
					return {};
				}

				align16();
				const auto data_size = buf.size();

				byte_buffer names;
				const auto write_name = [&](const std::uint32_t sig, const char* name)
				{
					names.write<std::uint32_t>(sig);
					names.write<std::uint8_t>(0x09);
					const auto offset = names.size();
					names.write(name, std::strlen(name) + 1);
					return offset;
				};

				write_name(SIG_HK_CLASS, "hkClass");
				write_name(SIG_HK_CLASS_MEMBER, "hkClassMember");
				write_name(SIG_HK_CLASS_ENUM, "hkClassEnum");
				write_name(SIG_HK_CLASS_ENUM_ITEM, "hkClassEnumItem");

				std::array<std::size_t, 5> name_offsets{};
				name_offsets[0] = write_name(SIG_PHYSICS_ASSET, "HavokPhysicsAsset");
				name_offsets[1] = write_name(SIG_PHYSICS_SYSTEM_DATA, "hknpPhysicsSystemData");
				name_offsets[2] = write_name(SIG_CONVEX_POLYTOPE_SHAPE,
					"hknpConvexPolytopeShape");
				name_offsets[3] = write_name(SIG_REF_COUNTED_PROPERTIES,
					"hkRefCountedProperties");
				name_offsets[4] = write_name(SIG_SHAPE_MASS_PROPERTIES,
					"hknpShapeMassProperties");
				names.align(16, 0xFF);

				// ordered by destination
				std::sort(local_fixups.begin(), local_fixups.end(),
					[](const std::pair<std::size_t, std::size_t>& a,
						const std::pair<std::size_t, std::size_t>& b)
					{
						return a.second < b.second;
					});
				byte_buffer fixups;
				for (const auto& fixup : local_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				fixups.align(16, 0xFF);
				const auto local_size = fixups.size();

				std::sort(global_fixups.begin(), global_fixups.end());
				for (const auto& fixup : global_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(2); // dstSectionIndex -- __data__
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				fixups.align(16, 0xFF);
				const auto global_size = fixups.size() - local_size;

				std::sort(virtual_fixups.begin(), virtual_fixups.end());
				for (const auto& fixup : virtual_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(0);
					fixups.write<std::int32_t>(
						static_cast<std::int32_t>(name_offsets[fixup.second]));
				}
				fixups.align(16, 0xFF);
				const auto virtual_size = fixups.size() - local_size - global_size;

				byte_buffer file;
				file.write<std::uint32_t>(HK_MAGIC0);
				file.write<std::uint32_t>(HK_MAGIC1);
				file.write<std::int32_t>(0); // userTag
				file.write<std::int32_t>(HK_FILE_VERSION);
				file.write<std::uint8_t>(8); // pointerSize
				file.write<std::uint8_t>(1); // littleEndian
				file.write<std::uint8_t>(0); // reuseBaseClassPadding
				file.write<std::uint8_t>(1); // emptyBaseClassOptimization
				file.write<std::int32_t>(3); // numSections
				file.write<std::int32_t>(2); // contentsSectionIndex
				file.write<std::int32_t>(0); // contentsSectionOffset
				file.write<std::int32_t>(0); // contentsClassNameSectionIndex
				file.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[0]));
				const char* version = "hk_2014.2.5-r1";
				const auto version_length = std::strlen(version) + 1;
				file.write(version, version_length);
				file.fill(16 - version_length, 0xFF);
				file.write<std::int32_t>(0); // flags
				file.write<std::uint16_t>(HK_MAX_PREDICATE);
				file.write<std::uint16_t>(0);

				const auto names_start = HK_HEADER_SIZE + 3 * HK_SECTION_HEADER_SIZE;
				const auto data_start = names_start + names.size();

				const auto write_section_header = [&](const char* tag, const std::size_t abs,
					const std::size_t payload, const std::size_t local, const std::size_t global,
					const std::size_t virt)
				{
					char name[19] = {};
					std::strncpy(name, tag, sizeof(name));
					file.write(name, sizeof(name));
					file.write<std::uint8_t>(0xFF); // m_nullByte
					file.write<std::int32_t>(static_cast<std::int32_t>(abs));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local + global));
					const auto end = payload + local + global + virt;
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.fill(16, 0xFF); // m_pad[4]
				};

				write_section_header("__classnames__", names_start, names.size(), 0, 0, 0);
				write_section_header("__types__", data_start, 0, 0, 0, 0);
				write_section_header("__data__", data_start, data_size, local_size, global_size,
					virtual_size);

				file.write(names.data.data(), names.size());
				file.write(buf.data.data(), buf.size());
				file.write(fixups.data.data(), fixups.size());

				ZONETOOL_INFO("havok: physics asset \"%s\" built -- %zu bytes",
					input.body_name.c_str(), file.size());

				return file.data;
			}

			// A model that moves: hknpPhysicsSystemData with a dynamic body over a convex shape built from the
			// model's PhysCollmap, with mass properties on every shape.

			namespace
			{
				struct mass_properties
				{
					float volume = 0.0f;
					float mass = 0.0f;
					float com[3] = {0.0f, 0.0f, 0.0f};
					// full inertia tensor about `com`, for the given mass
					float inertia[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
				};

				// Volume, centroid and inertia of a closed polytope with uniform density 1,
				// faces wound counter-clockwise about their outward normals. Eberly,
				// "Polyhedral Mass Properties (Revisited)".
				mass_properties polytope_mass(const polytope& convex, const float scale)
				{
					constexpr float mult[10] = {
						1.0f / 6.0f, 1.0f / 24.0f, 1.0f / 24.0f, 1.0f / 24.0f,
						1.0f / 60.0f, 1.0f / 60.0f, 1.0f / 60.0f,
						1.0f / 120.0f, 1.0f / 120.0f, 1.0f / 120.0f
					};
					double intg[10] = {};

					const auto subexpr = [](const double w0, const double w1, const double w2,
						double& f1, double& f2, double& f3, double& g0, double& g1, double& g2)
					{
						const auto temp0 = w0 + w1;
						f1 = temp0 + w2;
						const auto temp1 = w0 * w0;
						const auto temp2 = temp1 + w1 * temp0;
						f2 = temp2 + w2 * f1;
						f3 = w0 * temp1 + w1 * temp2 + w2 * f2;
						g0 = f2 + w0 * (f1 + w0);
						g1 = f2 + w1 * (f1 + w1);
						g2 = f2 + w2 * (f1 + w2);
					};

					for (const auto& face : convex.faces)
					{
						for (std::size_t t = 2; t < face.indices.size(); t++)
						{
							const std::size_t corner[3] = {
								face.indices[0], face.indices[t - 1], face.indices[t]
							};
							if (corner[0] >= convex.verts.size() || corner[1] >= convex.verts.size()
								|| corner[2] >= convex.verts.size())
							{
								continue;
							}

							double p[3][3];
							for (auto c = 0; c < 3; c++)
							{
								for (auto k = 0; k < 3; k++)
								{
									p[c][k] = static_cast<double>(convex.verts[corner[c]][k]) * scale;
								}
							}

							const double a1 = p[1][0] - p[0][0], b1 = p[1][1] - p[0][1], c1 = p[1][2] - p[0][2];
							const double a2 = p[2][0] - p[0][0], b2 = p[2][1] - p[0][1], c2 = p[2][2] - p[0][2];
							const double d0 = b1 * c2 - b2 * c1;
							const double d1 = a2 * c1 - a1 * c2;
							const double d2 = a1 * b2 - a2 * b1;

							double f1x, f2x, f3x, g0x, g1x, g2x;
							double f1y, f2y, f3y, g0y, g1y, g2y;
							double f1z, f2z, f3z, g0z, g1z, g2z;
							subexpr(p[0][0], p[1][0], p[2][0], f1x, f2x, f3x, g0x, g1x, g2x);
							subexpr(p[0][1], p[1][1], p[2][1], f1y, f2y, f3y, g0y, g1y, g2y);
							subexpr(p[0][2], p[1][2], p[2][2], f1z, f2z, f3z, g0z, g1z, g2z);

							intg[0] += d0 * f1x;
							intg[1] += d0 * f2x;
							intg[2] += d1 * f2y;
							intg[3] += d2 * f2z;
							intg[4] += d0 * f3x;
							intg[5] += d1 * f3y;
							intg[6] += d2 * f3z;
							intg[7] += d0 * (p[0][1] * g0x + p[1][1] * g1x + p[2][1] * g2x);
							intg[8] += d1 * (p[0][2] * g0y + p[1][2] * g1y + p[2][2] * g2y);
							intg[9] += d2 * (p[0][0] * g0z + p[1][0] * g1z + p[2][0] * g2z);
						}
					}

					for (auto i = 0; i < 10; i++)
					{
						intg[i] *= mult[i];
					}

					mass_properties out{};
					const auto volume = intg[0];
					if (volume <= 1e-12)
					{
						return out;
					}

					const double cx = intg[1] / volume, cy = intg[2] / volume, cz = intg[3] / volume;
					// inertia about the centroid, unit density
					const double ixx = intg[5] + intg[6] - volume * (cy * cy + cz * cz);
					const double iyy = intg[4] + intg[6] - volume * (cz * cz + cx * cx);
					const double izz = intg[4] + intg[5] - volume * (cx * cx + cy * cy);
					const double ixy = -(intg[7] - volume * cx * cy);
					const double iyz = -(intg[8] - volume * cy * cz);
					const double ixz = -(intg[9] - volume * cz * cx);

					out.volume = static_cast<float>(volume);
					out.mass = static_cast<float>(volume);
					out.com[0] = static_cast<float>(cx);
					out.com[1] = static_cast<float>(cy);
					out.com[2] = static_cast<float>(cz);
					out.inertia[0][0] = static_cast<float>(ixx);
					out.inertia[1][1] = static_cast<float>(iyy);
					out.inertia[2][2] = static_cast<float>(izz);
					out.inertia[0][1] = out.inertia[1][0] = static_cast<float>(ixy);
					out.inertia[1][2] = out.inertia[2][1] = static_cast<float>(iyz);
					out.inertia[0][2] = out.inertia[2][0] = static_cast<float>(ixz);
					return out;
				}

				// Rescale a unit-density result to a real mass.
				void set_mass(mass_properties& mp, const float mass)
				{
					if (mp.volume <= 0.0f)
					{
						mp.mass = mass;
						return;
					}
					const auto factor = mass / mp.volume;
					mp.mass = mass;
					for (auto& row : mp.inertia)
					{
						for (auto& value : row)
						{
							value *= factor;
						}
					}
				}

				// Combine children (each about its own centroid) into one body: mass-weighted
				// centroid, parallel-axis shift of every child tensor.
				mass_properties combine_mass(const std::vector<mass_properties>& parts)
				{
					mass_properties out{};
					for (const auto& p : parts)
					{
						out.volume += p.volume;
						out.mass += p.mass;
						for (auto k = 0; k < 3; k++)
						{
							out.com[k] += p.com[k] * p.mass;
						}
					}
					if (out.mass <= 0.0f)
					{
						return out;
					}
					for (auto k = 0; k < 3; k++)
					{
						out.com[k] /= out.mass;
					}
					for (const auto& p : parts)
					{
						const float d[3] = {p.com[0] - out.com[0], p.com[1] - out.com[1], p.com[2] - out.com[2]};
						const auto dd = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
						for (auto i = 0; i < 3; i++)
						{
							for (auto j = 0; j < 3; j++)
							{
								out.inertia[i][j] += p.inertia[i][j]
									+ p.mass * ((i == j ? dd : 0.0f) - d[i] * d[j]);
							}
						}
					}
					return out;
				}

				// Symmetric 3x3 eigen-decomposition (cyclic Jacobi). `axes` columns are the
				// principal axes, `diag` the principal moments.
				void diagonalise(const float (&m)[3][3], float (&diag)[3], float (&axes)[3][3])
				{
					double a[3][3], v[3][3];
					for (auto i = 0; i < 3; i++)
					{
						for (auto j = 0; j < 3; j++)
						{
							a[i][j] = m[i][j];
							v[i][j] = i == j ? 1.0 : 0.0;
						}
					}

					for (auto sweep = 0; sweep < 50; sweep++)
					{
						const auto off = a[0][1] * a[0][1] + a[0][2] * a[0][2] + a[1][2] * a[1][2];
						if (off < 1e-24)
						{
							break;
						}
						for (auto p = 0; p < 3; p++)
						{
							for (auto q = p + 1; q < 3; q++)
							{
								if (std::fabs(a[p][q]) < 1e-30)
								{
									continue;
								}
								const auto theta = (a[q][q] - a[p][p]) / (2.0 * a[p][q]);
								const auto t = (theta >= 0.0 ? 1.0 : -1.0)
									/ (std::fabs(theta) + std::sqrt(theta * theta + 1.0));
								const auto c = 1.0 / std::sqrt(t * t + 1.0);
								const auto s = t * c;
								for (auto k = 0; k < 3; k++)
								{
									const auto akp = a[k][p], akq = a[k][q];
									a[k][p] = c * akp - s * akq;
									a[k][q] = s * akp + c * akq;
								}
								for (auto k = 0; k < 3; k++)
								{
									const auto apk = a[p][k], aqk = a[q][k];
									a[p][k] = c * apk - s * aqk;
									a[q][k] = s * apk + c * aqk;
								}
								for (auto k = 0; k < 3; k++)
								{
									const auto vkp = v[k][p], vkq = v[k][q];
									v[k][p] = c * vkp - s * vkq;
									v[k][q] = s * vkp + c * vkq;
								}
							}
						}
					}

					// keep a right-handed frame so it converts to a rotation
					const double det = v[0][0] * (v[1][1] * v[2][2] - v[1][2] * v[2][1])
						- v[0][1] * (v[1][0] * v[2][2] - v[1][2] * v[2][0])
						+ v[0][2] * (v[1][0] * v[2][1] - v[1][1] * v[2][0]);
					if (det < 0.0)
					{
						for (auto k = 0; k < 3; k++)
						{
							v[k][2] = -v[k][2];
						}
					}

					for (auto i = 0; i < 3; i++)
					{
						diag[i] = static_cast<float>(std::max(a[i][i], 0.0));
						for (auto j = 0; j < 3; j++)
						{
							axes[i][j] = static_cast<float>(v[i][j]);
						}
					}
				}

				// Rotation matrix (columns = axes) to quaternion (x, y, z, w).
				void quat_from_axes(const float (&r)[3][3], float (&q)[4])
				{
					const auto trace = r[0][0] + r[1][1] + r[2][2];
					if (trace > 0.0f)
					{
						const auto s = std::sqrt(trace + 1.0f) * 2.0f;
						q[3] = 0.25f * s;
						q[0] = (r[2][1] - r[1][2]) / s;
						q[1] = (r[0][2] - r[2][0]) / s;
						q[2] = (r[1][0] - r[0][1]) / s;
					}
					else if (r[0][0] > r[1][1] && r[0][0] > r[2][2])
					{
						const auto s = std::sqrt(1.0f + r[0][0] - r[1][1] - r[2][2]) * 2.0f;
						q[3] = (r[2][1] - r[1][2]) / s;
						q[0] = 0.25f * s;
						q[1] = (r[0][1] + r[1][0]) / s;
						q[2] = (r[0][2] + r[2][0]) / s;
					}
					else if (r[1][1] > r[2][2])
					{
						const auto s = std::sqrt(1.0f + r[1][1] - r[0][0] - r[2][2]) * 2.0f;
						q[3] = (r[0][2] - r[2][0]) / s;
						q[0] = (r[0][1] + r[1][0]) / s;
						q[1] = 0.25f * s;
						q[2] = (r[1][2] + r[2][1]) / s;
					}
					else
					{
						const auto s = std::sqrt(1.0f + r[2][2] - r[0][0] - r[1][1]) * 2.0f;
						q[3] = (r[1][0] - r[0][1]) / s;
						q[0] = (r[0][2] + r[2][0]) / s;
						q[1] = (r[1][2] + r[2][1]) / s;
						q[2] = 0.25f * s;
					}
					const auto length = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
					if (length > 0.0f)
					{
						for (auto& c : q)
						{
							c /= length;
						}
					}
				}

				// hkPackedVector3: three int16 lanes and a shared power-of-two scale kept as the top 16 bits of a float.
				// The exponent is the smallest that keeps the largest lane inside int16.
				void pack_vector3(const float* v, std::uint16_t (&out)[4])
				{
					auto largest = 0.0f;
					for (auto k = 0; k < 3; k++)
					{
						largest = std::max(largest, std::fabs(v[k]));
					}
					auto k = -46;
					if (largest > 0.0f)
					{
						int exponent = 0;
						std::frexp(largest / 32767.0f, &exponent); // largest/32767 = m * 2^exponent, m in [0.5,1)
						k = exponent - 16;
					}
					const auto scale = std::ldexp(1.0f, 16 + k);
					for (auto i = 0; i < 3; i++)
					{
						const auto lane = static_cast<int>(std::lround(v[i] / scale));
						out[i] = static_cast<std::uint16_t>(static_cast<std::int16_t>(
							std::max(-32767, std::min(32767, lane))));
					}
					out[3] = static_cast<std::uint16_t>((127 + k) << 7);
				}

				// hkPackedUnitVector as hkCompressedMassProperties::pack uses it: the high
				// half of (int)(q * 30000 * 65536) + 0x80000000 per lane. Unpack normalises,
				// so the scale only sets precision; 30000 reproduces the stock words.
				void pack_quat(const float (&q)[4], std::uint16_t (&out)[4])
				{
					for (auto i = 0; i < 4; i++)
					{
						const auto scaled = static_cast<std::int32_t>(q[i] * 1966080000.0f);
						const auto shifted = static_cast<std::uint32_t>(scaled) + 0x80000000u;
						out[i] = static_cast<std::uint16_t>(shifted >> 16);
					}
				}

				struct compressed_mass
				{
					std::uint16_t com[4];
					std::uint16_t inertia[4];
					std::uint16_t axes[4];
					float mass;
					float volume;
					// the diagonalised form, for the motion cinfo
					float diag[3];
					float quat[4];
				};

				compressed_mass compress_mass(const mass_properties& mp)
				{
					compressed_mass out{};
					float axes[3][3];
					diagonalise(mp.inertia, out.diag, axes);
					quat_from_axes(axes, out.quat);
					pack_vector3(mp.com, out.com);
					pack_vector3(out.diag, out.inertia);
					pack_quat(out.quat, out.axes);
					out.mass = mp.mass;
					out.volume = mp.volume;
					return out;
				}
			}

			std::vector<std::uint8_t> build_dynamic_physics_asset(const dynamic_physics_asset_input& input)
			{
				struct half_edge
				{
					std::uint16_t face = 0;
					std::uint8_t edge = 0;
				};

				// Body quality / material / motion-properties name CRCs live in the asset's
				// lookup arrays, exactly as for the static dummy, plus the third one that
				// every dynamic stock asset fills (0x9F53AC92 on all of them).
				constexpr auto MOTION_INFINITE = 0x5F7FFFF0u; // maxLinearAccelerationDistancePerStep / maxRotationPerStep
				constexpr auto CHILD_DENSITY = 1000.0f; // stock child mass properties are volume x 1000

				if (input.convexes.empty())
				{
					ZONETOOL_ERROR("havok: dynamic asset \"%s\" has no convexes", input.body_name.c_str());
					return {};
				}

				byte_buffer buf;
				std::vector<std::pair<std::size_t, std::size_t>> local_fixups;
				std::vector<std::pair<std::size_t, std::size_t>> global_fixups;
				std::vector<std::pair<std::size_t, int>> virtual_fixups;
				const auto align16 = [&] { buf.align(16, 0); };

				// class-name indices, in the order written to __classnames__ below
				enum : int
				{
					CLASS_ASSET, CLASS_SYSTEM_DATA, CLASS_CONVEX, CLASS_PROPERTIES,
					CLASS_MASS, CLASS_CONNECTIVITY, CLASS_COMPOUND, CLASS_COMPOUND_DATA,
					CLASS_COUNT
				};

				const auto compound = input.convexes.size() > 1;
				const auto instance_count = static_cast<int>(input.convexes.size());

				std::vector<mass_properties> child_mass;
				for (const auto& convex : input.convexes)
				{
					child_mass.emplace_back(polytope_mass(convex, input.scale));
				}
				auto body_mass = combine_mass(child_mass);
				if (body_mass.volume <= 0.0f)
				{
					ZONETOOL_WARNING("havok: dynamic asset \"%s\" has no volume -- hull(s) not "
						"closed?", input.body_name.c_str());
				}
				set_mass(body_mass, input.mass);
				for (auto& child : child_mass)
				{
					set_mass(child, child.volume * CHILD_DENSITY);
				}
				const auto body_compressed = compress_mass(body_mass);

				// --- HavokPhysicsAsset (144) ---
				virtual_fixups.emplace_back(static_cast<std::size_t>(0), CLASS_ASSET);
				buf.reserve(8); // isRagdoll + pad
				const auto system_data_slot = buf.reserve(8);
				std::array<std::size_t, 8> lookup_field{};
				const int lookup_count[8] = {1, 1, 1, 1, 1, 1, 0, 1};
				for (auto i = 0; i < 8; i++)
				{
					lookup_field[i] = buf.size();
					write_hk_array_header(buf, lookup_count[i]);
				}
				const std::uint32_t lookup_value[8] = {
					input.body_quality_crc, input.material_crc, input.motion_properties_crc, 0, 0, 0, 0, 0
				};
				for (auto i = 0; i < 8; i++)
				{
					if (!lookup_count[i])
					{
						continue;
					}
					local_fixups.emplace_back(lookup_field[i], buf.size());
					buf.write<std::uint32_t>(lookup_value[i]);
					align16();
				}

				// --- hknpPhysicsSystemData (120) ---
				const auto system_data_offset = buf.size();
				global_fixups.emplace_back(system_data_slot, system_data_offset);
				virtual_fixups.emplace_back(system_data_offset, CLASS_SYSTEM_DATA);
				buf.reserve(16); // hkReferencedObject
				write_hk_array_header(buf, 0); // materials
				write_hk_array_header(buf, 0); // motionProperties
				const auto motion_cinfos_field = buf.size();
				write_hk_array_header(buf, 1); // motionCinfos
				const auto body_cinfos_field = buf.size();
				write_hk_array_header(buf, 1); // bodyCinfos
				write_hk_array_header(buf, 0); // constraintCinfos
				const auto referenced_field = buf.size();
				write_hk_array_header(buf, 1); // referencedObjects
				const auto system_name_slot = buf.reserve(8); // name
				align16();

				// --- hknpMotionCinfo (96) ---
				local_fixups.emplace_back(motion_cinfos_field, buf.size());
				buf.write<std::uint16_t>(0xFFFF); // motionPropertiesId -- resolved from the lookup
				buf.write<std::uint8_t>(1); // enableDeactivation
				buf.write<std::uint8_t>(0);
				buf.write<float>(body_mass.mass > 0.0f ? 1.0f / body_mass.mass : 0.0f); // inverseMass
				buf.write<std::uint32_t>(MOTION_INFINITE);
				buf.write<std::uint32_t>(MOTION_INFINITE);
				for (auto k = 0; k < 3; k++)
				{
					buf.write<float>(body_compressed.diag[k] > 0.0f ? 1.0f / body_compressed.diag[k] : 0.0f);
				}
				buf.write<float>(1.0f); // inverseInertiaLocal.w
				for (auto k = 0; k < 3; k++)
				{
					buf.write<float>(body_mass.com[k]); // centerOfMassWorld -- body at the origin
				}
				buf.write<float>(0.0f);
				for (auto k = 0; k < 4; k++)
				{
					buf.write<float>(body_compressed.quat[k]); // orientation = principal axes
				}
				buf.reserve(32); // linear + angular velocity
				align16();

				// --- hknpBodyCinfo (160) --- as the static dummy, but pointing at motion 0
				const auto body_offset = buf.size();
				local_fixups.emplace_back(body_cinfos_field, body_offset);
				const auto body_shape_slot = buf.reserve(8);
				buf.write<std::int32_t>(0); // flags
				buf.write<std::uint32_t>(input.body_contents); // collisionFilterInfo
				buf.write<std::uint16_t>(0xFFFF); // materialId
				buf.write<std::uint8_t>(0xFF); // qualityId
				buf.reserve(5);
				buf.write<std::uint64_t>(0); // userData
				const auto body_name_slot = buf.reserve(8);
				buf.write<std::uint8_t>(0); // motionType
				buf.reserve(7);
				for (auto i = 0; i < 2; i++)
				{
					buf.write<float>(0.0f);
					buf.write<float>(0.0f);
					buf.write<float>(0.0f);
					buf.write<float>(1.0f);
				}
				buf.reserve(32); // velocities
				buf.write<float>(-1.0f); // mass -- "use the shape's"
				buf.reserve(12);
				buf.write<std::uint16_t>(0xFFFF); // motionPropertiesId
				buf.write<std::uint16_t>(0);
				buf.write<std::uint32_t>(0x7FFFFFFFu); // reservedBodyId
				buf.write<std::uint32_t>(0); // reservedMotionId -> motionCinfos[0]: this is what makes it dynamic
				buf.write<std::uint32_t>(0); // collisionLookAheadDistance
				buf.reserve(16);
				if (buf.size() - body_offset != 160)
				{
					ZONETOOL_ERROR("havok: hknpBodyCinfo is %zu bytes, expected 160", buf.size() - body_offset);
					return {};
				}

				local_fixups.emplace_back(body_name_slot, buf.size());
				buf.write(input.body_name.c_str(), input.body_name.size() + 1);
				align16();

				local_fixups.emplace_back(referenced_field, buf.size());
				const auto referenced_slot = buf.reserve(8);
				align16();

				local_fixups.emplace_back(system_name_slot, buf.size());
				const char* system_name = "Default Physics System Data";
				buf.write(system_name, std::strlen(system_name) + 1);
				align16();

				const auto write_rel_array = [&](const std::size_t field, const std::size_t count)
				{
					buf.patch<std::uint16_t>(field, static_cast<std::uint16_t>(count));
					buf.patch<std::uint16_t>(field + 2, static_cast<std::uint16_t>(buf.size() - field));
				};

				// hkRefCountedProperties -> hknpShapeMassProperties, hung off `properties_slot`
				const auto write_mass_properties = [&](const std::size_t properties_slot,
					const compressed_mass& cm)
				{
					const auto properties_offset = buf.size();
					global_fixups.emplace_back(properties_slot, properties_offset);
					virtual_fixups.emplace_back(properties_offset, CLASS_PROPERTIES);
					const auto entries_field = buf.size();
					write_hk_array_header(buf, 1);
					local_fixups.emplace_back(entries_field, buf.size());
					const auto mass_slot = buf.reserve(8);
					buf.write<std::uint16_t>(0xF100); // key -- mass properties
					buf.write<std::uint16_t>(0);
					buf.reserve(4);
					align16();

					const auto mass_offset = buf.size();
					global_fixups.emplace_back(mass_slot, mass_offset);
					virtual_fixups.emplace_back(mass_offset, CLASS_MASS);
					buf.reserve(16); // hkReferencedObject
					for (const auto w : cm.com) buf.write<std::uint16_t>(w);
					for (const auto w : cm.inertia) buf.write<std::uint16_t>(w);
					for (const auto w : cm.axes) buf.write<std::uint16_t>(w);
					buf.write<float>(cm.mass);
					buf.write<float>(cm.volume);
					align16();
				};

				// One hknpConvexPolytopeShape + payloads + connectivity. Same bytes as the
				// ents writer, plus a properties pointer carrying the mass properties.
				// Returns the object offset, or 0 on failure.
				const auto write_convex = [&](const polytope& convex, const compressed_mass& cm,
					const int index, float (&mn)[3], float (&mx)[3]) -> std::size_t
				{
					auto index_total = 0u;
					for (const auto& face : convex.faces)
					{
						index_total += static_cast<unsigned int>(face.indices.size());
					}
					const auto vertex_count = padded_vertex_count(convex.verts.size());
					if (convex.verts.empty() || vertex_count > 255 || convex.faces.size() > 0xFFFF
						|| index_total > 0xFFFF)
					{
						ZONETOOL_ERROR("havok: convex %d of \"%s\" exceeds the format limits "
							"(%zu verts, %zu faces, %u indices)", index, input.body_name.c_str(),
							convex.verts.size(), convex.faces.size(), index_total);
						return 0;
					}

					const auto convex_offset = buf.size();
					virtual_fixups.emplace_back(convex_offset, CLASS_CONVEX);
					buf.reserve(16); // hkReferencedObject
					buf.write<std::uint16_t>(static_cast<std::uint16_t>(CONVEX_SHAPE_FLAGS));
					buf.write<std::uint8_t>(0); // numShapeKeyBits
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(CONVEX_DISPATCH_TYPE));
					buf.write<float>(0.0f); // convexRadius
					buf.write<std::uint64_t>(0); // userData
					const auto properties_slot = buf.reserve(8);
					buf.reserve(8);
					const auto vertices_field = buf.size();
					buf.reserve(4);
					buf.reserve(12);
					const auto planes_field = buf.size();
					buf.reserve(4);
					const auto faces_field = buf.size();
					buf.reserve(4);
					const auto indices_field = buf.size();
					buf.reserve(4);
					buf.reserve(4);
					const auto connectivity_slot = buf.reserve(8);
					buf.reserve(8);
					if (buf.size() - convex_offset != SIZEOF_CONVEX_POLYTOPE_SHAPE)
					{
						ZONETOOL_ERROR("havok: hknpConvexPolytopeShape is %zu bytes, expected %d",
							buf.size() - convex_offset, SIZEOF_CONVEX_POLYTOPE_SHAPE);
						return 0;
					}

					// connectivity, as in build_ents_shape_list
					std::vector<half_edge> vertex_edges(vertex_count);
					std::vector<bool> vertex_seen(vertex_count, false);
					std::vector<half_edge> face_links(index_total);
					std::map<std::pair<std::uint8_t, std::uint8_t>, half_edge> directed;
					for (auto f = 0u; f < convex.faces.size(); f++)
					{
						const auto& face = convex.faces[f];
						for (auto e = 0u; e < face.indices.size(); e++)
						{
							const auto from = face.indices[e];
							const auto to = face.indices[(e + 1) % face.indices.size()];
							const half_edge self{static_cast<std::uint16_t>(f), static_cast<std::uint8_t>(e)};
							if (from < vertex_seen.size() && !vertex_seen[from])
							{
								vertex_seen[from] = true;
								vertex_edges[from] = self;
							}
							directed[{from, to}] = self;
						}
					}
					auto global = 0u;
					auto unmatched = 0;
					for (auto f = 0u; f < convex.faces.size(); f++)
					{
						const auto& face = convex.faces[f];
						for (auto e = 0u; e < face.indices.size(); e++)
						{
							const auto from = face.indices[e];
							const auto to = face.indices[(e + 1) % face.indices.size()];
							const auto twin = directed.find({to, from});
							if (twin != directed.end())
							{
								face_links[global] = twin->second;
							}
							else
							{
								face_links[global] = {static_cast<std::uint16_t>(f), static_cast<std::uint8_t>(e)};
								unmatched++;
							}
							global++;
						}
					}
					if (unmatched)
					{
						ZONETOOL_WARNING("havok: convex %d of \"%s\" has %d unpaired edge(s) -- "
							"hull is not closed", index, input.body_name.c_str(), unmatched);
					}

					write_rel_array(vertices_field, vertex_count);
					for (auto j = 0u; j < vertex_count; j++)
					{
						const auto source = std::min<std::size_t>(j, convex.verts.size() - 1);
						const auto& v = convex.verts[source];
						for (auto k = 0; k < 3; k++)
						{
							const auto value = v[k] * input.scale;
							buf.write<float>(value);
							mn[k] = std::min(mn[k], value);
							mx[k] = std::max(mx[k], value);
						}
						buf.write<std::uint32_t>(INT24_W_BASE | static_cast<std::uint32_t>(source));
					}
					align16();

					write_rel_array(planes_field, convex.faces.size());
					for (const auto& face : convex.faces)
					{
						for (auto k = 0; k < 3; k++)
						{
							buf.write<float>(face.plane[k]);
						}
						buf.write<float>(-face.plane[3] * input.scale);
					}
					align16();

					write_rel_array(faces_field, convex.faces.size());
					auto first_index = 0u;
					for (auto f = 0u; f < convex.faces.size(); f++)
					{
						const auto& face = convex.faces[f];
						buf.write<std::uint16_t>(static_cast<std::uint16_t>(first_index));
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(face.indices.size()));
						auto smallest = 45.0f;
						for (auto e = 0u; e < face.indices.size(); e++)
						{
							const auto& twin = face_links[first_index + e];
							if (twin.face == f || twin.face >= convex.faces.size())
							{
								continue;
							}
							const auto& other = convex.faces[twin.face].plane;
							auto dot = face.plane[0] * other[0] + face.plane[1] * other[1] + face.plane[2] * other[2];
							dot = std::max(-1.0f, std::min(1.0f, dot));
							const auto dihedral = 180.0f - static_cast<float>(std::acos(dot) * 57.29577951308232);
							smallest = std::min(smallest, dihedral * 0.5f);
						}
						const auto half_radians = smallest * 0.017453292519943295f;
						auto quantised = static_cast<int>((half_radians - 1.1920929e-7f) * 41720.875f + 0.5f);
						quantised = std::max(0, std::min(65535, quantised)) >> 8;
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(quantised));
						first_index += static_cast<unsigned int>(face.indices.size());
					}
					align16();

					write_rel_array(indices_field, index_total);
					for (const auto& face : convex.faces)
					{
						for (const auto idx : face.indices)
						{
							buf.write<std::uint8_t>(idx);
						}
					}
					align16();

					// properties (mass) come right after the shape in stock, then connectivity
					write_mass_properties(properties_slot, cm);

					for (auto j = convex.verts.size(); j < vertex_count; j++)
					{
						vertex_edges[j] = vertex_edges[convex.verts.size() - 1];
					}
					const auto connectivity_offset = buf.size();
					global_fixups.emplace_back(connectivity_slot, connectivity_offset);
					virtual_fixups.emplace_back(connectivity_offset, CLASS_CONNECTIVITY);
					buf.reserve(16);
					const auto vertex_edges_field = buf.size();
					write_hk_array_header(buf, static_cast<int>(vertex_edges.size()));
					const auto face_links_field = buf.size();
					write_hk_array_header(buf, static_cast<int>(face_links.size()));
					const auto write_edges = [&](const std::size_t field, const std::vector<half_edge>& edges)
					{
						if (edges.empty())
						{
							return;
						}
						local_fixups.emplace_back(field, buf.size());
						for (const auto& edge : edges)
						{
							buf.write<std::uint16_t>(edge.face);
							buf.write<std::uint8_t>(edge.edge);
							buf.write<std::uint8_t>(0);
						}
						align16();
					};
					write_edges(vertex_edges_field, vertex_edges);
					write_edges(face_links_field, face_links);
					return convex_offset;
				};

				std::size_t root_offset = 0;
				if (!compound)
				{
					float mn[3] = {FLT_MAX, FLT_MAX, FLT_MAX}, mx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
					root_offset = write_convex(input.convexes[0], body_compressed, 0, mn, mx);
					if (!root_offset)
					{
						return {};
					}
				}
				else
				{
					// --- hknpDynamicCompoundShape (208) --- as in build_ents_shape_list, with
					// a properties pointer for the body's mass.
					const auto compound_offset = buf.size();
					root_offset = compound_offset;
					virtual_fixups.emplace_back(compound_offset, CLASS_COMPOUND);
					buf.reserve(16);
					buf.write<std::uint16_t>(static_cast<std::uint16_t>(COMPOUND_SHAPE_FLAGS));
					auto key_bits = 0;
					for (auto n = instance_count; n > 0; n >>= 1)
					{
						key_bits++;
					}
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(key_bits));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(COMPOUND_DISPATCH_TYPE));
					buf.write<float>(0.0f);
					buf.write<std::uint64_t>(0);
					const auto compound_properties_slot = buf.reserve(8);
					buf.reserve(8);
					buf.write<std::uint32_t>(0xFFFFFFFFu);
					buf.write<std::uint32_t>(0);
					write_hk_array_header(buf, 0);
					write_hk_array_header(buf, 0);
					buf.write<std::uint32_t>(0xFFFFFFFFu); // shapeTagCodecInfo
					buf.reserve(4);
					const auto instances_field = buf.size();
					write_hk_array_header(buf, instance_count);
					buf.write<std::int32_t>(-1);
					buf.reserve(4);
					buf.reserve(8);
					const auto aabb_slot = buf.reserve(32);
					buf.write<std::uint8_t>(1); // isMutable
					buf.reserve(7);
					buf.reserve(16);
					buf.reserve(8);
					const auto bvd_slot = buf.reserve(8);
					buf.reserve(8);
					if (buf.size() - compound_offset != SIZEOF_DYNAMIC_COMPOUND_SHAPE)
					{
						ZONETOOL_ERROR("havok: hknpDynamicCompoundShape is %zu bytes, expected %d",
							buf.size() - compound_offset, SIZEOF_DYNAMIC_COMPOUND_SHAPE);
						return {};
					}

					local_fixups.emplace_back(instances_field, buf.size());
					std::vector<std::size_t> instance_shape_slots;
					std::vector<std::size_t> instance_offsets;
					for (auto i = 0; i < instance_count; i++)
					{
						instance_offsets.emplace_back(buf.size());
						buf.write<float>(1.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<std::uint32_t>(SHAPE_INSTANCE_FLAGS_W);
						buf.write<float>(0.0f); buf.write<float>(1.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(1.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<std::uint32_t>(INT24_W_BASE);
						for (auto c = 0; c < 4; c++)
						{
							buf.write<float>(1.0f);
						}
						instance_shape_slots.emplace_back(buf.reserve(8));
						// 0xFFFF: a physics asset has no tag table, and a real tag would resolve against the map's.
						buf.write<std::uint16_t>(0xFFFF);
						buf.write<std::uint16_t>(0xFFFF); // destructionTag
						buf.reserve(36);
					}
					align16();

					// the compound's own mass properties sit right after its instances in stock
					write_mass_properties(compound_properties_slot, body_compressed);

					float shape_min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
					float shape_max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
					std::vector<std::array<float, 6>> convex_bounds;
					for (auto c = 0; c < instance_count; c++)
					{
						float mn[3] = {FLT_MAX, FLT_MAX, FLT_MAX}, mx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
						const auto convex_offset = write_convex(input.convexes[c],
							compress_mass(child_mass[c]), c, mn, mx);
						if (!convex_offset)
						{
							return {};
						}
						global_fixups.emplace_back(instance_shape_slots[c], convex_offset);
						convex_bounds.push_back({mn[0], mn[1], mn[2], mx[0], mx[1], mx[2]});
						for (auto k = 0; k < 3; k++)
						{
							shape_min[k] = std::min(shape_min[k], mn[k]);
							shape_max[k] = std::max(shape_max[k], mx[k]);
						}
					}
					for (auto k = 0; k < 3; k++)
					{
						buf.patch<float>(aabb_slot + k * 4, shape_min[k]);
						buf.patch<float>(aabb_slot + 16 + k * 4, shape_max[k]);
					}
					buf.patch<std::uint32_t>(aabb_slot + 12,
						INT24_W_BASE | static_cast<std::uint32_t>(instance_count - 1));
					buf.patch<float>(aabb_slot + 28, 0.0f);

					// --- hknpDynamicCompoundShapeData + tree --- (mirrors the ents writer)
					struct tree_node
					{
						float mn[3] = {0.0f, 0.0f, 0.0f};
						float mx[3] = {0.0f, 0.0f, 0.0f};
						std::uint16_t parent = 0;
						std::uint32_t data = 0;
					};
					const auto bvd_offset = buf.size();
					global_fixups.emplace_back(bvd_slot, bvd_offset);
					virtual_fixups.emplace_back(bvd_offset, CLASS_COMPOUND_DATA);
					buf.reserve(16);
					const auto nodes_field = buf.size();
					const auto node_count = 2 * instance_count + 1;
					write_hk_array_header(buf, node_count);
					buf.write<std::int32_t>(2 * instance_count);
					buf.reserve(4);
					buf.write<std::int32_t>(instance_count);
					buf.reserve(4);
					buf.write<std::int32_t>(1);
					buf.reserve(4);
					if (buf.size() - bvd_offset != SIZEOF_DYNAMIC_COMPOUND_SHAPE_DATA)
					{
						ZONETOOL_ERROR("havok: hknpDynamicCompoundShapeData is %zu bytes, expected %d",
							buf.size() - bvd_offset, SIZEOF_DYNAMIC_COMPOUND_SHAPE_DATA);
						return {};
					}
					align16();

					std::vector<tree_node> nodes(node_count);
					auto next_node = 1;
					std::vector<int> order(instance_count);
					for (auto i = 0; i < instance_count; i++)
					{
						order[i] = i;
					}
					std::function<int(int, int, std::uint16_t)> build_tree =
						[&](const int first, const int count, const std::uint16_t parent) -> int
					{
						const auto self = next_node++;
						auto& node = nodes[self];
						node.parent = parent;
						for (auto k = 0; k < 3; k++)
						{
							node.mn[k] = FLT_MAX;
							node.mx[k] = -FLT_MAX;
						}
						for (auto i = 0; i < count; i++)
						{
							const auto& b = convex_bounds[order[first + i]];
							for (auto k = 0; k < 3; k++)
							{
								node.mn[k] = std::min(node.mn[k], b[k]);
								node.mx[k] = std::max(node.mx[k], b[k + 3]);
							}
						}
						if (count == 1)
						{
							node.data = static_cast<std::uint32_t>(order[first]) << 16;
							return self;
						}
						auto axis = 0;
						auto widest = node.mx[0] - node.mn[0];
						for (auto k = 1; k < 3; k++)
						{
							if (node.mx[k] - node.mn[k] > widest)
							{
								widest = node.mx[k] - node.mn[k];
								axis = k;
							}
						}
						const auto centre = [&](const int idx)
						{
							const auto& b = convex_bounds[idx];
							return (b[axis] + b[axis + 3]) * 0.5f;
						};
						std::sort(order.begin() + first, order.begin() + first + count,
							[&](const int a, const int b) { return centre(a) < centre(b); });
						const auto half = count / 2;
						const auto left = build_tree(first, half, static_cast<std::uint16_t>(self));
						const auto right = build_tree(first + half, count - half, static_cast<std::uint16_t>(self));
						nodes[self].data = static_cast<std::uint32_t>(left) | (static_cast<std::uint32_t>(right) << 16);
						return self;
					};
					build_tree(0, instance_count, 0);

					for (auto n = 1; n < node_count - 1; n++)
					{
						const auto& node = nodes[n];
						if ((node.data & 0xFFFF) != 0)
						{
							continue;
						}
						const auto instance = static_cast<int>(node.data >> 16);
						if (instance < instance_count)
						{
							buf.patch<std::uint32_t>(instance_offsets[instance] + 48 + 12,
								INT24_W_BASE | static_cast<std::uint32_t>(n));
						}
					}

					local_fixups.emplace_back(nodes_field, buf.size());
					for (auto n = 0; n < node_count; n++)
					{
						const auto& node = nodes[n];
						const auto used = (n != 0 && n != node_count - 1);
						for (auto k = 0; k < 3; k++)
						{
							buf.write<float>(used ? node.mn[k] : 0.0f);
						}
						buf.write<std::uint16_t>(used ? node.parent : 0);
						buf.write<std::uint16_t>(used ? 0x3F00 : 0);
						for (auto k = 0; k < 3; k++)
						{
							buf.write<float>(used ? node.mx[k] : 0.0f);
						}
						buf.write<std::uint32_t>(used ? node.data : 0);
					}
					align16();
				}

				global_fixups.emplace_back(body_shape_slot, root_offset);
				global_fixups.emplace_back(referenced_slot, root_offset);
				const auto data_size = buf.size();

				byte_buffer names;
				const auto write_name = [&](const std::uint32_t sig, const char* name)
				{
					names.write<std::uint32_t>(sig);
					names.write<std::uint8_t>(0x09);
					const auto offset = names.size();
					names.write(name, std::strlen(name) + 1);
					return offset;
				};
				write_name(SIG_HK_CLASS, "hkClass");
				write_name(SIG_HK_CLASS_MEMBER, "hkClassMember");
				write_name(SIG_HK_CLASS_ENUM, "hkClassEnum");
				write_name(SIG_HK_CLASS_ENUM_ITEM, "hkClassEnumItem");
				std::array<std::size_t, CLASS_COUNT> name_offsets{};
				name_offsets[CLASS_ASSET] = write_name(SIG_PHYSICS_ASSET, "HavokPhysicsAsset");
				name_offsets[CLASS_SYSTEM_DATA] = write_name(SIG_PHYSICS_SYSTEM_DATA, "hknpPhysicsSystemData");
				if (compound)
				{
					name_offsets[CLASS_COMPOUND] = write_name(SIG_DYNAMIC_COMPOUND_SHAPE, "hknpDynamicCompoundShape");
				}
				name_offsets[CLASS_PROPERTIES] = write_name(SIG_REF_COUNTED_PROPERTIES, "hkRefCountedProperties");
				name_offsets[CLASS_MASS] = write_name(SIG_SHAPE_MASS_PROPERTIES, "hknpShapeMassProperties");
				name_offsets[CLASS_CONVEX] = write_name(SIG_CONVEX_POLYTOPE_SHAPE, "hknpConvexPolytopeShape");
				name_offsets[CLASS_CONNECTIVITY] = write_name(SIG_CONVEX_POLYTOPE_CONNECTIVITY, "hknpConvexPolytopeShapeConnectivity");
				if (compound)
				{
					name_offsets[CLASS_COMPOUND_DATA] = write_name(SIG_DYNAMIC_COMPOUND_SHAPE_DATA, "hknpDynamicCompoundShapeData");
				}
				names.align(16, 0xFF);

				std::sort(local_fixups.begin(), local_fixups.end(),
					[](const std::pair<std::size_t, std::size_t>& a, const std::pair<std::size_t, std::size_t>& b)
					{
						return a.second < b.second;
					});
				byte_buffer fixups;
				for (const auto& fixup : local_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				fixups.align(16, 0xFF);
				const auto local_size = fixups.size();
				std::sort(global_fixups.begin(), global_fixups.end());
				for (const auto& fixup : global_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(2);
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				fixups.align(16, 0xFF);
				const auto global_size = fixups.size() - local_size;
				std::sort(virtual_fixups.begin(), virtual_fixups.end());
				for (const auto& fixup : virtual_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(0);
					fixups.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[fixup.second]));
				}
				fixups.align(16, 0xFF);
				const auto virtual_size = fixups.size() - local_size - global_size;

				byte_buffer file;
				file.write<std::uint32_t>(HK_MAGIC0);
				file.write<std::uint32_t>(HK_MAGIC1);
				file.write<std::int32_t>(0);
				file.write<std::int32_t>(HK_FILE_VERSION);
				file.write<std::uint8_t>(8);
				file.write<std::uint8_t>(1);
				file.write<std::uint8_t>(0);
				file.write<std::uint8_t>(1);
				file.write<std::int32_t>(3);
				file.write<std::int32_t>(2);
				file.write<std::int32_t>(0);
				file.write<std::int32_t>(0);
				file.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[CLASS_ASSET]));
				const char* version = "hk_2014.2.5-r1";
				const auto version_length = std::strlen(version) + 1;
				file.write(version, version_length);
				file.fill(16 - version_length, 0xFF);
				file.write<std::int32_t>(0);
				file.write<std::uint16_t>(HK_MAX_PREDICATE);
				file.write<std::uint16_t>(0);

				const auto names_start = HK_HEADER_SIZE + 3 * HK_SECTION_HEADER_SIZE;
				const auto data_start = names_start + names.size();
				const auto write_section_header = [&](const char* tag, const std::size_t abs,
					const std::size_t payload, const std::size_t local, const std::size_t global,
					const std::size_t virt)
				{
					char name[19] = {};
					std::strncpy(name, tag, sizeof(name));
					file.write(name, sizeof(name));
					file.write<std::uint8_t>(0xFF);
					file.write<std::int32_t>(static_cast<std::int32_t>(abs));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local + global));
					const auto end = payload + local + global + virt;
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.fill(16, 0xFF);
				};
				write_section_header("__classnames__", names_start, names.size(), 0, 0, 0);
				write_section_header("__types__", data_start, 0, 0, 0, 0);
				write_section_header("__data__", data_start, data_size, local_size, global_size, virtual_size);
				file.write(names.data.data(), names.size());
				file.write(buf.data.data(), buf.size());
				file.write(fixups.data.data(), fixups.size());

				ZONETOOL_INFO("havok: dynamic physics asset \"%s\" built -- %d convex(es), mass %.2f, "
					"volume %.4f, %zu bytes", input.body_name.c_str(), instance_count,
					body_mass.mass, body_mass.volume, file.size());
				return file.data;
			}
		}
	}
}
