#include <std_include.hpp>
#include "s2.hpp"

#include <formats/cast/cast_scene.hpp>

#include <utils/cryptography.hpp>
#include <utils/string.hpp>

namespace zonetool::s2::cast_xmodel
{
	namespace
	{
		namespace cs = zonetool::cast;

		using triangle = std::array<std::uint16_t, 3>;

		constexpr std::size_t max_influences = std::extent_v<decltype(XSurfaceSkinVertex::bones)>;
		constexpr std::size_t max_lods = std::extent_v<decltype(XModelBase::lodInfo)>;
		constexpr std::size_t max_bones = std::extent_v<decltype(XSurface::partBits)> * 32;

		constexpr std::uint32_t bone_offset_stride = 64;
		constexpr std::uint16_t shared_streams_rigid = 0x7E0;
		constexpr std::uint16_t shared_streams_skinned = 0x1000;

		constexpr float no_cull_lod_dist = 1000000.0f;
		constexpr float lod_dist_divisor = 565.087f;

		float vec3_normalize(cs::vec3_t& value)
		{
			const auto length = std::sqrt(value.v[0] * value.v[0] + value.v[1] * value.v[1] + value.v[2] * value.v[2]);
			if (length <= 0.000001f)
			{
				return 0.0f;
			}

			const auto inverse = 1.0f / length;
			value.v[0] *= inverse;
			value.v[1] *= inverse;
			value.v[2] *= inverse;
			return length;
		}

		cs::vec3_t vec3_cross(const cs::vec3_t& a, const cs::vec3_t& b)
		{
			cs::vec3_t out{};
			out.v[0] = a.v[1] * b.v[2] - a.v[2] * b.v[1];
			out.v[1] = a.v[2] * b.v[0] - a.v[0] * b.v[2];
			out.v[2] = a.v[0] * b.v[1] - a.v[1] * b.v[0];
			return out;
		}

		float vec3_dot(const cs::vec3_t& a, const cs::vec3_t& b)
		{
			return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
		}

		cs::vec3_t vec3_sub(const cs::vec3_t& a, const cs::vec3_t& b)
		{
			return cs::vec3_t{{a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2]}};
		}

		void vec3_add_scaled(cs::vec3_t& target, const cs::vec3_t& value, const float scale)
		{
			target.v[0] += value.v[0] * scale;
			target.v[1] += value.v[1] * scale;
			target.v[2] += value.v[2] * scale;
		}

		cs::vec3_t perpendicular(const cs::vec3_t& normal)
		{
			auto axis = 0;
			if (std::abs(normal.v[1]) < std::abs(normal.v[axis])) axis = 1;
			if (std::abs(normal.v[2]) < std::abs(normal.v[axis])) axis = 2;

			cs::vec3_t reference{};
			reference.v[axis] = 1.0f;

			auto result = vec3_cross(normal, reference);
			if (vec3_normalize(result) == 0.0f)
			{
				result = cs::vec3_t{{1.0f, 0.0f, 0.0f}};
			}
			return result;
		}

		float angle_between(const cs::vec3_t& a, const cs::vec3_t& b)
		{
			return std::acos(std::clamp(vec3_dot(a, b), -1.0f, 1.0f));
		}

		void compute_normals(const cs::mesh_t& mesh, std::vector<cs::vec3_t>& normals)
		{
			normals.assign(mesh.vertex_count(), cs::vec3_t{});

			for (std::size_t tri = 0; tri < mesh.triangle_count(); tri++)
			{
				const auto i0 = mesh.faces[tri * 3 + 0];
				const auto i1 = mesh.faces[tri * 3 + 1];
				const auto i2 = mesh.faces[tri * 3 + 2];

				const auto edge0 = vec3_sub(mesh.positions[i1], mesh.positions[i0]);
				const auto edge1 = vec3_sub(mesh.positions[i2], mesh.positions[i0]);

				const auto face_normal = vec3_cross(edge0, edge1);
				vec3_add_scaled(normals[i0], face_normal, 1.0f);
				vec3_add_scaled(normals[i1], face_normal, 1.0f);
				vec3_add_scaled(normals[i2], face_normal, 1.0f);
			}

			for (auto& normal : normals)
			{
				if (vec3_normalize(normal) == 0.0f)
				{
					normal = cs::vec3_t{{0.0f, 0.0f, 1.0f}};
				}
			}
		}

