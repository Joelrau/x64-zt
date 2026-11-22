#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"

#include <DirectXTex.h>
#include <iw7/assets/gfxworld.hpp>
#include <iw7/assets/gfxlightmap.hpp>
#include <iw7/assets/gfxworld_tr.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace gfxworld
	{
		namespace
		{
			namespace target = zonetool::iw7;
			namespace source = zonetool::iw5;

			constexpr auto static_model_flag_no_cast_shadow = 0x10;
			constexpr auto static_model_flag_ground_lighting = 0x20;
			constexpr auto static_model_flag_lightgrid_lighting = 0x40;
			constexpr auto static_model_flag_scale_one = 9;

			target::GfxImage* image_reference(const char* name, utils::memory::allocator& allocator)
			{
				if (!name)
				{
					return nullptr;
				}

				const auto reference = allocator.allocate<target::GfxImage>();
				reference->name = allocator.duplicate_string(name);
				return reference;
			}

			target::GfxImage* image_reference(const x86::pointer<source::GfxImage>& image, utils::memory::allocator& allocator)
			{
				return image.value ? image_reference(image->name.get(), allocator) : nullptr;
			}

			target::Material* material_reference(const x86::pointer<source::Material>& material, utils::memory::allocator& allocator)
			{
				if (!material.value)
				{
					return nullptr;
				}

				const auto reference = allocator.allocate<target::Material>();
				reference->name = allocator.duplicate_string(material->info.name.get());
				return reference;
			}

			target::GfxImage* generate_reflection_probe_array_image(const unsigned int probe_count, utils::memory::allocator& allocator)
			{
				constexpr auto size = 128u;
				constexpr auto levels = 8u;
				constexpr unsigned short neutral_grey = 0x3400; // half float 0.25

				std::vector<std::vector<unsigned short>> storage;
				std::vector<DirectX::Image> images;
				storage.reserve(static_cast<std::size_t>(probe_count) * 6 * levels);
				for (unsigned int face = 0; face < probe_count * 6; face++)
				{
					for (unsigned int level = 0; level < levels; level++)
					{
						const auto extent = std::max(1u, size >> level);
						auto& pixels = storage.emplace_back(static_cast<std::size_t>(extent) * extent * 4, neutral_grey);
						for (std::size_t i = 3; i < pixels.size(); i += 4)
						{
							pixels[i] = 0x3C00; // alpha 1.0
						}

						DirectX::Image image{};
						image.width = extent;
						image.height = extent;
						image.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
						image.pixels = reinterpret_cast<std::uint8_t*>(pixels.data());
						DirectX::ComputePitch(image.format, image.width, image.height, image.rowPitch, image.slicePitch);
						images.push_back(image);
					}
				}

				DirectX::TexMetadata metadata{};
				metadata.width = size;
				metadata.height = size;
				metadata.depth = 1;
				metadata.arraySize = static_cast<std::size_t>(probe_count) * 6;
				metadata.mipLevels = levels;
				metadata.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
				metadata.miscFlags |= DirectX::TEX_MISC_TEXTURECUBE;

				DirectX::ScratchImage compressed;
				const auto compress_result = DirectX::Compress(images.data(), images.size(), metadata, DXGI_FORMAT_BC6H_UF16,
					DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, compressed);
				if (FAILED(compress_result))
				{
					ZONETOOL_WARNING("Reflection probe array compression failed (0x%08X)", static_cast<unsigned int>(compress_result));
					return image_reference("*reflection_probe_array", allocator);
				}

				const auto directory = filesystem::get_dump_path() + "images\\";
				std::filesystem::create_directories(directory);
				const auto path = std::filesystem::path(directory) / "_reflection_probe_array.dds";
				const auto save_result = DirectX::SaveToDDSFile(compressed.GetImages(), compressed.GetImageCount(), compressed.GetMetadata(),
					DirectX::DDS_FLAGS_NONE, path.c_str());
				if (FAILED(save_result))
				{
					ZONETOOL_WARNING("Reflection probe array could not be saved (0x%08X)", static_cast<unsigned int>(save_result));
				}

				return image_reference("*reflection_probe_array", allocator);
			}

			// A structurally empty Umbra tome whose view volume is a small cube at +200000 on every axis,
			// outside the BSP range but inside Umbra's own +/-262144 tree range. Without a tome IW7 marks
			// nothing visible and the map renders black; with this one every query reports the camera
			// outside the view volume and the renderer falls back to drawing everything.
			// Load_UmbraTome checks only the magic (0xD600 high word, 0x12 to 0x14 low word), 16 byte
			// alignment (the zone writer aligns it) and umbraTomeSize >= m_size.
			char* generate_draw_everything_umbra_tome(unsigned int& size, utils::memory::allocator& allocator)
			{
				static_assert(sizeof(target::Umbra::ImpTome) == 368);
				constexpr auto origin = 200000.0f;
				constexpr auto extent = 64.0f;

				const auto tome = allocator.allocate<target::Umbra::ImpTome>();
				tome->m_versionMagic = 0xD6000014;
				tome->m_size = sizeof(target::Umbra::ImpTome);
				tome->m_lodBaseDistance = 128.0f;
				tome->m_flags = 0;
				tome->m_treeMin = {origin, origin, origin};
				tome->m_treeMax = {origin + extent, origin + extent, origin + extent};
				tome->m_boundsMin = tome->m_treeMin;
				tome->m_boundsMax = tome->m_treeMax;
				tome->m_clusterCoordScale = 1.0f;

				// CRC-32C over the bytes from m_size onwards, as the Umbra optimizer writes it.
				// IW7 does not check it at load time.
				const auto bytes = reinterpret_cast<const unsigned char*>(&tome->m_size);
				const auto count = tome->m_size - offsetof(target::Umbra::ImpTome, m_size);
				auto crc = 0xFFFFFFFFu;
				for (std::size_t i = 0; i < count; i++)
				{
					crc ^= bytes[i];
					for (auto bit = 0; bit < 8; bit++)
					{
						crc = (crc & 1u) ? ((crc >> 1) ^ 0x82F63B78u) : (crc >> 1);
					}
				}
				tome->m_crc32 = ~crc;

				size = tome->m_size;
				return reinterpret_cast<char*>(tome);
			}

			target::GfxImage* generate_ies_lookup(utils::memory::allocator& allocator)
			{
				return image_reference("*ieslookup", allocator);
			}

			unsigned int first_reflection_probe(const unsigned int reflection_probe_count)
			{
				return reflection_probe_count > 1 ? 1 : 0; // slot 0 is the invalid-probe sentinel
			}

			template <typename Type>
			void expand_bounds(float* lows, float* highs, const Type& bounds)
			{
				for (auto i = 0; i < 3; i++)
				{
					lows[i] = std::min(lows[i], bounds.midPoint[i] - bounds.halfSize[i]);
					highs[i] = std::max(highs[i], bounds.midPoint[i] + bounds.halfSize[i]);
				}
			}

			void convert_aabb_node(target::GfxAabbTree* destination, const source::GfxAabbTree* node, const int children_index_delta)
			{
				std::memcpy(&destination->bounds, &node->bounds, sizeof(float[2][3]));
				destination->startSurfIndex = node->startSurfIndex;
				destination->surfaceCount = node->surfaceCount;
				destination->smodelIndexCount = node->smodelIndexCount;
				destination->smodelIndexes = node->smodelIndexes.get();
				destination->childCount = node->childCount;
				destination->childrenOffset = static_cast<int>(
					(node->childrenOffset / sizeof(source::GfxAabbTree) + children_index_delta) * sizeof(target::GfxAabbTree));
			}

			void convert_planes_and_cells(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator,
				const bool merge_cells)
			{
				const auto source_cell_count = asset->dpvsPlanes.cellCount;

				new_asset->dpvsPlanes.cellCount = merge_cells ? 1 : source_cell_count;

				static_assert(sizeof(source::cplane_s) == sizeof(target::cplane_s));
				new_asset->dpvsPlanes.planes = allocator.allocate_array<target::cplane_s>(asset->planeCount);
				std::memcpy(new_asset->dpvsPlanes.planes, asset->dpvsPlanes.planes.get(), sizeof(target::cplane_s) * asset->planeCount);

				new_asset->dpvsPlanes.nodes = allocator.allocate_array<unsigned short>(asset->nodeCount);
				std::memcpy(new_asset->dpvsPlanes.nodes, asset->dpvsPlanes.nodes.get(), sizeof(unsigned short) * asset->nodeCount);

				if (asset->dpvsPlanes.sceneEntCellBits.value)
				{
					const auto words = static_cast<std::size_t>(source_cell_count) * 512;
					new_asset->dpvsPlanes.sceneEntCellBits = allocator.allocate_array<unsigned int>(words);
				}

				if (merge_cells)
				{
					// node value v: 0 solid, v <= cellCount leaf of cell v-1, else plane v-(cellCount+1);
					// front child at node+2, back child at node+node[1]
					auto* nodes = new_asset->dpvsPlanes.nodes;
					std::vector<int> pending{0};
					while (!pending.empty())
					{
						const auto node = pending.back();
						pending.pop_back();
						if (node < 0 || node + 1 >= asset->nodeCount)
						{
							continue;
						}

						auto& value = nodes[node];
						if (value == 0)
						{
							continue;
						}

						if (value <= source_cell_count)
						{
							value = 1;
							continue;
						}

						value = static_cast<unsigned short>(value - (source_cell_count + 1) + 2);
						pending.push_back(node + 2);
						pending.push_back(node + nodes[node + 1]);
					}

					new_asset->cells = allocator.allocate_array<target::GfxCell>(1);
					float lows[3]{FLT_MAX, FLT_MAX, FLT_MAX};
					float highs[3]{-FLT_MAX, -FLT_MAX, -FLT_MAX};
					for (auto i = 0; i < source_cell_count; i++)
					{
						expand_bounds(lows, highs, asset->cells[i].bounds);
					}

					for (auto i = 0; i < 3; i++)
					{
						new_asset->cells[0].bounds.midPoint[i] = (lows[i] + highs[i]) * 0.5f;
						new_asset->cells[0].bounds.halfSize[i] = (highs[i] - lows[i]) * 0.5f;
					}
				}
				else
				{
					new_asset->cells = allocator.allocate_array<target::GfxCell>(source_cell_count);
					for (auto i = 0; i < source_cell_count; i++)
					{
						const auto& cell = asset->cells[i];
						auto& new_cell = new_asset->cells[i];

						std::memcpy(&new_cell.bounds, &cell.bounds, sizeof(float[2][3]));
						new_cell.portalCount = static_cast<unsigned short>(cell.portalCount);
						new_cell.portals = allocator.allocate_array<target::GfxPortal>(cell.portalCount);

						for (auto j = 0; j < cell.portalCount; j++)
						{
							const auto& portal = cell.portals[j];
							auto& new_portal = new_cell.portals[j];

							std::memcpy(&new_portal.plane, &portal.plane, sizeof(float[4]));
							new_portal.vertexCount = static_cast<char>(portal.vertexCount);
							new_portal.vertices = reinterpret_cast<vec3_t*>(allocator.allocate_array<float>(3ull * portal.vertexCount));
							std::memcpy(new_portal.vertices, portal.vertices.get(), sizeof(float[3]) * portal.vertexCount);
							new_portal.cellIndex = portal.cellIndex;
							new_portal.closeDistance = 0;
							std::memcpy(&new_portal.hullAxis, &portal.hullAxis, sizeof(float[2][3]));
						}
					}
				}

				new_asset->cellTransientInfos = allocator.allocate_array<target::GfxCellTransientInfo>(new_asset->dpvsPlanes.cellCount);
				for (unsigned short i = 0; i < new_asset->dpvsPlanes.cellCount; i++)
				{
					new_asset->cellTransientInfos[i].aabbTreeIndex = i;
					new_asset->cellTransientInfos[i].transientZone = 0;
				}
			}

			void convert_aabb_trees(source::GfxWorld* asset, target::GfxWorldTransientZone* zone, utils::memory::allocator& allocator,
				const bool merge_cells)
			{
				const auto source_cell_count = asset->dpvsPlanes.cellCount;

				zone->cellCount = merge_cells ? 1 : source_cell_count;
				zone->aabbTreeCounts = allocator.allocate_array<target::GfxCellTreeCount>(zone->cellCount);
				zone->aabbTrees = allocator.allocate_array<target::GfxCellTree>(zone->cellCount);

				if (!merge_cells)
				{
					for (auto i = 0; i < source_cell_count; i++)
					{
						const auto count = asset->aabbTreeCounts[i].aabbTreeCount;
						zone->aabbTreeCounts[i].aabbTreeCount = count;
						zone->aabbTrees[i].aabbTree = allocator.allocate_array<target::GfxAabbTree>(count);
						for (auto j = 0; j < count; j++)
						{
							convert_aabb_node(&zone->aabbTrees[i].aabbTree[j], &asset->aabbTrees[i].aabbTree[j], 0);
						}
					}

					return;
				}

				// A new root takes the old per-cell roots as consecutive children (slots 1..N), the
				// other nodes follow in order. A node fully in view is marked from its own surface
				// range and model list, so the root carries the union of everything below it.
				std::vector<int> cell_roots;
				auto node_count = 1;
				for (auto i = 0; i < source_cell_count; i++)
				{
					if (asset->aabbTreeCounts[i].aabbTreeCount > 0)
					{
						cell_roots.push_back(i);
						node_count += asset->aabbTreeCounts[i].aabbTreeCount;
					}
				}

				const auto tree = allocator.allocate_array<target::GfxAabbTree>(node_count);
				auto& root = tree[0];

				float lows[3]{FLT_MAX, FLT_MAX, FLT_MAX};
				float highs[3]{-FLT_MAX, -FLT_MAX, -FLT_MAX};
				unsigned int surface_begin = UINT_MAX;
				unsigned int surface_end = 0;
				std::vector<unsigned short> root_models;

				const auto child_slots = static_cast<int>(cell_roots.size());
				auto next_node = 1 + child_slots;
				for (auto slot = 0; slot < child_slots; slot++)
				{
					const auto cell = cell_roots[slot];
					const auto nodes = asset->aabbTrees[cell].aabbTree.get();
					const auto count = asset->aabbTreeCounts[cell].aabbTreeCount;

					const auto root_slot = 1 + slot;
					convert_aabb_node(&tree[root_slot], &nodes[0], next_node - 1 - root_slot);
					for (auto j = 1; j < count; j++)
					{
						convert_aabb_node(&tree[next_node + j - 1], &nodes[j], 0);
					}
					next_node += count - 1;

					expand_bounds(lows, highs, nodes[0].bounds);
					if (nodes[0].surfaceCount)
					{
						surface_begin = std::min<unsigned int>(surface_begin, nodes[0].startSurfIndex);
						surface_end = std::max<unsigned int>(surface_end, nodes[0].startSurfIndex + nodes[0].surfaceCount);
					}

					const auto models = nodes[0].smodelIndexes.get();
					root_models.insert(root_models.end(), models, models + nodes[0].smodelIndexCount);
				}

				for (auto i = 0; i < 3; i++)
				{
					root.bounds.midPoint[i] = (lows[i] + highs[i]) * 0.5f;
					root.bounds.halfSize[i] = (highs[i] - lows[i]) * 0.5f;
				}

				root.childCount = static_cast<unsigned short>(child_slots);
				root.childrenOffset = static_cast<int>(sizeof(target::GfxAabbTree));
				if (surface_begin < surface_end)
				{
					root.startSurfIndex = surface_begin;
					root.surfaceCount = static_cast<unsigned short>(surface_end - surface_begin);
				}

				root.smodelIndexCount = static_cast<unsigned short>(root_models.size());
				if (!root_models.empty())
				{
					root.smodelIndexes = allocator.allocate_array<unsigned short>(root_models.size());
					std::memcpy(root.smodelIndexes, root_models.data(), sizeof(unsigned short) * root_models.size());
				}

				zone->aabbTreeCounts[0].aabbTreeCount = node_count;
				zone->aabbTrees[0].aabbTree = tree;
			}

			void convert_reflection_probes(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				constexpr auto fallback_half_extent = 262144.0f;
				constexpr auto fallback_feather = 8.0f;
				constexpr auto volume_margin = 16.0f;
				constexpr auto volume_feather = 32.0f;
				constexpr auto orphan_minimum_half_size = 64.0f;
				constexpr auto orphan_maximum_half_size = 2048.0f;

				const auto first_probe = first_reflection_probe(asset->draw.reflectionProbeCount);
				const auto probe_count = asset->draw.reflectionProbeCount - first_probe;
				const auto instance_count = probe_count + 1; // the world fallback

				auto& data = new_asset->draw.reflectionProbeData;
				data.reflectionProbeCount = probe_count;
				data.sharedReflectionProbeCount = 0;
				data.reflectionProbes = allocator.allocate_array<target::GfxReflectionProbe>(probe_count);
				data.reflectionProbeArrayImage = generate_reflection_probe_array_image(probe_count, allocator);
				data.reflectionProbeLightgridSampleData = allocator.allocate_array<target::GfxReflectionProbeSampleData>(probe_count);

				// IW7 selects a probe at runtime from the instance volumes, so each volume is the union of
				// the bounds of everything the source assigned to that probe
				struct probe_region
				{
					unsigned int references = 0;
					float lows[3]{FLT_MAX, FLT_MAX, FLT_MAX};
					float highs[3]{-FLT_MAX, -FLT_MAX, -FLT_MAX};
				};

				std::vector<probe_region> regions(asset->draw.reflectionProbeCount);
				const auto add_bounds = [&](const unsigned int probe, const Bounds& bounds)
				{
					if (probe < regions.size())
					{
						regions[probe].references++;
						expand_bounds(regions[probe].lows, regions[probe].highs, bounds);
					}
				};

				for (unsigned int i = 0; i < asset->surfaceCount; i++)
				{
					add_bounds(asset->dpvs.surfaces[i].laf.fields.reflectionProbeIndex, asset->dpvs.surfacesBounds[i].bounds);
				}

				for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
				{
					add_bounds(asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex, asset->dpvs.smodelInsts[i].bounds);
				}

				const auto nearest_distance = [&](const unsigned int index)
				{
					auto best = FLT_MAX;
					const auto* a = asset->draw.reflectionProbeOrigins[index].origin;
					for (auto j = first_probe; j < asset->draw.reflectionProbeCount; j++)
					{
						if (j == index)
						{
							continue;
						}

						const auto* b = asset->draw.reflectionProbeOrigins[j].origin;
						const float delta[3] = {b[0] - a[0], b[1] - a[1], b[2] - a[2]};
						best = std::min(best, std::sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]));
					}

					return best;
				};

				data.reflectionProbeInstanceCount = instance_count;
				const auto instances = allocator.allocate_array<target::GfxReflectionProbeInstance>(instance_count);
				data.reflectionProbeInstances = instances;

				// probe 0 owns two slots (itself and the fallback), so 2 + (count - 1) == instance_count
				const auto instance_indices = allocator.allocate_array<unsigned int>(instance_count);

				for (unsigned int i = 0; i < probe_count; i++)
				{
					const auto source_index = first_probe + i;
					const auto& origin = asset->draw.reflectionProbeOrigins[source_index];
					auto& probe = data.reflectionProbes[i];
					auto& instance = instances[i];

					probe.probeRelightingIndex = static_cast<unsigned int>(-1);
					std::memcpy(probe.origin, origin.origin, sizeof(probe.origin));
					if (i == 0)
					{
						probe.probeInstanceCount = 2;
						probe.probeInstances = &instance_indices[0];
						probe.probeInstances[0] = 0;
						probe.probeInstances[1] = probe_count;
					}
					else
					{
						probe.probeInstanceCount = 1;
						probe.probeInstances = &instance_indices[i + 1];
						probe.probeInstances[0] = i;
					}

					std::memcpy(instance.probePosition, origin.origin, sizeof(instance.probePosition));
					instance.probeImageIndex = static_cast<unsigned short>(i);
					instance.probeRotation[3] = 1.0f;
					instance.volumeObb.xAxis[0] = 1.0f;
					instance.volumeObb.yAxis[1] = 1.0f;
					instance.volumeObb.zAxis[2] = 1.0f;

					float half[3];
					const auto& region = regions[source_index];
					if (region.references)
					{
						for (auto k = 0; k < 3; k++)
						{
							instance.volumeObb.center[k] = (region.lows[k] + region.highs[k]) * 0.5f;
							half[k] = (region.highs[k] - region.lows[k]) * 0.5f + volume_margin;
						}
					}
					else
					{
						auto extent = nearest_distance(source_index) * 0.5f;
						if (!(extent > 0.0f) || extent > orphan_maximum_half_size)
						{
							extent = orphan_maximum_half_size;
						}

						extent = std::max(extent, orphan_minimum_half_size);
						for (auto k = 0; k < 3; k++)
						{
							instance.volumeObb.center[k] = origin.origin[k];
							half[k] = extent;
						}
					}

					std::memcpy(instance.volumeObb.halfSize, half, sizeof(half));
					instance.priority = -(half[0] + half[1] + half[2]); // the tighter volume wins
					for (auto k = 0; k < 3; k++)
					{
						instance.feather[k] = std::min(volume_feather, half[k] * 0.25f);
					}
				}

				auto& fallback = instances[probe_count];
				const auto* fallback_origin = data.reflectionProbes[0].origin;
				std::memcpy(fallback.probePosition, fallback_origin, sizeof(fallback.probePosition));
				std::memcpy(fallback.volumeObb.center, fallback_origin, sizeof(fallback.volumeObb.center));
				fallback.probeImageIndex = 0;
				fallback.probeRotation[3] = 1.0f;
				fallback.volumeObb.xAxis[0] = 1.0f;
				fallback.volumeObb.yAxis[1] = 1.0f;
				fallback.volumeObb.zAxis[2] = 1.0f;
				for (auto k = 0; k < 3; k++)
				{
					fallback.volumeObb.halfSize[k] = fallback_half_extent;
					fallback.feather[k] = fallback_feather;
				}
				fallback.priority = -FLT_MAX;
			}

			void convert_lightmaps(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				auto& draw = new_asset->draw;
				draw.lightmapOverridePrimary = image_reference(asset->draw.lightmapOverridePrimary, allocator);
				draw.lightmapOverrideSecondary = image_reference(asset->draw.lightmapOverrideSecondary, allocator);

				draw.lightMapCount = asset->draw.lightmapCount;
				draw.lightMaps = allocator.allocate_array<target::GfxLightMap*>(asset->draw.lightmapCount);
				for (auto i = 0; i < asset->draw.lightmapCount; i++)
				{
					const auto lightmap = allocator.allocate<target::GfxLightMap>();
					lightmap->name = allocator.duplicate_string(utils::string::va("*lightmap%d", i));
					lightmap->textures[0] = image_reference(asset->draw.lightmaps[i].primary, allocator);
					lightmap->textures[1] = image_reference(asset->draw.lightmaps[i].secondary, allocator);
					lightmap->textures[2] = image_reference(utils::string::va("*lightmap%d_secondunorm", i), allocator);
					draw.lightMaps[i] = lightmap;
				}
			}

			void convert_transient_zone(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator,
				const bool merge_cells)
			{
				auto& draw = new_asset->draw;
				draw.transientZoneCount = 1;

				const auto zone = allocator.allocate<target::GfxWorldTransientZone>();
				draw.transientZones[0] = zone;
				zone->name = allocator.duplicate_string(filesystem::get_fastfile());
				zone->transientZoneIndex = 0;

				zone->vertexCount = asset->draw.vertexCount;
				zone->vd.vertices = allocator.allocate_array<target::GfxWorldVertex>(asset->draw.vertexCount);
				for (unsigned int i = 0; i < asset->draw.vertexCount; i++)
				{
					const auto& vertex = asset->draw.vd.vertices[i];
					auto& new_vertex = zone->vd.vertices[i];

					static_assert(sizeof(source::GfxWorldVertex) == sizeof(target::GfxWorldVertex));
					std::memcpy(&new_vertex, &vertex, sizeof(target::GfxWorldVertex));

					float normal[3]{};
					PackedVec::Vec3UnpackUnitVec_T6(vertex.normal.array, normal);
					new_vertex.normal.packed = PackedVec::Vec3PackUnitVec_H1(normal);

					float tangent[3]{};
					PackedVec::Vec3UnpackUnitVec_T6(vertex.tangent.array, tangent);
					new_vertex.tangent.packed = PackedVec::Vec3PackUnitVec_H1(tangent);

					new_vertex.color.array[0] = vertex.color.array[2]; // bgra to rgba
					new_vertex.color.array[1] = vertex.color.array[1];
					new_vertex.color.array[2] = vertex.color.array[0];
					new_vertex.color.array[3] = vertex.color.array[3];
				}

				zone->vertexLayerDataSize = asset->draw.vertexLayerDataSize;
				if (asset->draw.vertexLayerDataSize && asset->draw.vld.data.value)
				{
					zone->vld.data = allocator.allocate_array<unsigned char>(asset->draw.vertexLayerDataSize);
					std::memcpy(zone->vld.data, asset->draw.vld.data.get(), asset->draw.vertexLayerDataSize);
				}

				convert_aabb_trees(asset, zone, allocator, merge_cells);

				draw.indexCount = asset->draw.indexCount;
				draw.indices = allocator.allocate_array<unsigned short>(asset->draw.indexCount);
				std::memcpy(draw.indices, asset->draw.indices.get(), sizeof(unsigned short) * asset->draw.indexCount);
			}

			void convert_light_grid(target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				auto& grid = new_asset->lightGrid;

				constexpr int unknown_values[] = {0, 0, 5, 5, 6, 32, 32, 64, 0};
				std::memcpy(grid.unk, unknown_values, sizeof(unknown_values));
				grid.tableVersion = 1;
				grid.paletteVersion = 1;
				grid.rangeExponent8BitsEncoding = 0;
				grid.rangeExponent12BitsEncoding = 4;
				grid.rangeExponent16BitsEncoding = 23;
				grid.stageCount = 0;

				// IW7 maps ship this 3-entry palette and 2-node tree as a stub next to the probe volume
				static const int palette_addresses[3] = {0, 30, 86};
				static const unsigned char palette_bitstream[116] = {
					0xE7, 0x1C, 0x00, 0xF8, 0x08, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF1, 0x00, 0x08, 0x80, 0xF8, 0x80,
					0x80, 0x80, 0xB8, 0x48, 0x00, 0x80, 0xF8, 0x08, 0x80, 0x80, 0x80, 0x48, 0x48, 0x00, 0x00, 0x00,
					0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
					0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x5C, 0x5E, 0x4A, 0x3F,
					0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF,
					0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
					0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80,
					0x80, 0x80, 0x80, 0x00,
				};

				grid.paletteEntryCount = 3;
				grid.paletteEntryAddress = allocator.allocate_array<int>(3);
				std::memcpy(grid.paletteEntryAddress, palette_addresses, sizeof(palette_addresses));
				grid.paletteBitstreamSize = sizeof(palette_bitstream);
				grid.paletteBitstream = allocator.allocate_array<unsigned char>(sizeof(palette_bitstream));
				std::memcpy(grid.paletteBitstream, palette_bitstream, sizeof(palette_bitstream));

				// engine constants in IW7, identical in every shipped map
				for (auto i = 0; i < 56; i++)
				{
					grid.defaultLightGridColors.rgb[i][2] = 0.21875f;
				}

				static const unsigned int node_table[2] = {16777217u, 2147483648u};
				static const unsigned char leaf_table[6] = {0x01, 0x83, 0x00, 0x04, 0x06, 0x11};

				grid.tree.maxDepth = 1;
				grid.tree.nodeCount = 2;
				grid.tree.leafCount = 1;
				grid.tree.coordMinGridSpace[0] = 4092;
				grid.tree.coordMinGridSpace[1] = 4092;
				grid.tree.coordMinGridSpace[2] = 2047;
				grid.tree.coordMaxGridSpace[0] = 4100;
				grid.tree.coordMaxGridSpace[1] = 4100;
				grid.tree.coordMaxGridSpace[2] = 2049;
				grid.tree.coordHalfSizeGridSpace[0] = 4;
				grid.tree.coordHalfSizeGridSpace[1] = 4;
				grid.tree.coordHalfSizeGridSpace[2] = 1;
				grid.tree.defaultColorIndexBitCount = 2;
				grid.tree.defaultLightIndexBitCount = 32;
				grid.tree.p_nodeTable = allocator.allocate_array<unsigned int>(2);
				std::memcpy(grid.tree.p_nodeTable, node_table, sizeof(node_table));
				grid.tree.leafTableSize = sizeof(leaf_table);
				grid.tree.p_leafTable = allocator.allocate_array<unsigned char>(sizeof(leaf_table));
				std::memcpy(grid.tree.p_leafTable, leaf_table, sizeof(leaf_table));

				grid.probeData.zoneCount = 1;
				grid.probeData.zones = allocator.allocate_array<target::GfxGpuLightGridZone>(1); // no probe volume is built
			}

			void convert_voxel_trees(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				new_asset->voxelTreeCount = new_asset->skyCount;
				new_asset->voxelTree = allocator.allocate_array<target::GfxVoxelTree>(new_asset->voxelTreeCount);

				for (auto i = 0; i < new_asset->skyCount; i++)
				{
					auto& tree = new_asset->voxelTree[i];

					// union of the sky surface bounds
					float lows[3]{FLT_MAX, FLT_MAX, FLT_MAX};
					float highs[3]{-FLT_MAX, -FLT_MAX, -FLT_MAX};
					auto seen = 0;
					const auto& sky = asset->skies[i];
					for (auto j = 0; j < sky.skySurfCount; j++)
					{
						const auto sorted = sky.skyStartSurfs[j];
						if (sorted < 0 || static_cast<unsigned int>(sorted) >= asset->dpvs.staticSurfaceCount)
						{
							continue;
						}

						const auto index = asset->dpvs.sortedSurfIndex[sorted];
						if (index >= asset->surfaceCount)
						{
							continue;
						}

						expand_bounds(lows, highs, asset->dpvs.surfacesBounds[index].bounds);
						seen++;
					}

					if (seen)
					{
						for (auto k = 0; k < 3; k++)
						{
							tree.zoneBound.midPoint[k] = (lows[k] + highs[k]) * 0.5f;
							tree.zoneBound.halfSize[k] = (highs[k] - lows[k]) * 0.5f;
						}
					}

					tree.voxelTopDownViewNodeCount = 1;
					tree.voxelInternalNodeCount = 1;
					tree.voxelLeafNodeCount = 1;
					tree.lightListArraySize = 1;

					tree.voxelTreeHeader = allocator.allocate<target::GfxVoxelTreeHeader>();
					tree.voxelTopDownViewNodeArray = allocator.allocate_array<target::GfxVoxelTopDownViewNode>(1);
					tree.voxelTopDownViewNodeArray->firstNodeIndex = -1;
					tree.voxelTopDownViewNodeArray->zMin = 2147483647;
					tree.voxelTopDownViewNodeArray->zMax = -2147483647 - 1;
					tree.voxelInternalNodeArray = allocator.allocate_array<target::GfxVoxelInternalNode>(1);
					tree.voxelLeafNodeArray = allocator.allocate_array<target::GfxVoxelLeafNode>(1);
					tree.lightListArray = allocator.allocate_array<unsigned short>(1);
					tree.voxelInternalNodeDynamicLightList = allocator.allocate_array<unsigned int>(2);
				}
			}

			void convert_light_regions(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				new_asset->lightRegion = allocator.allocate_array<target::GfxLightRegion>(new_asset->primaryLightCount);
				if (!asset->lightRegion.value)
				{
					return;
				}

				for (unsigned int i = 0; i < new_asset->primaryLightCount && i < asset->primaryLightCount; i++)
				{
					const auto& region = asset->lightRegion[i];
					auto& new_region = new_asset->lightRegion[i];

					new_region.hullCount = region.hullCount;
					new_region.hulls = allocator.allocate_array<target::GfxLightRegionHull>(region.hullCount);
					for (unsigned int j = 0; j < region.hullCount; j++)
					{
						const auto& hull = region.hulls[j];
						auto& new_hull = new_region.hulls[j];

						std::memcpy(new_hull.kdopMidPoint, hull.kdopMidPoint, sizeof(hull.kdopMidPoint));
						std::memcpy(new_hull.kdopHalfSize, hull.kdopHalfSize, sizeof(hull.kdopHalfSize));
						new_hull.axisCount = hull.axisCount;
						static_assert(sizeof(target::GfxLightRegionAxis) == sizeof(source::GfxLightRegionAxis));
						new_hull.axis = allocator.allocate_array<target::GfxLightRegionAxis>(hull.axisCount);
						std::memcpy(new_hull.axis, hull.axis.get(), sizeof(target::GfxLightRegionAxis) * hull.axisCount);
					}
				}
			}

			void convert_dpvs(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				const auto& dpvs = asset->dpvs;
				auto& new_dpvs = new_asset->dpvs;

				new_dpvs.smodelCount = dpvs.smodelCount;
				new_dpvs.staticSurfaceCount = dpvs.staticSurfaceCount;
				new_dpvs.litOpaqueSurfsBegin = dpvs.litOpaqueSurfsBegin;
				new_dpvs.litOpaqueSurfsEnd = dpvs.litOpaqueSurfsEnd;
				new_dpvs.litDecalSurfsBegin = dpvs.litOpaqueSurfsEnd;
				new_dpvs.litDecalSurfsEnd = dpvs.litOpaqueSurfsEnd;
				new_dpvs.litTransSurfsBegin = dpvs.litTransSurfsBegin;
				new_dpvs.litTransSurfsEnd = dpvs.litTransSurfsEnd;
				new_dpvs.emissiveSurfsBegin = dpvs.emissiveSurfsBegin;
				new_dpvs.emissiveSurfsEnd = dpvs.emissiveSurfsEnd;
				new_dpvs.smodelVisDataCount = (new_dpvs.smodelCount + 0x1F) >> 5;
				new_dpvs.surfaceVisDataCount = (new_asset->surfaceCount + 0x1F) >> 5;
				new_dpvs.primaryLightVisDataCount = (new_asset->primaryLightCount + 0x1F) >> 5;
				new_dpvs.reflectionProbeVisDataCount = (new_asset->draw.reflectionProbeData.reflectionProbeInstanceCount + 0x1F) >> 5;
				new_dpvs.volumetricVisDataCount = (new_asset->draw.volumetrics.volumetricCount + 0x1F) >> 5;
				new_dpvs.decalVisDataCount = (new_asset->draw.decalVolumeCollectionCount + 0x1F) >> 5;

				new_dpvs.lodData = allocator.allocate_array<unsigned int>(new_dpvs.smodelCount + 1); // 1-based identity
				for (unsigned int i = 0; i < new_dpvs.smodelCount; i++)
				{
					new_dpvs.lodData[i + 1] = i;
				}

				new_dpvs.sortedSurfIndex = allocator.allocate_array<unsigned int>(dpvs.staticSurfaceCount);
				for (unsigned int i = 0; i < dpvs.staticSurfaceCount; i++)
				{
					new_dpvs.sortedSurfIndex[i] = dpvs.sortedSurfIndex[i];
				}

				static_assert(sizeof(target::GfxStaticModelInst) == sizeof(source::GfxStaticModelInst));
				new_dpvs.smodelInsts = allocator.allocate_array<target::GfxStaticModelInst>(dpvs.smodelCount);
				std::memcpy(new_dpvs.smodelInsts, dpvs.smodelInsts.get(), sizeof(target::GfxStaticModelInst) * dpvs.smodelCount);

				// flags bits 3..7 are the sun lights a surface casts for; bit 0 means the same in both engines
				const auto sun_light_count = std::min<unsigned int>(new_asset->lastSunPrimaryLightIndex, 5);
				const auto sun_light_mask = static_cast<unsigned char>((1 << sun_light_count) - 1);

				new_dpvs.surfaces = allocator.allocate_array<target::GfxSurface>(asset->surfaceCount);
				new_dpvs.surfacesBounds = allocator.allocate_array<target::GfxSurfaceBounds>(asset->surfaceCount);
				for (unsigned int i = 0; i < asset->surfaceCount; i++)
				{
					const auto& surface = dpvs.surfaces[i];
					auto& new_surface = new_dpvs.surfaces[i];

					new_surface.tris.vertexLayerData = surface.tris.vertexLayerData;
					new_surface.tris.firstVertex = surface.tris.firstVertex;
					new_surface.tris.vertexCount = surface.tris.vertexCount;
					new_surface.tris.triCount = surface.tris.triCount;
					new_surface.tris.baseIndex = surface.tris.baseIndex;
					new_surface.material = material_reference(surface.material, allocator);
					new_surface.lightmapIndex = surface.laf.fields.lightmapIndex == 0x1F ? 0 : surface.laf.fields.lightmapIndex;
					new_surface.flags = (surface.laf.fields.flags & 1) ? static_cast<unsigned char>(1 | (sun_light_mask << 3)) : 0;

					std::memcpy(&new_dpvs.surfacesBounds[i].bounds, &dpvs.surfacesBounds[i].bounds, sizeof(float[2][3]));
				}

				new_dpvs.smodelDrawInsts = allocator.allocate_array<target::GfxStaticModelDrawInst>(dpvs.smodelCount);
				for (unsigned int i = 0; i < dpvs.smodelCount; i++)
				{
					const auto& instance = dpvs.smodelDrawInsts[i];
					auto& new_instance = new_dpvs.smodelDrawInsts[i];

					static_assert(sizeof(target::GfxPackedPlacement) == sizeof(source::GfxPackedPlacement));
					std::memcpy(&new_instance.placement, &instance.placement, sizeof(target::GfxPackedPlacement));

					new_instance.model = allocator.allocate<target::XModel>();
					new_instance.model->name = allocator.duplicate_string(instance.model->name.get());

					new_instance.modelLightmapInfo.lightmapIndex = -1;
					new_instance.lightingHandle = instance.lightingHandle;
					new_instance.cullDist = instance.cullDist;
					new_instance.primaryLightEnvIndex = 0; // stock maps never light static models through it
					new_instance.reflectionProbeIndex = 0; // the probe is picked at runtime from the volumes
					new_instance.firstMtlSkinIndex = instance.firstMtlSkinIndex;
					new_instance.sunShadowFlags = sun_light_mask;
					new_instance.transientZone = 0;

					new_instance.flags = static_cast<unsigned short>(instance.flags);
					new_instance.flags |= static_model_flag_scale_one;
					if (instance.flags & 0x10)
					{
						new_instance.flags |= static_model_flag_no_cast_shadow;
					}

					if (instance.flags & 0x20)
					{
						new_instance.flags |= static_model_flag_ground_lighting;
					}

					new_instance.flags |= static_model_flag_lightgrid_lighting;
				}

				new_dpvs.surfaceMaterials = allocator.allocate_array<target::GfxDrawSurf>(asset->surfaceCount); // runtime

				// measured on the stock maps: min(lastSunPrimaryLightIndex, 5) and the visibility words rounded up to 32
				new_dpvs.sunShadowOptCount = sun_light_count;
				new_dpvs.sunSurfVisDataCount = (new_dpvs.surfaceVisDataCount + 31) & ~31u;

				new_dpvs.surfaceCastsSunShadow = allocator.allocate_array<unsigned int>(new_dpvs.surfaceVisDataCount);
				if (dpvs.surfaceCastsSunShadow.value)
				{
					std::memcpy(new_dpvs.surfaceCastsSunShadow, dpvs.surfaceCastsSunShadow.get(),
						sizeof(unsigned int) * new_dpvs.surfaceVisDataCount);
				}

				new_dpvs.sortedSmodelIndices = allocator.allocate_array<unsigned short>(dpvs.smodelCount);
				for (unsigned int i = 0; i < dpvs.smodelCount; i++)
				{
					new_dpvs.sortedSmodelIndices[i] = static_cast<unsigned short>(i);
				}

				new_dpvs.usageCount = dpvs.usageCount;
			}

			void convert_runtime_arrays(target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				const auto align_up = [](const unsigned int value, const unsigned int alignment)
				{
					return (value + (alignment - 1)) & ~(alignment - 1);
				};

				const auto lights = new_asset->primaryLightCount - new_asset->lastSunPrimaryLightIndex
					- new_asset->movingScriptablePrimaryLightCount - 1;

				new_asset->staticSpotOmniPrimaryLightCountAligned = align_up(lights, 32);

				new_asset->primaryLightMotionDetectBitsEntries = new_asset->staticSpotOmniPrimaryLightCountAligned >> 4;
				new_asset->primaryLightMotionDetectBits = allocator.allocate_array<unsigned int>(new_asset->primaryLightMotionDetectBitsEntries);

				new_asset->entityMotionBitsEntries = 134; // constant in shipped maps
				new_asset->entityMotionBits = allocator.allocate_array<unsigned int>(new_asset->entityMotionBitsEntries);

				new_asset->numPrimaryLightEntityShadowVisEntries = new_asset->staticSpotOmniPrimaryLightCountAligned * 0x86;
				new_asset->primaryLightEntityShadowVis = allocator.allocate_array<unsigned int>(new_asset->numPrimaryLightEntityShadowVisEntries);

				for (auto i = 0; i < 2; i++)
				{
					new_asset->dynEntMotionBitsEntries[i] = ((new_asset->dpvsDyn.dynEntClientCount[i] + 31) >> 5) * 2;
					new_asset->dynEntMotionBits[i] = allocator.allocate_array<unsigned int>(new_asset->dynEntMotionBitsEntries[i]);

					new_asset->numPrimaryLightDynEntShadowVisEntries[i] =
						(new_asset->staticSpotOmniPrimaryLightCountAligned * new_asset->dpvsDyn.dynEntClientCount[i]) >> 4;
					new_asset->primaryLightDynEntShadowVis[i] = allocator.allocate_array<unsigned int>(new_asset->numPrimaryLightDynEntShadowVisEntries[i]);
				}
			}
		}

		target::GfxWorld* convert(source::GfxWorld* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<target::GfxWorld>();

			new_asset->name = allocator.duplicate_string(asset->name.get());
			new_asset->baseName = allocator.duplicate_string(asset->baseName.get());
			new_asset->bspVersion = 159;
			new_asset->planeCount = asset->planeCount;
			new_asset->nodeCount = asset->nodeCount;
			new_asset->surfaceCount = asset->surfaceCount;

			new_asset->skyCount = asset->skyCount;
			new_asset->skies = allocator.allocate_array<target::GfxSky>(asset->skyCount);
			for (auto i = 0; i < asset->skyCount; i++)
			{
				new_asset->skies[i].skySurfCount = asset->skies[i].skySurfCount;
				new_asset->skies[i].skyStartSurfs = allocator.allocate_array<int>(asset->skies[i].skySurfCount);
				std::memcpy(new_asset->skies[i].skyStartSurfs, asset->skies[i].skyStartSurfs.get(), sizeof(int) * asset->skies[i].skySurfCount);
				new_asset->skies[i].skyImage = image_reference(asset->skies[i].skyImage, allocator);
				new_asset->skies[i].skySamplerState = asset->skies[i].skySamplerState;
			}

			new_asset->lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
			new_asset->primaryLightCount = asset->primaryLightCount;
			new_asset->movingScriptablePrimaryLightCount = 0;

			new_asset->sortKeyLitDecal = 7;
			new_asset->sortKeyEffectDecal = 14;
			new_asset->sortKeyTopDecal = 17;
			new_asset->sortKeyEffectAuto = 35;
			new_asset->sortKeyDistortion = 24;
			new_asset->sortKeyEffectDistortion = 36;
			new_asset->sortKey2D = 41;
			new_asset->sortKeyOpaqueBegin = 1;
			new_asset->sortKeyOpaqueEnd = 6;
			new_asset->sortKeyDecalBegin = 7;
			new_asset->sortKeyDecalEnd = 17;
			new_asset->sortKeyTransBegin = 18;
			new_asset->sortKeyTransEnd = 34;
			new_asset->sortKeyEmissiveBegin = 35;
			new_asset->sortKeyEmissiveEnd = 40;

			// Shipped IW7 maps have one cell and leave visibility to Umbra. With several, the sun shadow
			// pass only draws casters of the visible cells plus the cellCasterBits the game rebuilds by
			// a portal walk, so the sun leaks through walls from a sealed room.
			const auto merge_cells = asset->dpvsPlanes.cellCount > 1;
			convert_planes_and_cells(asset, new_asset, allocator, merge_cells);

			convert_reflection_probes(asset, new_asset, allocator);
			new_asset->draw.iesLookupTexture = generate_ies_lookup(allocator);
			convert_lightmaps(asset, new_asset, allocator);
			convert_transient_zone(asset, new_asset, allocator, merge_cells);
			convert_light_grid(new_asset, allocator);

			new_asset->frustumLights = allocator.allocate_array<target::GfxFrustumLights>(new_asset->primaryLightCount);
			new_asset->lightViewFrustums = allocator.allocate_array<target::GfxLightViewFrustum>(new_asset->primaryLightCount);
			convert_voxel_trees(asset, new_asset, allocator);

			new_asset->modelCount = asset->modelCount;
			new_asset->models = allocator.allocate_array<target::GfxBrushModel>(asset->modelCount);
			for (auto i = 0; i < asset->modelCount; i++)
			{
				std::memcpy(&new_asset->models[i].bounds, &asset->models[i].bounds, sizeof(float[2][3]));
				new_asset->models[i].radius = asset->models[i].radius;
				new_asset->models[i].startSurfIndex = asset->models[i].startSurfIndex;
				new_asset->models[i].surfaceCount = asset->models[i].surfaceCount;
			}

			std::memcpy(&new_asset->bounds, &asset->bounds, sizeof(Bounds));
			new_asset->checksum = asset->checksum;

			new_asset->materialMemoryCount = asset->materialMemoryCount;
			new_asset->materialMemory = allocator.allocate_array<target::MaterialMemory>(asset->materialMemoryCount);
			for (auto i = 0; i < asset->materialMemoryCount; i++)
			{
				new_asset->materialMemory[i].material = material_reference(asset->materialMemory[i].material, allocator);
				new_asset->materialMemory[i].memory = asset->materialMemory[i].memory;
			}

			const auto& sun = asset->sun;
			auto& new_sun = new_asset->sun;
			new_sun.hasValidData = sun.hasValidData;
			new_sun.spriteMaterial = material_reference(sun.spriteMaterial, allocator);
			new_sun.flareMaterial = material_reference(sun.flareMaterial, allocator);
			new_sun.spriteSize = sun.spriteSize;
			new_sun.flareMinSize = sun.flareMinSize;
			new_sun.flareMinDot = sun.flareMinDot;
			new_sun.flareMaxSize = sun.flareMaxSize;
			new_sun.flareMaxDot = sun.flareMaxDot;
			new_sun.flareMaxAlpha = sun.flareMaxAlpha;
			new_sun.flareFadeInTime = sun.flareFadeInTime;
			new_sun.flareFadeOutTime = sun.flareFadeOutTime;
			new_sun.blindMinDot = sun.blindMinDot;
			new_sun.blindMaxDot = sun.blindMaxDot;
			new_sun.blindMaxDarken = sun.blindMaxDarken;
			new_sun.blindFadeInTime = sun.blindFadeInTime;
			new_sun.blindFadeOutTime = sun.blindFadeOutTime;
			new_sun.glareMinDot = sun.glareMinDot;
			new_sun.glareMaxDot = sun.glareMaxDot;
			new_sun.glareMaxLighten = sun.glareMaxLighten;
			new_sun.glareFadeInTime = sun.glareFadeInTime;
			new_sun.glareFadeOutTime = sun.glareFadeOutTime;
			std::memcpy(new_sun.sunFxPosition, sun.sunFxPosition, sizeof(sun.sunFxPosition));

			std::memcpy(new_asset->outdoorLookupMatrix, asset->outdoorLookupMatrix, sizeof(asset->outdoorLookupMatrix));
			new_asset->outdoorImage = image_reference(asset->outdoorImage, allocator);
			new_asset->materialLod0SizeThreshold = 0.5f;

			new_asset->shadowGeomOptimized = allocator.allocate_array<target::GfxShadowGeometry>(new_asset->primaryLightCount); // no casters listed

			convert_light_regions(asset, new_asset, allocator);
			convert_dpvs(asset, new_asset, allocator);

			std::memcpy(new_asset->dpvsDyn.dynEntClientWordCount, asset->dpvsDyn.dynEntClientWordCount, sizeof(asset->dpvsDyn.dynEntClientWordCount));
			std::memcpy(new_asset->dpvsDyn.dynEntClientCount, asset->dpvsDyn.dynEntClientCount, sizeof(asset->dpvsDyn.dynEntClientCount));
			new_asset->dpvsDyn.dynEntClientCount[0] += 64; // reserved dynents
			new_asset->dpvsDyn.dynEntClientWordCount[0] += 2;

			for (auto list = 0; list < 2; list++)
			{
				new_asset->dpvsDyn.dynEntCellBits[list] = allocator.allocate_array<unsigned int>(
					static_cast<std::size_t>(new_asset->dpvsDyn.dynEntClientCount[list]) * new_asset->dpvsPlanes.cellCount);
				for (auto i = 0; i < 3; i++)
				{
					new_asset->dpvsDyn.dynEntVisData[list][i] = allocator.allocate_array<unsigned char>(
						32ull * new_asset->dpvsDyn.dynEntClientWordCount[list]);
				}
			}

			new_asset->mapVtxChecksum = asset->mapVtxChecksum;
			new_asset->heroOnlyLightCount = 0;

			new_asset->umbraTomeData = generate_draw_everything_umbra_tome(new_asset->umbraTomeSize, allocator);
			new_asset->umbraTomePtr = nullptr;

			convert_runtime_arrays(new_asset, allocator);
			return new_asset;
		}

		void dump(source::GfxWorld* asset)
		{
			utils::memory::allocator allocator;
			const auto converted = convert(asset, allocator);
			zonetool::iw7::gfx_world::dump(converted);

			for (unsigned int i = 0; i < converted->draw.lightMapCount; i++)
			{
				zonetool::iw7::gfx_light_map::dump(converted->draw.lightMaps[i]);
			}

			zonetool::iw7::gfx_world_tr::dump(converted->draw.transientZones[0]);
		}
	}
}