		void compute_tangent_space(const cs::mesh_t& mesh, const std::vector<cs::vec3_t>& normals,
			std::vector<cs::vec3_t>& tangents, std::vector<float>& binormal_signs)
		{
			const auto vertex_count = mesh.vertex_count();
			const auto has_uvs = mesh.uvs.size() == vertex_count;

			std::vector<cs::vec3_t> accumulated_tangents(vertex_count, cs::vec3_t{});
			std::vector<cs::vec3_t> accumulated_bitangents(vertex_count, cs::vec3_t{});

			for (std::size_t tri = 0; has_uvs && tri < mesh.triangle_count(); tri++)
			{
				const std::uint32_t index[3]{mesh.faces[tri * 3 + 0], mesh.faces[tri * 3 + 1], mesh.faces[tri * 3 + 2]};

				const auto du1 = mesh.uvs[index[1]].v[0] - mesh.uvs[index[0]].v[0];
				const auto du2 = mesh.uvs[index[2]].v[0] - mesh.uvs[index[0]].v[0];
				const auto dv1 = mesh.uvs[index[1]].v[1] - mesh.uvs[index[0]].v[1];
				const auto dv2 = mesh.uvs[index[2]].v[1] - mesh.uvs[index[0]].v[1];

				const auto edge1 = vec3_sub(mesh.positions[index[1]], mesh.positions[index[0]]);
				const auto edge2 = vec3_sub(mesh.positions[index[2]], mesh.positions[index[0]]);

				const auto flip = dv2 * du1 < dv1 * du2 ? -1.0f : 1.0f;
				cs::vec3_t tangent{};
				cs::vec3_t bitangent{};
				for (auto axis = 0; axis < 3; axis++)
				{
					tangent.v[axis] = (edge1.v[axis] * dv2 - edge2.v[axis] * dv1) * flip;
					bitangent.v[axis] = (edge2.v[axis] * du1 - edge1.v[axis] * du2) * flip;
				}

				vec3_normalize(tangent);
				vec3_normalize(bitangent);

				auto edge10 = vec3_sub(mesh.positions[index[0]], mesh.positions[index[1]]);
				auto edge21 = vec3_sub(mesh.positions[index[1]], mesh.positions[index[2]]);
				auto edge02 = vec3_sub(mesh.positions[index[2]], mesh.positions[index[0]]);
				vec3_normalize(edge10);
				vec3_normalize(edge21);
				vec3_normalize(edge02);

				const float weight[3]{angle_between(edge10, edge02), angle_between(edge21, edge10), angle_between(edge02, edge21)};
				for (auto corner = 0; corner < 3; corner++)
				{
					vec3_add_scaled(accumulated_tangents[index[corner]], tangent, weight[corner]);
					vec3_add_scaled(accumulated_bitangents[index[corner]], bitangent, weight[corner]);
				}
			}

			tangents.assign(vertex_count, cs::vec3_t{});
			binormal_signs.assign(vertex_count, 1.0f);

			const auto has_source_tangents = mesh.tangents.size() == vertex_count;
			for (std::size_t i = 0; i < vertex_count; i++)
			{
				const auto& normal = normals[i];
				auto tangent = has_source_tangents ? mesh.tangents[i] : accumulated_tangents[i];

				vec3_add_scaled(tangent, normal, -vec3_dot(normal, tangent));
				if (vec3_normalize(tangent) < 0.001f)
				{
					tangent = vec3_cross(accumulated_bitangents[i], normal);
					if (vec3_normalize(tangent) < 0.001f)
					{
						tangent = perpendicular(normal);
					}
				}

				tangents[i] = tangent;
				binormal_signs[i] = vec3_dot(accumulated_bitangents[i], vec3_cross(normal, tangent)) >= 0.0f ? 1.0f : -1.0f;
			}
		}

		void prepare_influences(const cs::mesh_t& mesh, const std::size_t bone_count, const std::string& model_name,
			std::vector<std::vector<cs::influence_t>>& out, const bool warn_missing)
		{
			const auto vertex_count = mesh.vertex_count();
			out.assign(vertex_count, {});

			auto reported_missing = false;
			auto reported_trim = false;

			for (std::size_t vertex = 0; vertex < vertex_count; vertex++)
			{
				auto influences = vertex < mesh.influences.size() ? mesh.influences[vertex] : std::vector<cs::influence_t>{};
				for (const auto& influence : influences)
				{
					if (influence.bone >= bone_count)
					{
						throw cs::cast_error(utils::string::va("CAST XModel \"%s\": mesh \"%s\" vertex %zu references bone %u, the skeleton has %zu bones",
							model_name.data(), mesh.name.data(), vertex, influence.bone, bone_count));
					}
				}

				std::stable_sort(influences.begin(), influences.end(), [](const cs::influence_t& a, const cs::influence_t& b)
				{
					return a.weight > b.weight;
				});

				if (influences.size() > max_influences)
				{
					if (!reported_trim)
					{
						ZONETOOL_WARNING("CAST XModel \"%s\": mesh \"%s\" vertex %zu has %zu weights, only the %zu strongest are kept",
							model_name.data(), mesh.name.data(), vertex, influences.size(), max_influences);
						reported_trim = true;
					}
					influences.resize(max_influences);
				}

				auto total = 0.0f;
				for (const auto& influence : influences)
				{
					total += influence.weight;
				}

				if (influences.empty() || total <= 0.0f)
				{
					if (warn_missing && !reported_missing)
					{
						ZONETOOL_WARNING("CAST XModel \"%s\": mesh \"%s\" has vertices without weights (first is vertex %zu), they bind to bone 0",
							model_name.data(), mesh.name.data(), vertex);
						reported_missing = true;
					}

					influences = {cs::influence_t{0, 1.0f}};
					total = 1.0f;
				}

				for (auto& influence : influences)
				{
					influence.weight /= total;
				}

				out[vertex] = std::move(influences);
			}
		}

		void add_bone_to_part_bits(unsigned int* part_bits, const std::uint32_t bone)
		{
			part_bits[bone / 32] |= 1u << (31 - (bone % 32));
		}

		class stream_writer
		{
		public:
			template <typename T>
			std::size_t take(const std::size_t count)
			{
				this->data_.resize((this->data_.size() + 7) & ~7ull);
				const auto start = this->data_.size();
				this->data_.resize(start + count * sizeof(T));
				return start;
			}

			template <typename T>
			T* at(const std::size_t offset)
			{
				return reinterpret_cast<T*>(this->data_.data() + offset);
			}

			std::string& data()
			{
				return this->data_;
			}

		private:
			std::string data_;
		};

		std::uint32_t pack_sky_visibility(const std::uint32_t packed_normal)
		{
			float n[3]{};
			for (auto i = 0; i < 3; i++)
			{
				n[i] = static_cast<float>((packed_normal >> (10 * i)) & 1023) / 1023.0f * 2.0f - 1.0f;
			}

			const auto sum = std::abs(n[0]) + std::abs(n[1]) + std::abs(n[2]);
			auto x = sum > 0.0f ? n[0] / sum : 0.0f;
			auto y = sum > 0.0f ? n[1] / sum : 0.0f;
			if (n[2] < 0.0f)
			{
				const auto folded_x = (1.0f - std::abs(y)) * (x >= 0.0f ? 1.0f : -1.0f);
				const auto folded_y = (1.0f - std::abs(x)) * (y >= 0.0f ? 1.0f : -1.0f);
				x = folded_x;
				y = folded_y;
			}

			const auto quantize = [](const float value)
			{
				return static_cast<std::uint32_t>(std::lround(std::clamp(value * 0.5f + 0.5f, 0.0f, 1.0f) * 511.0f));
			};

			return quantize(x) | (quantize(y) << 9) | (127u << 18);
		}

		std::uint32_t pack_tangent(cs::vec3_t tangent, const float binormal_sign)
		{
			const auto packed = PackedVec::Vec3PackUnitVec_H1(tangent.v);
			return (packed & ~0xC0000000u) | (binormal_sign < 0.0f ? 0u : 0xC0000000u);
		}

		class surface_builder
		{
		public:
			surface_builder(const cs::mesh_t& mesh, const std::size_t bone_count, std::string model_name, const bool has_skeleton)
				: mesh_(mesh)
				, bone_count_(bone_count)
				, model_name_(std::move(model_name))
				, has_skeleton_(has_skeleton)
			{
			}

			void build(XSurface& surface, zone_memory* mem)
			{
				const auto vertex_count = this->mesh_.vertex_count();
				if (vertex_count == 0 || this->mesh_.triangle_count() == 0)
				{
					throw cs::cast_error(utils::string::va("CAST XModel \"%s\": mesh \"%s\" is empty", this->model_name_.data(), this->mesh_.name.data()));
				}

				constexpr std::size_t max_vertices = std::numeric_limits<decltype(XSurface::vertCount)>::max();
				if (vertex_count > max_vertices)
				{
					throw cs::cast_error(utils::string::va("CAST XModel \"%s\": mesh \"%s\" has %zu vertices, a surface holds at most %zu",
						this->model_name_.data(), this->mesh_.name.data(), vertex_count, max_vertices));
				}

				this->padded_triangles_ = this->mesh_.triangle_count() + (this->mesh_.triangle_count() & 1);

				constexpr std::size_t max_triangles = std::numeric_limits<decltype(XSurface::triCount)>::max();
				if (this->padded_triangles_ > max_triangles)
				{
					throw cs::cast_error(utils::string::va("CAST XModel \"%s\": mesh \"%s\" has %zu triangles, a surface holds at most %zu",
						this->model_name_.data(), this->mesh_.name.data(), this->mesh_.triangle_count(), max_triangles));
				}

				this->prepare_normals();
				if (this->mesh_.uvs.size() != vertex_count)
				{
					ZONETOOL_WARNING("CAST XModel \"%s\": mesh \"%s\" has no uv layer, its texture coordinates are zero",
						this->model_name_.data(), this->mesh_.name.data());
				}

				compute_tangent_space(this->mesh_, this->normals_, this->tangents_, this->binormal_signs_);
				prepare_influences(this->mesh_, this->bone_count_, this->model_name_, this->influences_, this->has_skeleton_);

				this->sort_vertices();
				this->build_indices();

				const auto has_colors = this->mesh_.colors.size() == vertex_count
					&& std::ranges::any_of(this->mesh_.colors, [](const std::uint32_t color) { return color != 0xFFFFFFFF; });

				surface.flags = XSURFACE_FLAG_UNK_0x20
					| (has_colors ? 0 : XSURFACE_FLAG_VERTCOL_NONE)
					| (this->rigid_ ? 0 : XSURFACE_FLAG_SKINNED);
				surface.vertCount = static_cast<unsigned short>(vertex_count);
				surface.triCount = static_cast<unsigned short>(this->padded_triangles_);
				surface.uvSetCount = 1;

				std::vector<XSurfaceSkinVertex> skin;
				if (this->rigid_)
				{
					this->write_rigid_vert_lists(surface, mem);
				}
				else
				{
					skin = this->build_skin(surface);
				}

				this->write_indices(surface, mem);
				this->write_shared(surface, mem, has_colors, skin);
				this->write_verts(surface, mem);
			}

			[[nodiscard]] const std::vector<std::vector<cs::influence_t>>& influences() const
			{
				return this->influences_;
			}

		private:
			void prepare_normals()
			{
				if (this->mesh_.normals.size() != this->mesh_.vertex_count())
				{
					if (!this->mesh_.normals.empty())
					{
						ZONETOOL_WARNING("CAST XModel \"%s\": mesh \"%s\" has an incomplete normal buffer, the normals are recalculated",
							this->model_name_.data(), this->mesh_.name.data());
					}
					compute_normals(this->mesh_, this->normals_);
					return;
				}

				this->normals_ = this->mesh_.normals;
				for (auto& normal : this->normals_)
				{
					if (vec3_normalize(normal) == 0.0f)
					{
						normal = cs::vec3_t{{0.0f, 0.0f, 1.0f}};
					}
				}
			}

			[[nodiscard]] std::uint32_t packed_normal(const std::uint32_t source) const
			{
				auto normal = this->normals_[source];
				return PackedVec::Vec3PackUnitVec_H1(normal.v);
			}

			[[nodiscard]] int triangle_bone(const std::size_t index) const
			{
				const auto& tri = this->triangles_[index];
				const auto bone0 = this->influences_[this->order_[tri[0]]][0].bone;
				const auto bone1 = this->influences_[this->order_[tri[1]]][0].bone;
				const auto bone2 = this->influences_[this->order_[tri[2]]][0].bone;
				return (bone0 == bone1 && bone1 == bone2) ? static_cast<int>(bone0) : -1;
			}

			void sort_vertices()
			{
				const auto vertex_count = this->mesh_.vertex_count();
				const auto& influences = this->influences_;

				this->order_.resize(vertex_count);
				std::iota(this->order_.begin(), this->order_.end(), 0u);
				std::ranges::stable_sort(this->order_, [&influences](const std::uint32_t a, const std::uint32_t b)
				{
					const auto count_a = influences[a].size();
					const auto count_b = influences[b].size();
					if (count_a != count_b)
					{
						return count_a < count_b;
					}
					return count_a == 1 && influences[a][0].bone < influences[b][0].bone;
				});

				this->reverse_.resize(vertex_count);
				for (std::size_t i = 0; i < vertex_count; i++)
				{
					this->reverse_[this->order_[i]] = static_cast<std::uint32_t>(i);
				}

				this->rigid_ = std::ranges::all_of(influences, [](const auto& entry) { return entry.size() == 1; });
			}

			void build_indices()
			{
				const auto triangle_count = this->mesh_.triangle_count();

				this->triangles_.resize(triangle_count);
				for (std::size_t tri = 0; tri < triangle_count; tri++)
				{
					this->triangles_[tri][0] = static_cast<std::uint16_t>(this->reverse_[this->mesh_.faces[tri * 3 + 0]]);
					this->triangles_[tri][1] = static_cast<std::uint16_t>(this->reverse_[this->mesh_.faces[tri * 3 + 2]]);
					this->triangles_[tri][2] = static_cast<std::uint16_t>(this->reverse_[this->mesh_.faces[tri * 3 + 1]]);
				}

				if (!this->rigid_)
				{
					return;
				}

				std::vector<int> bones(triangle_count);
				for (std::size_t i = 0; i < triangle_count; i++)
				{
					bones[i] = this->triangle_bone(i);
				}

				std::vector<std::size_t> sorted(triangle_count);
				std::iota(sorted.begin(), sorted.end(), 0u);
				std::ranges::stable_sort(sorted, [&bones](const std::size_t a, const std::size_t b)
				{
					if ((bones[a] < 0) != (bones[b] < 0))
					{
						return bones[b] < 0;
					}
					return bones[a] < bones[b];
				});

				std::vector<triangle> reordered(triangle_count);
				for (std::size_t i = 0; i < triangle_count; i++)
				{
					reordered[i] = this->triangles_[sorted[i]];
				}
				this->triangles_ = std::move(reordered);
			}

			void write_rigid_vert_lists(XSurface& surface, zone_memory* mem)
			{
				const auto vertex_count = this->mesh_.vertex_count();
				const auto triangle_count = this->mesh_.triangle_count();

				std::vector<XRigidVertList> lists;
				std::size_t vertex_tail = 0;
				std::size_t triangle_tail = 0;

				for (std::uint32_t bone = 0; bone < this->bone_count_; bone++)
				{
					auto vertex_head = vertex_tail;
					while (vertex_head < vertex_count && this->influences_[this->order_[vertex_head]][0].bone == bone)
					{
						vertex_head++;
					}

					auto triangle_head = triangle_tail;
					while (triangle_head < triangle_count && this->triangle_bone(triangle_head) == static_cast<int>(bone))
					{
						triangle_head++;
					}

					if (vertex_head == vertex_tail && triangle_head == triangle_tail)
					{
						continue;
					}

					XRigidVertList list{};
					list.boneOffset = static_cast<unsigned short>(bone * bone_offset_stride);
					list.vertCount = static_cast<unsigned short>(vertex_head - vertex_tail);
					list.triOffset = static_cast<unsigned short>(triangle_tail);
					list.triCount = static_cast<unsigned short>(triangle_head - triangle_tail);
					list.collisionTree = nullptr; // TODO: build the tree that model marks trace against
					lists.emplace_back(list);

					vertex_tail = vertex_head;
					triangle_tail = triangle_head;
					add_bone_to_part_bits(surface.partBits, bone);
				}

				constexpr std::size_t max_lists = std::numeric_limits<decltype(XSurface::rigidVertListCount)>::max();
				if (lists.size() > max_lists)
				{
					throw cs::cast_error(utils::string::va("CAST XModel \"%s\": mesh \"%s\" binds rigidly to %zu bones, a surface supports at most %zu",
						this->model_name_.data(), this->mesh_.name.data(), lists.size(), max_lists));
				}

				surface.rigidVertListCount = static_cast<unsigned char>(lists.size());
				surface.rigidVertLists = mem->allocate<XRigidVertList>(lists.size());
				std::ranges::copy(lists, surface.rigidVertLists);
			}

			std::vector<XSurfaceSkinVertex> build_skin(XSurface& surface)
			{
				std::vector<XSurfaceSkinVertex> skin(this->mesh_.vertex_count());
				for (std::size_t i = 0; i < skin.size(); i++)
				{
					const auto& influences = this->influences_[this->order_[i]];
					auto& vertex = skin[i];
					vertex.count = static_cast<unsigned short>(influences.size());

					for (std::size_t influence = 0; influence < influences.size(); influence++)
					{
						vertex.bones[influence] = static_cast<unsigned short>(influences[influence].bone);
						if (influence > 0)
						{
							vertex.weights[influence - 1] = static_cast<unsigned short>(std::clamp(influences[influence].weight, 0.0f, 1.0f) * 65535.0f + 0.5f);
						}
						add_bone_to_part_bits(surface.partBits, influences[influence].bone);
					}
				}

				return skin;
			}

			void write_indices(XSurface& surface, zone_memory* mem)
			{
				auto padded = this->triangles_;
				if (this->padded_triangles_ > padded.size())
				{
					const auto last = padded.back()[2];
					padded.emplace_back(triangle{last, last, last});
				}

				surface.triIndices = mem->allocate<unsigned short>(padded.size() * 3);
				std::memcpy(surface.triIndices, padded.data(), padded.size() * sizeof(triangle));
				this->triangles_ = std::move(padded);
			}

			void write_shared(XSurface& surface, zone_memory* mem, const bool has_colors, const std::vector<XSurfaceSkinVertex>& skin)
			{
				const auto vertex_count = this->mesh_.vertex_count();
				const auto has_uvs = this->mesh_.uvs.size() == vertex_count;

				stream_writer writer;
				const auto faces_offset = writer.take<triangle>(this->triangles_.size());
				const auto positions_offset = writer.take<float>(vertex_count * 3);
				const auto sky_visibility_offset = writer.take<std::uint32_t>(vertex_count);
				const auto colors_offset = writer.take<std::uint32_t>(vertex_count);
				const auto tex_coords_offset = writer.take<std::uint32_t>(vertex_count);
				const auto normals_offset = writer.take<std::uint32_t>(vertex_count);
				const auto tangents_offset = writer.take<std::uint32_t>(vertex_count);
				const auto skin_offset = skin.empty() ? 0 : writer.take<XSurfaceSkinVertex>(vertex_count);

				std::memcpy(writer.at<triangle>(faces_offset), this->triangles_.data(), this->triangles_.size() * sizeof(triangle));
				for (std::size_t i = 0; i < vertex_count; i++)
				{
					const auto source = this->order_[i];
					std::memcpy(writer.at<float>(positions_offset) + i * 3, this->mesh_.positions[source].v, sizeof(float) * 3);

					const auto packed_normal = this->packed_normal(source);
					writer.at<std::uint32_t>(normals_offset)[i] = packed_normal;
					writer.at<std::uint32_t>(sky_visibility_offset)[i] = pack_sky_visibility(packed_normal);
					writer.at<std::uint32_t>(tangents_offset)[i] = pack_tangent(this->tangents_[source], this->binormal_signs_[source]);
					writer.at<std::uint32_t>(colors_offset)[i] = has_colors ? this->mesh_.colors[source] : 0xFFFFFFFF;

					float uv[4]{};
					if (has_uvs)
					{
						uv[0] = this->mesh_.uvs[source].v[0];
						uv[1] = this->mesh_.uvs[source].v[1];
					}
					writer.at<std::uint32_t>(tex_coords_offset)[i] = PackedVec::Vec2PackTexCoords(uv);
				}

				if (!skin.empty())
				{
					std::memcpy(writer.at<XSurfaceSkinVertex>(skin_offset), skin.data(), skin.size() * sizeof(XSurfaceSkinVertex));
				}

				const auto& data = writer.data();
				auto* shared = mem->allocate<XSurfaceShared>();
				const auto streams = static_cast<std::uint16_t>(shared_streams_rigid | (skin.empty() ? 0 : shared_streams_skinned));
				shared->flags = static_cast<std::uint8_t>(streams);
				shared->__pad0 = static_cast<std::uint8_t>(streams >> 8);
				const auto hash = utils::cryptography::sha256::compute(data, true);
				std::memcpy(shared->hash, hash.data(), sizeof(shared->hash));
				shared->indexCount = static_cast<unsigned int>(this->triangles_.size() * 3);
				shared->vertCount = static_cast<unsigned short>(vertex_count);
				shared->dataSize = static_cast<unsigned int>(data.size());
				shared->data = mem->allocate<char>(data.size());
				std::memcpy(shared->data, data.data(), data.size());

				surface.shared = shared;
			}

			void write_verts(XSurface& surface, zone_memory* mem)
			{
				surface.verts = mem->allocate<vec4_t>(this->mesh_.vertex_count());
				for (std::size_t i = 0; i < this->mesh_.vertex_count(); i++)
				{
					const auto source = this->order_[i];
					const auto packed_normal = this->packed_normal(source);

					std::memcpy(surface.verts[i], this->mesh_.positions[source].v, sizeof(float) * 3);
					std::memcpy(&surface.verts[i][3], &packed_normal, sizeof(packed_normal));
				}
			}

			const cs::mesh_t& mesh_;
			std::size_t bone_count_;
			std::string model_name_;
			bool has_skeleton_;

			std::vector<cs::vec3_t> normals_;
			std::vector<cs::vec3_t> tangents_;
			std::vector<float> binormal_signs_;
			std::vector<std::vector<cs::influence_t>> influences_;

			std::vector<std::uint32_t> order_;
			std::vector<std::uint32_t> reverse_;
			std::vector<triangle> triangles_;
			std::size_t padded_triangles_ = 0;
			bool rigid_ = true;
		};

		DObjAnimMat base_pose(const cs::bone_t& bone)
		{
			DObjAnimMat mat{};
			std::memcpy(mat.quat, bone.world_rotation.v, sizeof(mat.quat));
			std::memcpy(mat.trans, bone.world_position.v, sizeof(mat.trans));

			const auto norm_squared = cs::quat_dot(bone.world_rotation, bone.world_rotation);
			if (std::abs(norm_squared) < std::numeric_limits<float>::epsilon())
			{
				mat.quat[0] = mat.quat[1] = mat.quat[2] = 0.0f;
				mat.quat[3] = 1.0f;
				mat.transWeight = 2.0f;
			}
			else
			{
				mat.transWeight = 2.0f / norm_squared;
			}

			return mat;
		}

		void set_bounds(float* mid_point, float* half_size, const float* mins, const float* maxs)
		{
			for (auto axis = 0; axis < 3; axis++)
			{
				half_size[axis] = (maxs[axis] - mins[axis]) * 0.5f;
				mid_point[axis] = mins[axis] + half_size[axis];
			}
		}

		float length_squared(const float* value)
		{
			return value[0] * value[0] + value[1] * value[1] + value[2] * value[2];
		}

		void calculate_bone_bounds(XBoneInfo* bone_info, const std::size_t bone_count, const std::vector<cs::mesh_t>& meshes,
			const std::vector<std::vector<std::vector<cs::influence_t>>>& influences)
		{
			for (std::uint32_t bone = 0; bone < bone_count; bone++)
			{
				float mins[3]{};
				float maxs[3]{};

				for (std::size_t mesh_index = 0; mesh_index < meshes.size(); mesh_index++)
				{
					const auto& mesh = meshes[mesh_index];
					for (std::size_t vertex = 0; vertex < mesh.vertex_count(); vertex++)
					{
						const auto& weights = influences[mesh_index][vertex];
						if (std::ranges::none_of(weights, [bone](const cs::influence_t& influence) { return influence.bone == bone; }))
						{
							continue;
						}

						for (auto axis = 0; axis < 3; axis++)
						{
							mins[axis] = std::min(mins[axis], mesh.positions[vertex].v[axis]);
							maxs[axis] = std::max(maxs[axis], mesh.positions[vertex].v[axis]);
						}
					}
				}

				auto& info = bone_info[bone];
				set_bounds(info.bounds[0], info.bounds[1], mins, maxs);
				info.radiusSquared = length_squared(info.bounds[1]);
			}
		}

		void remap_mesh_influences(cs::model_t& lod_model, const std::vector<std::string>& source_bones,
			const std::vector<cs::bone_t>& bones, const std::string& model_name, const std::size_t lod)
		{
			if (source_bones.empty())
			{
				return;
			}

			std::unordered_map<std::string, std::uint32_t> by_name;
			for (std::size_t i = 0; i < bones.size(); i++)
			{
				by_name.emplace(bones[i].name, static_cast<std::uint32_t>(i));
			}

			std::vector<std::uint32_t> remap(source_bones.size());
			for (std::size_t i = 0; i < source_bones.size(); i++)
			{
				const auto itr = by_name.find(source_bones[i]);
				if (itr == by_name.end())
				{
					throw cs::cast_error(utils::string::va("CAST XModel \"%s\": lod %zu binds to bone \"%s\", which lod 0 does not have",
						model_name.data(), lod, source_bones[i].data()));
				}
				remap[i] = itr->second;
			}

			for (auto& mesh : lod_model.meshes)
			{
				for (auto& influences : mesh.influences)
				{
					for (auto& influence : influences)
					{
						if (influence.bone >= remap.size())
						{
							throw cs::cast_error(utils::string::va("CAST XModel \"%s\": mesh \"%s\" references bone %u, lod %zu has %zu bones",
								model_name.data(), mesh.name.data(), influence.bone, lod, remap.size()));
						}
						influence.bone = remap[influence.bone];
					}
				}
			}
		}

		std::string material_name(const cs::mesh_t& mesh, const std::string& model_name)
		{
			if (mesh.material.empty())
			{
				ZONETOOL_WARNING("CAST XModel \"%s\": mesh \"%s\" has no material, it uses \"$default\"", model_name.data(), mesh.name.data());
				return "$default";
			}

			return mesh.material;
		}

		float lod_dist_sq(const float dist, const float radius)
		{
			const auto view_dist = dist <= 0.0f || dist >= no_cull_lod_dist ? no_cull_lod_dist : dist;
			const auto scaled = std::max(0.0f, view_dist - radius) / lod_dist_divisor;
			return scaled * scaled;
		}

		struct build_cache
		{
			zone_memory* mem = nullptr;
			std::unordered_map<std::string, XModelBase*> bases;
			std::unordered_map<std::string, XModelSurfs*> surfs;
		};

		build_cache& get_cache()
		{
			static build_cache cache;
			return cache;
		}

		build_cache& cache_for(zone_memory* mem)
		{
			auto& cache = get_cache();
			if (cache.mem != mem)
			{
				cache.bases.clear();
				cache.surfs.clear();
				cache.mem = mem;
			}
			return cache;
		}

		template <typename T>
		T* find_cached(const std::unordered_map<std::string, T*>& entries, const std::string& name, zone_memory* mem)
		{
			if (get_cache().mem != mem)
			{
				return nullptr;
			}

			const auto itr = entries.find(name);
			return itr == entries.end() ? nullptr : itr->second;
		}

		std::unique_ptr<cs::file> load_lod_file(const std::string& name, const std::size_t lod, std::string& used_path)
		{
			std::vector<std::string> candidates;
			if (lod == 0)
			{
				candidates.emplace_back(dump_path(ASSET_TYPE_XMODEL, name.data(), ".cast"));
			}
			candidates.emplace_back(dump_path(ASSET_TYPE_XMODEL, utils::string::va("%s_lod%zu", name.data(), lod), ".cast"));
			candidates.emplace_back(dump_path(ASSET_TYPE_XMODEL, utils::string::va("%s_LOD%zu", name.data(), lod), ".cast"));

			for (const auto& candidate : candidates)
			{
				if (auto result = cs::load(candidate))
				{
					used_path = candidate;
					return result;
				}
			}

			return nullptr;
		}

		std::vector<cs::model_t> load_lod_models(const std::string& name)
		{
			std::string base_path;
			const auto base = load_lod_file(name, 0, base_path);
			if (!base)
			{
				return {};
			}

			auto scene = cs::build_scene(*base, base_path);
			if (scene.models.empty())
			{
				throw cs::cast_error(utils::string::va("CAST XModel \"%s\": the file has no model node", name.data()));
			}

			auto models = std::move(scene.models);
			const auto lods_from_files = models.size() == 1;
			for (auto lod = 1u; lods_from_files && lod < max_lods; lod++)
			{
				std::string path;
				const auto lod_file = load_lod_file(name, lod, path);
				if (!lod_file)
				{
					break;
				}

				auto lod_scene = cs::build_scene(*lod_file, path);
				if (lod_scene.models.empty())
				{
					ZONETOOL_WARNING("CAST XModel \"%s\": \"%s\" has no model node, lod %u is skipped", name.data(), path.data(), lod);
					break;
				}

				models.emplace_back(std::move(lod_scene.models[0]));
			}

			if (models.size() > max_lods)
			{
				ZONETOOL_WARNING("CAST XModel \"%s\": the file has %zu models, only the first %zu are lods", name.data(), models.size(), max_lods);
				models.resize(max_lods);
			}

			return models;
		}

		struct lod_build
		{
			std::vector<std::string> materials;
			std::vector<std::vector<std::vector<cs::influence_t>>> lod0_influences;
			float mins[3]{};
			float maxs[3]{};
			std::size_t vertex_count = 0;
			std::size_t triangle_count = 0;
		};

		void add_lod0_bounds(const cs::mesh_t& mesh, lod_build& state)
		{
			for (const auto& position : mesh.positions)
			{
				const auto first = state.vertex_count == 0 && &position == &mesh.positions.front();
				for (auto axis = 0; axis < 3; axis++)
				{
					state.mins[axis] = first ? position.v[axis] : std::min(state.mins[axis], position.v[axis]);
					state.maxs[axis] = first ? position.v[axis] : std::max(state.maxs[axis], position.v[axis]);
				}
			}
		}

		XModelSurfs* build_lod_surfs(const cs::model_t& lod_model, const std::size_t lod, const std::string& name,
			const std::size_t bone_count, const bool has_skeleton, lod_build& state, unsigned int& base_vert_index, zone_memory* mem)
		{
			if (lod_model.meshes.empty())
			{
				throw cs::cast_error(utils::string::va("CAST XModel \"%s\": lod %zu has no mesh", name.data(), lod));
			}

			auto* surfs = mem->allocate<XModelSurfs>();
			surfs->name = mem->duplicate_string(utils::string::va("%s_lod%zu", name.data(), lod));
			surfs->numsurfs = static_cast<unsigned short>(lod_model.meshes.size());
			surfs->surfs = mem->allocate<XSurface>(surfs->numsurfs);

			for (std::size_t i = 0; i < lod_model.meshes.size(); i++)
			{
				const auto& mesh = lod_model.meshes[i];
				auto& surface = surfs->surfs[i];

				surface_builder builder(mesh, bone_count, name, has_skeleton);
				builder.build(surface, mem);

				surface.baseVertIndex = base_vert_index;
				base_vert_index += (surface.vertCount + 1u) & ~1u;

				for (auto bits = 0u; bits < std::size(surfs->partBits); bits++)
				{
					surfs->partBits[bits] |= surface.partBits[bits];
				}

				state.materials.emplace_back(material_name(mesh, name));
				if (lod == 0)
				{
					add_lod0_bounds(mesh, state);
					state.vertex_count += mesh.vertex_count();
					state.triangle_count += mesh.triangle_count();
					state.lod0_influences.emplace_back(builder.influences());
				}
			}

			return surfs;
		}

		void build_skeleton(XModelBase* base, const std::vector<cs::bone_t>& bones, const std::string& name, zone_memory* mem)
		{
			base->numBones = static_cast<unsigned short>(bones.size());
			base->numRootBones = static_cast<unsigned short>(std::ranges::count_if(bones, [](const cs::bone_t& bone) { return bone.parent_index < 0; }));
			base->numClientBones = 0;

			if (base->numRootBones > 1)
			{
				std::string root_names;
				for (const auto& bone : bones)
				{
					if (bone.parent_index < 0)
					{
						root_names += (root_names.empty() ? "" : ", ") + bone.name;
					}
				}

				ZONETOOL_WARNING("CAST XModel \"%s\": the skeleton has %u root bones (%s). The model attaches by \"%s\", the other roots do not follow it.",
					name.data(), base->numRootBones, root_names.data(), bones[0].name.data());
			}

			const auto child_bones = bones.size() - base->numRootBones;
			auto* parent_list = mem->allocate<unsigned short>(child_bones);
			auto* tag_angles = mem->allocate<XModelAngle>(child_bones);
			auto* tag_positions = mem->allocate<XModelTagPos>(child_bones);
			auto* base_mat = mem->allocate<DObjAnimMat>(bones.size());

			base->boneNames = mem->allocate<scr_string_t>(bones.size());
			base->partClassification = mem->allocate<unsigned char>(bones.size());

			auto warned_about_scale = false;
			for (std::size_t i = 0; i < bones.size(); i++)
			{
				const auto& bone = bones[i];
				base->boneNames[i] = static_cast<scr_string_t>(SL_GetString(bone.name.data()));
				base_mat[i] = base_pose(bone);

				if (i >= base->numRootBones)
				{
					const auto child = i - base->numRootBones;
					parent_list[child] = static_cast<unsigned short>(i - bone.parent_index);
					tag_positions[child] = {bone.local_position.v[0], bone.local_position.v[1], bone.local_position.v[2]};
					tag_angles[child].x = QuatInt16::ToInt16(bone.local_rotation.v[0]);
					tag_angles[child].y = QuatInt16::ToInt16(bone.local_rotation.v[1]);
					tag_angles[child].z = QuatInt16::ToInt16(bone.local_rotation.v[2]);
					tag_angles[child].base = QuatInt16::ToInt16(bone.local_rotation.v[3]);
				}

				if (!warned_about_scale && (bone.scale.v[0] != 1.0f || bone.scale.v[1] != 1.0f || bone.scale.v[2] != 1.0f))
				{
					ZONETOOL_WARNING("CAST XModel \"%s\": bone \"%s\" has a bind pose scale, an XModel cannot store it", name.data(), bone.name.data());
					warned_about_scale = true;
				}
			}

			base->parentList = parent_list;
			base->tagAngles = reinterpret_cast<short*>(tag_angles);
			base->tagPositions = reinterpret_cast<float*>(tag_positions);
			base->baseMat = reinterpret_cast<char*>(base_mat);
		}

		std::vector<float> lod_distances(const std::vector<cs::model_t>& models, const float radius, const std::string& name)
		{
			std::vector<float> distances(models.size(), no_cull_lod_dist);
			const auto carries_distances = std::all_of(models.begin(), models.end() - 1, [](const cs::model_t& model) { return model.has_lod_distance; });

			auto derived = std::max(radius, 1.0f) * 8.0f;
			for (std::size_t lod = 0; lod + 1 < models.size(); lod++)
			{
				distances[lod] = carries_distances ? models[lod].lod_distance : derived;
				derived *= 2.0f;
			}

			if (!carries_distances)
			{
				ZONETOOL_WARNING("CAST XModel \"%s\": cast has no lod distances, they come from the model radius (%.1f)", name.data(), radius);
			}

			return distances;
		}

		float max_distance_from(const std::vector<cs::mesh_t>& meshes, const float* center)
		{
			auto radius_squared = 0.0f;
			for (const auto& mesh : meshes)
			{
				for (const auto& position : mesh.positions)
				{
					const float offset[3]{position.v[0] - center[0], position.v[1] - center[1], position.v[2] - center[2]};
					radius_squared = std::max(radius_squared, length_squared(offset));
				}
			}

			return std::sqrt(radius_squared);
		}
	}

	void reset()
	{
		auto& cache = get_cache();
		cache.bases.clear();
		cache.surfs.clear();
		cache.mem = nullptr;
	}

	XModelBase* find_base(const std::string& name, zone_memory* mem)
	{
		return find_cached(get_cache().bases, name, mem);
	}

	XModelSurfs* find_surfs(const std::string& name, zone_memory* mem)
	{
		return find_cached(get_cache().surfs, name, mem);
	}

	XModel* parse(const std::string& name, zone_memory* mem)
	{
		auto models = load_lod_models(name);
		if (models.empty())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing cast xmodel \"%s\"...", name.data());

		std::vector<std::vector<std::string>> source_bone_names(models.size());
		for (std::size_t lod = 0; lod < models.size(); lod++)
		{
			for (const auto& bone : models[lod].bones)
			{
				source_bone_names[lod].emplace_back(bone.name);
			}
		}

		auto& bones = models[0].bones;
		const auto has_skeleton = !bones.empty();
		if (!has_skeleton)
		{
			cs::bone_t root{};
			root.name = "tag_origin";
			root.has_local = true;
			root.has_world = true;
			bones.emplace_back(std::move(root));
		}

		cs::normalize_skeleton(bones, utils::string::va("XModel \"%s\"", name.data()));
		for (std::size_t lod = 0; has_skeleton && lod < models.size(); lod++)
		{
			remap_mesh_influences(models[lod], source_bone_names[lod], bones, name, lod);
		}

		if (bones.size() > max_bones)
		{
			throw cs::cast_error(utils::string::va("CAST XModel \"%s\": the skeleton has %zu bones, the game supports at most %zu",
				name.data(), bones.size(), max_bones));
		}

		auto& cache = cache_for(mem);

		auto* base = mem->allocate<XModelBase>();
		base->name = mem->duplicate_string(name + "_base");
		base->lodRampType = 0;
		base->collLod = -1;
		base->forcedLod = 0xFE;
		base->cullSizeScale = 15.0f;
		base->lodDistScale = 1.0f;
		build_skeleton(base, bones, name, mem);

		lod_build state{};
		auto base_vert_index = 0u;
		base->numLods = static_cast<unsigned char>(models.size());
		for (std::size_t lod = 0; lod < models.size(); lod++)
		{
			if (lod > 0 && models[lod].bones.size() != bones.size())
			{
				ZONETOOL_WARNING("CAST XModel \"%s\": lod %zu has %zu bones and lod 0 has %zu, the skeleton of lod 0 is used",
					name.data(), lod, models[lod].bones.size(), bones.size());
			}

			auto& info = base->lodInfo[lod];
			info.dist = no_cull_lod_dist;
			info.surfIndex = static_cast<unsigned short>(state.materials.size());
			info.modelSurfs = build_lod_surfs(models[lod], lod, name, bones.size(), has_skeleton, state, base_vert_index, mem);
			info.numsurfs = info.modelSurfs->numsurfs;
			cache.surfs.insert_or_assign(info.modelSurfs->name, info.modelSurfs);
		}

		set_bounds(base->boundsMidPoint, base->boundsHalfSize, state.mins, state.maxs);
		base->boundsRadius = std::sqrt(length_squared(base->boundsHalfSize));
		base->radius = max_distance_from(models[0].meshes, base->boundsMidPoint);

		auto* bone_info = mem->allocate<XBoneInfo>(bones.size());
		calculate_bone_bounds(bone_info, bones.size(), models[0].meshes, state.lod0_influences);
		base->boneInfo = reinterpret_cast<char*>(bone_info);

		const auto distances = lod_distances(models, base->radius, name);
		for (std::size_t lod = 0; lod < models.size(); lod++)
		{
			base->lodDistSq[lod] = lod_dist_sq(distances[lod], base->radius);
			base->lodInfo[lod].modelSurfs->surfaceArea = base->lodDistSq[lod];
		}

		cache.bases.insert_or_assign(base->name, base);

		constexpr std::size_t max_materials = std::numeric_limits<decltype(XModel::numMaterials)>::max();
		if (state.materials.size() > max_materials)
		{
			throw cs::cast_error(utils::string::va("CAST XModel \"%s\": the model has %zu surfaces in all lods, the game supports at most %zu",
				name.data(), state.materials.size(), max_materials));
		}

		auto* model = mem->allocate<XModel>();
		model->name = mem->duplicate_string(name);
		model->base = base;
		model->flags = XMODEL_FLAG_UNK_0x800;
		model->numMaterials = static_cast<unsigned char>(state.materials.size());
		model->materialHandles = mem->allocate<Material*>(state.materials.size());
		model->materialIndices = mem->allocate<unsigned short>(state.materials.size());
		model->runtimeSize = static_cast<unsigned short>(sizeof(XModel) + state.materials.size() * (sizeof(Material*) + sizeof(unsigned short)));

		for (std::size_t i = 0; i < state.materials.size(); i++)
		{
			model->materialHandles[i] = mem->allocate<Material>();
			model->materialHandles[i]->name = mem->duplicate_string(state.materials[i]);
			model->materialIndices[i] = 0xFFFF;
		}

		ZONETOOL_INFO("CAST XModel \"%s\": %u bones (%u root), %zu lods, %zu surfaces, %zu vertices, %zu triangles",
			name.data(), base->numBones, base->numRootBones, models.size(), state.materials.size(), state.vertex_count, state.triangle_count);
		ZONETOOL_INFO("CAST XModel \"%s\": bounds %.2f %.2f %.2f to %.2f %.2f %.2f, radius %.2f",
			name.data(), state.mins[0], state.mins[1], state.mins[2], state.maxs[0], state.maxs[1], state.maxs[2], base->radius);

		return model;
	}
}
