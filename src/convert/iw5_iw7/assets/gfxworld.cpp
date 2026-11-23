#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "comworld.hpp"
#include "gfxworld_umbra.hpp"

#include <DirectXTex.h>
#include <iw7/assets/gfxworld.hpp>
#include <iw7/assets/gfxlightmap.hpp>
#include <iw7/assets/gfxworld_tr.hpp>

#include "lightgrid/probes.hpp"
#include "lightgrid/sh.hpp"
#include "lightgrid/tree.hpp"

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
				constexpr unsigned short neutral_grey = 0x3400;

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
							pixels[i] = 0x3C00;
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

			target::GfxImage* generate_ies_lookup(utils::memory::allocator& allocator)
			{
				return image_reference("*ieslookup", allocator);
			}

			unsigned int first_reflection_probe(const unsigned int reflection_probe_count)
			{
				return reflection_probe_count > 1 ? 1 : 0;
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
				const auto instance_count = probe_count + 1;

				auto& data = new_asset->draw.reflectionProbeData;
				data.reflectionProbeCount = probe_count;
				data.sharedReflectionProbeCount = 0;
				data.reflectionProbes = allocator.allocate_array<target::GfxReflectionProbe>(probe_count);
				data.reflectionProbeArrayImage = generate_reflection_probe_array_image(probe_count, allocator);
				data.reflectionProbeLightgridSampleData = allocator.allocate_array<target::GfxReflectionProbeSampleData>(probe_count);

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
					instance.priority = -(half[0] + half[1] + half[2]);
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

					new_vertex.color.array[0] = vertex.color.array[2];
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
				grid.probeData.zones = allocator.allocate_array<target::GfxGpuLightGridZone>(1);
			}

			void convert_voxel_trees(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				new_asset->voxelTreeCount = new_asset->skyCount;
				new_asset->voxelTree = allocator.allocate_array<target::GfxVoxelTree>(new_asset->voxelTreeCount);

				for (auto i = 0; i < new_asset->skyCount; i++)
				{
					auto& tree = new_asset->voxelTree[i];

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

				new_dpvs.lodData = allocator.allocate_array<unsigned int>(new_dpvs.smodelCount + 1);
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
					new_instance.primaryLightEnvIndex = 0;
					new_instance.reflectionProbeIndex = 0;
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

				new_dpvs.surfaceMaterials = allocator.allocate_array<target::GfxDrawSurf>(asset->surfaceCount);

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

				new_asset->entityMotionBitsEntries = 134;
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

			constexpr unsigned int smodel_probe_samples = 2;

			bool is_sun_light(const unsigned int light, const unsigned int last_sun)
			{
				return (last_sun != 0 && light >= 1 && light <= last_sun) || light >= 256 - last_sun;
			}

			std::uint64_t grid_key(const int x, const int y, const int z)
			{
				return (static_cast<std::uint64_t>(x) << 32) | (static_cast<std::uint64_t>(y) << 16) | static_cast<std::uint64_t>(z);
			}

			struct reach_light
			{
				unsigned short index;
				bool directional;
				bool is_spot;
				float origin[3];
				float axis[3];
				float radius;
				float half_fov;
			};

			std::vector<reach_light> collect_lights(const unsigned int light_count, const bool directional, const bool spot_only, const unsigned int first)
			{
				const auto& lights = comworld::last_primary_lights();
				const auto count = std::min<std::size_t>(light_count, lights.size());

				std::vector<reach_light> result;
				for (std::size_t i = first; i < count && i <= 0xFFFF; i++)
				{
					const auto& source_light = lights[i];
					reach_light light{};
					light.index = static_cast<unsigned short>(i);

					if (source_light.type == target::GFX_LIGHT_TYPE_DIR)
					{
						if (directional)
						{
							light.directional = true;
							result.push_back(light);
						}
						continue;
					}

					if (source_light.type != target::GFX_LIGHT_TYPE_SPOT && source_light.type != target::GFX_LIGHT_TYPE_OMNI)
					{
						continue;
					}
					if (spot_only && source_light.type != target::GFX_LIGHT_TYPE_SPOT)
					{
						continue;
					}
					if (!(source_light.radius > 0.0f))
					{
						continue;
					}

					light.is_spot = source_light.type == target::GFX_LIGHT_TYPE_SPOT;
					std::memcpy(light.origin, source_light.origin, sizeof(light.origin));
					light.radius = source_light.radius;

					const float axis[3] = {-source_light.dir[0], -source_light.dir[1], -source_light.dir[2]};
					const auto length = std::sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
					if (length > 0.0f)
					{
						for (auto k = 0; k < 3; k++)
						{
							light.axis[k] = axis[k] / length;
						}
					}
					else
					{
						light.axis[2] = -1.0f;
						light.is_spot = false;
					}

					light.half_fov = std::acos(std::max(-1.0f, std::min(1.0f, source_light.cos_half_fov_outer)));
					result.push_back(light);
				}

				return result;
			}

			bool light_touches(const reach_light& light, const float* centre, const float extent, float* distance_out = nullptr)
			{
				if (light.directional)
				{
					return true;
				}

				const float delta[3] = {centre[0] - light.origin[0], centre[1] - light.origin[1], centre[2] - light.origin[2]};
				const auto distance = std::sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
				if (distance_out)
				{
					*distance_out = distance;
				}

				if (distance - extent > light.radius)
				{
					return false;
				}

				if (light.is_spot && distance > 0.001f)
				{
					const auto dot = (delta[0] * light.axis[0] + delta[1] * light.axis[1] + delta[2] * light.axis[2]) / distance;
					const auto theta = std::acos(std::max(-1.0f, std::min(1.0f, dot)));
					const auto slack = std::asin(std::min(1.0f, extent / distance));
					if (theta > light.half_fov + slack)
					{
						return false;
					}
				}

				return true;
			}

			float bounds_extent(const source::Bounds& bounds)
			{
				return std::sqrt(bounds.halfSize[0] * bounds.halfSize[0] + bounds.halfSize[1] * bounds.halfSize[1] + bounds.halfSize[2] * bounds.halfSize[2]);
			}

			struct grid_occupancy
			{
				std::unordered_set<std::uint64_t> open_cells;

				bool open(const float* point) const
				{
					const auto x = static_cast<int>(std::floor(point[0] / 32.0f)) + 4096;
					const auto y = static_cast<int>(std::floor(point[1] / 32.0f)) + 4096;
					const auto z = static_cast<int>(std::floor(point[2] / 64.0f)) + 2048;
					if (x < 0 || x > 0xFFFF || y < 0 || y > 0xFFFF || z < 0 || z > 0xFFFF)
					{
						return false;
					}
					return open_cells.contains((static_cast<std::uint64_t>(x) << 32) | (static_cast<std::uint64_t>(y) << 16) | static_cast<std::uint64_t>(z));
				}

				bool reaches(const float* from, const float* to) const
				{
					constexpr float step = 16.0f; 
					constexpr float slack = 48.0f;

					if (open_cells.empty())
					{
						return true;
					}

					float direction[3] = {to[0] - from[0], to[1] - from[1], to[2] - from[2]};
					const auto distance = std::sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
					if (distance <= slack)
					{
						return true;
					}
					for (auto k = 0; k < 3; k++)
					{
						direction[k] /= distance;
					}

					const auto stop = distance - slack;
					for (auto t = step; t < stop; t += step)
					{
						const float point[3] = {from[0] + direction[0] * t, from[1] + direction[1] * t, from[2] + direction[2] * t};
						if (!open(point))
						{
							return false;
						}
					}
					return true;
				}
			};

			grid_occupancy collect_occupancy(source::GfxWorld* asset)
			{
				grid_occupancy occupancy;
				const auto& grid = asset->lightGrid;
				if (!grid.rowDataStart.value || !grid.rawRowData.value)
				{
					return occupancy;
				}

				const auto references = lightgrid_tree::enumerate_row_data(grid.mins, grid.maxs,
					grid.rowAxis, grid.colAxis, grid.rowDataStart.get(), grid.rawRowData.get());
				for (const auto& reference : references)
				{
					occupancy.open_cells.insert(grid_key(reference.pos[0], reference.pos[1], reference.pos[2]));
				}
				return occupancy;
			}

			void convert_light_lists(source::GfxWorld* asset, target::GfxWorld* new_asset, const grid_occupancy& occupancy, utils::memory::allocator& allocator)
			{
				constexpr std::size_t max_per_object = 24;

				const auto receivers = collect_lights(new_asset->primaryLightCount, true, false, 0);
				if (receivers.empty())
				{
					return;
				}

				std::vector<unsigned short> hits;
				const auto list_for = [&](const source::Bounds& bounds)
				{
					hits.clear();
					const auto extent = bounds_extent(bounds);
					for (const auto& light : receivers)
					{
						if (!light_touches(light, bounds.midPoint, extent))
						{
							continue;
						}
						if (!light.directional && !occupancy.reaches(light.origin, bounds.midPoint))
						{
							continue;
						}
						hits.push_back(light.index);
					}
				};

				std::vector<unsigned short> lists{0};
				std::map<std::vector<unsigned short>, unsigned int> list_offset;
				std::size_t clamped = 0;

				const auto intern = [&]() -> unsigned int
				{
					if (hits.size() > max_per_object)
					{
						hits.resize(max_per_object);
						clamped++;
					}
					if (hits.empty())
					{
						return 0;
					}

					const auto found = list_offset.find(hits);
					if (found != list_offset.end())
					{
						return found->second;
					}

					const auto offset = static_cast<unsigned int>(lists.size());
					lists.push_back(static_cast<unsigned short>(hits.size()));
					lists.insert(lists.end(), hits.begin(), hits.end());
					list_offset.emplace(hits, offset);
					return offset;
				};

				const auto surface_count = std::min(asset->dpvs.staticSurfaceCount, asset->surfaceCount);
				const auto smodel_count = asset->dpvs.smodelCount;

				auto& light_lists = new_asset->lightLists;
				light_lists.surfaceListOffsetCount = surface_count;
				light_lists.smodelListOffsetCount = smodel_count;
				light_lists.surfaceListOffsets = allocator.allocate_array<unsigned int>(std::max(surface_count, 1u));
				light_lists.smodelListOffsets = allocator.allocate_array<unsigned int>(std::max(smodel_count, 1u));

				std::size_t lit_surfaces = 0;
				std::size_t lit_smodels = 0;
				for (auto i = 0u; asset->dpvs.surfacesBounds && i < surface_count; i++)
				{
					list_for(asset->dpvs.surfacesBounds[i].bounds);
					light_lists.surfaceListOffsets[i] = intern();
					lit_surfaces += light_lists.surfaceListOffsets[i] != 0;
				}
				for (auto i = 0u; asset->dpvs.smodelInsts && i < smodel_count; i++)
				{
					list_for(asset->dpvs.smodelInsts[i].bounds);
					light_lists.smodelListOffsets[i] = intern();
					lit_smodels += light_lists.smodelListOffsets[i] != 0;
				}

				light_lists.listsSize = static_cast<unsigned int>(lists.size());
				light_lists.lists = allocator.allocate_array<unsigned short>(lists.size());
				std::memcpy(light_lists.lists, lists.data(), sizeof(unsigned short) * lists.size());

				ZONETOOL_INFO("GfxWorld \"%s\": light lists for %zu/%u surfaces and %zu/%u static models, %zu distinct lists",
					asset->name.get(), lit_surfaces, surface_count, lit_smodels, smodel_count, list_offset.size());
				if (clamped)
				{
					ZONETOOL_WARNING("GfxWorld \"%s\": %zu light lists clamped to %zu lights", asset->name.get(), clamped, max_per_object);
				}
			}

			void convert_shadow_casters(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				new_asset->shadowGeomOptimized = allocator.allocate_array<target::GfxShadowGeometry>(std::max(new_asset->primaryLightCount, 1u));

				const auto lights = collect_lights(new_asset->primaryLightCount, false, false, new_asset->lastSunPrimaryLightIndex + 1);
				std::vector<unsigned int> surfaces;
				std::vector<unsigned short> smodels;
				for (const auto& light : lights)
				{
					surfaces.clear();
					smodels.clear();

					for (auto i = 0u; asset->dpvs.surfacesBounds && i < asset->surfaceCount && surfaces.size() < 0xFFFF; i++)
					{
						const auto& bounds = asset->dpvs.surfacesBounds[i].bounds;
						if (light_touches(light, bounds.midPoint, bounds_extent(bounds)))
						{
							surfaces.push_back(i);
						}
					}
					for (auto i = 0u; asset->dpvs.smodelInsts && i < asset->dpvs.smodelCount && i <= 0xFFFF && smodels.size() < 0xFFFF; i++)
					{
						const auto& bounds = asset->dpvs.smodelInsts[i].bounds;
						if (light_touches(light, bounds.midPoint, bounds_extent(bounds)))
						{
							smodels.push_back(static_cast<unsigned short>(i));
						}
					}

					auto& casters = new_asset->shadowGeomOptimized[light.index];
					casters.surfaceCount = static_cast<unsigned short>(surfaces.size());
					casters.smodelCount = static_cast<unsigned short>(smodels.size());
					if (casters.surfaceCount)
					{
						casters.sortedSurfIndex = allocator.allocate_array<unsigned int>(casters.surfaceCount);
						std::memcpy(casters.sortedSurfIndex, surfaces.data(), sizeof(unsigned int) * casters.surfaceCount);
					}
					if (casters.smodelCount)
					{
						casters.smodelIndex = allocator.allocate_array<unsigned short>(casters.smodelCount);
						std::memcpy(casters.smodelIndex, smodels.data(), sizeof(unsigned short) * casters.smodelCount);
					}
				}

				ZONETOOL_INFO("GfxWorld \"%s\": shadow casters listed for %zu local lights", asset->name.get(), lights.size());
			}

			void convert_voxel_light_lists(source::GfxWorld* asset, lightgrid_probes::probe_volume& volume, const grid_occupancy& occupancy)
			{
				constexpr unsigned short empty_address = 2;
				constexpr std::size_t max_per_leaf = 16;

				if (!volume.leaf_count || !(volume.leaf_size > 0.0f) || volume.leaf_bounds_min.size() < static_cast<std::size_t>(volume.leaf_count) * 3)
				{
					return;
				}

				const auto lights = collect_lights(static_cast<unsigned int>(comworld::last_primary_lights().size()), false, true, 0);

				volume.leaf_nodes.assign(volume.leaf_count, empty_address);
				volume.light_list = {16384, 1, 0};

				std::map<std::vector<unsigned short>, unsigned short> list_address;
				std::vector<std::pair<float, unsigned short>> ranked;
				std::vector<unsigned short> hits;
				unsigned int lit_leaves = 0;
				unsigned int clamped_leaves = 0;

				const auto half = volume.leaf_size * 0.5f;
				const auto half_diagonal = half * 1.7320508f;
				const auto inset = volume.leaf_size * 0.25f;

				for (auto leaf = 0u; leaf < volume.leaf_count && !lights.empty(); leaf++)
				{
					const auto* low = &volume.leaf_bounds_min[static_cast<std::size_t>(leaf) * 3];
					const float centre[3] = {low[0] + half, low[1] + half, low[2] + half};

					ranked.clear();
					for (const auto& light : lights)
					{
						auto distance = 0.0f;
						if (!light_touches(light, centre, half_diagonal, &distance))
						{
							continue;
						}

						auto seen = false;
						for (auto sample = 0; sample < 9 && !seen; sample++)
						{
							float point[3];
							if (sample == 0)
							{
								std::memcpy(point, centre, sizeof(point));
							}
							else
							{
								const auto corner = sample - 1;
								point[0] = low[0] + ((corner & 1) ? volume.leaf_size - inset : inset);
								point[1] = low[1] + ((corner & 2) ? volume.leaf_size - inset : inset);
								point[2] = low[2] + ((corner & 4) ? volume.leaf_size - inset : inset);
								if (!occupancy.open_cells.empty() && !occupancy.open(point))
								{
									continue;
								}
							}
							seen = occupancy.reaches(point, light.origin);
						}
						if (seen)
						{
							ranked.emplace_back(distance, light.index);
						}
					}

					if (ranked.empty())
					{
						continue;
					}

					std::sort(ranked.begin(), ranked.end());
					if (ranked.size() > max_per_leaf)
					{
						ranked.resize(max_per_leaf);
						clamped_leaves++;
					}

					hits.clear();
					for (const auto& entry : ranked)
					{
						hits.push_back(entry.second);
					}
					std::sort(hits.begin(), hits.end());

					const auto found = list_address.find(hits);
					if (found != list_address.end())
					{
						volume.leaf_nodes[leaf] = found->second;
						lit_leaves++;
						continue;
					}

					if (volume.light_list.size() + hits.size() + 1 > 0xFFFF)
					{
						continue;
					}

					const auto address = static_cast<unsigned short>(volume.light_list.size());
					volume.light_list.push_back(static_cast<unsigned short>(hits.size() << 7));
					volume.light_list.insert(volume.light_list.end(), hits.begin(), hits.end());
					list_address.emplace(hits, address);
					volume.leaf_nodes[leaf] = address;
					lit_leaves++;
				}

				ZONETOOL_INFO("GfxWorld \"%s\": voxel light lists for %u/%u leaves from %zu spot lights, %zu distinct lists",
					asset->name.get(), lit_leaves, volume.leaf_count, lights.size(), list_address.size());
				if (clamped_leaves)
				{
					ZONETOOL_WARNING("GfxWorld \"%s\": %u leaves clamped to %zu lights", asset->name.get(), clamped_leaves, max_per_leaf);
				}
			}

			void convert_probe_volume(source::GfxWorld* asset, target::GfxWorld* new_asset, utils::memory::allocator& allocator)
			{
				const auto& source_grid = asset->lightGrid;
				const auto* entries = source_grid.entries.value ? source_grid.entries.get() : nullptr;
				const auto* colors = source_grid.colors.value ? source_grid.colors.get() : nullptr;

				constexpr float sh_scale = 1.0f;

				float ambient[3] = {0.0f, 0.0f, 0.0f};
				if (colors && source_grid.colorCount)
				{
					std::vector<double> usage(source_grid.colorCount, 0.0);
					double total_usage = 0.0;
					for (unsigned int i = 0; entries && i < source_grid.entryCount; i++)
					{
						if (entries[i].colorsIndex < source_grid.colorCount)
						{
							usage[entries[i].colorsIndex] += 1.0;
							total_usage += 1.0;
						}
					}

					if (total_usage == 0.0)
					{
						std::fill(usage.begin(), usage.end(), 1.0);
						total_usage = static_cast<double>(source_grid.colorCount);
					}

					double accumulated[3] = {0.0, 0.0, 0.0};
					float hdr_colors[56][3];
					for (unsigned int i = 0; i < source_grid.colorCount; i++)
					{
						if (usage[i] == 0.0)
						{
							continue;
						}

						lightgrid_sh::ldr_colors_to_hdr(colors[i].rgb, hdr_colors);
						for (auto c = 0; c < 3; c++)
						{
							double sum = 0.0;
							for (auto j = 0; j < 56; j++)
							{
								sum += hdr_colors[j][c];
							}
							accumulated[c] += (sum / 56.0) * usage[i];
						}
					}

					for (auto c = 0; c < 3; c++)
					{
						ambient[c] = static_cast<float>(accumulated[c] / total_usage);
					}
				}

				lightgrid_probes::build_params params{};
				for (auto i = 0; i < 3; i++)
				{
					params.bounds_min[i] = asset->bounds.midPoint[i] - asset->bounds.halfSize[i];
					params.bounds_max[i] = asset->bounds.midPoint[i] + asset->bounds.halfSize[i];
				}

				using probe_sh = std::array<float, 28>;
				std::unordered_map<std::uint64_t, probe_sh> grid_samples;
				if (entries && colors && source_grid.rowDataStart.value && source_grid.rawRowData.value)
				{
					std::vector<probe_sh> colour_cache(source_grid.colorCount, probe_sh{});
					std::vector<char> colour_cached(source_grid.colorCount, 0);

					const auto references = lightgrid_tree::enumerate_row_data(source_grid.mins, source_grid.maxs,
						source_grid.rowAxis, source_grid.colAxis, source_grid.rowDataStart.get(), source_grid.rawRowData.get());

					float bins[56][3];
					for (const auto& reference : references)
					{
						if (reference.entry_index >= source_grid.entryCount)
						{
							continue;
						}

						const auto& entry = entries[reference.entry_index];
						if (entry.colorsIndex >= source_grid.colorCount)
						{
							continue;
						}

						if (!colour_cached[entry.colorsIndex])
						{
							lightgrid_sh::ldr_colors_to_hdr(colors[entry.colorsIndex].rgb, bins);
							lightgrid_probes::project_sh(bins, lightgrid_sh::grid_basis_dirs, 56, sh_scale, colour_cache[entry.colorsIndex].data());
							colour_cached[entry.colorsIndex] = 1;
						}

						auto cell = colour_cache[entry.colorsIndex];
						cell[27] = is_sun_light(entry.primaryLightIndex, source_grid.lastSunPrimaryLightIndex) ? 1.0f : 0.0f;
						grid_samples[grid_key(reference.pos[0], reference.pos[1], reference.pos[2])] = cell;
					}
				}

				ZONETOOL_INFO("GfxWorld \"%s\": %zu light grid cells for probe sampling", asset->name.get(), grid_samples.size());

				const auto sample = [&](const float* position, float* out_sh)
				{
					const auto x = static_cast<int>(std::floor(position[0] / 32.0f)) + 4096;
					const auto y = static_cast<int>(std::floor(position[1] / 32.0f)) + 4096;
					const auto z = static_cast<int>(std::floor(position[2] / 64.0f)) + 2048;

					const float* best = nullptr;
					auto best_distance = 0x7FFFFFFF;
					for (auto dz = -1; dz <= 1; dz++)
					{
						for (auto dy = -1; dy <= 1; dy++)
						{
							for (auto dx = -1; dx <= 1; dx++)
							{
								if (x + dx < 0 || y + dy < 0 || z + dz < 0 || x + dx > 0xFFFF || y + dy > 0xFFFF || z + dz > 0xFFFF)
								{
									continue;
								}

								const auto found = grid_samples.find(grid_key(x + dx, y + dy, z + dz));
								if (found == grid_samples.end())
								{
									continue;
								}

								const auto distance = dx * dx + dy * dy + 4 * dz * dz;
								if (distance < best_distance)
								{
									best_distance = distance;
									best = found->second.data();
								}
							}
						}
					}

					if (best)
					{
						std::memcpy(out_sh, best, sizeof(float) * 28);
						return;
					}

					lightgrid_probes::constant_sh(ambient, sh_scale, out_sh);
					out_sh[27] = 1.0f;
				};

				if (!grid_samples.empty())
				{
					params.cell_occupied = [&grid_samples](const float* low, const float size)
					{
						const auto low_x = static_cast<int>(std::floor(low[0] / 32.0f)) + 4096 - 1;
						const auto high_x = static_cast<int>(std::floor((low[0] + size) / 32.0f)) + 4096 + 1;
						const auto low_y = static_cast<int>(std::floor(low[1] / 32.0f)) + 4096 - 1;
						const auto high_y = static_cast<int>(std::floor((low[1] + size) / 32.0f)) + 4096 + 1;
						const auto low_z = static_cast<int>(std::floor(low[2] / 64.0f)) + 2048 - 1;
						const auto high_z = static_cast<int>(std::floor((low[2] + size) / 64.0f)) + 2048 + 1;

						for (auto z = std::max(low_z, 0); z <= std::min(high_z, 0xFFFF); z++)
						{
							for (auto y = std::max(low_y, 0); y <= std::min(high_y, 0xFFFF); y++)
							{
								for (auto x = std::max(low_x, 0); x <= std::min(high_x, 0xFFFF); x++)
								{
									if (grid_samples.contains(grid_key(x, y, z)))
									{
										return true;
									}
								}
							}
						}
						return false;
					};
				}

				auto volume = lightgrid_probes::build(params, sample, sh_scale);

				auto& probe_data = new_asset->lightGrid.probeData;
				auto& zone = probe_data.zones[0];

				if (!volume.valid)
				{
					ZONETOOL_WARNING("GfxWorld \"%s\": light grid probe volume could not be built, using a flat ambient probe", asset->name.get());

					float fallback_sh[28]{};
					lightgrid_probes::constant_sh(ambient, sh_scale, fallback_sh);
					fallback_sh[27] = 1.0f;
					unsigned short fallback[32];
					lightgrid_probes::encode_probe_sh(fallback_sh, fallback);
					std::memcpy(zone.fallbackProbeData.coeffs, fallback, sizeof(zone.fallbackProbeData.coeffs));
					return;
				}

				ZONETOOL_INFO("GfxWorld \"%s\": probe volume of %u probes, %u tetrahedrons, %u leaves",
					asset->name.get(), volume.probe_count, volume.tetrahedron_count, volume.leaf_count);

				{
					grid_occupancy occupancy;
					for (const auto& cell : grid_samples)
					{
						occupancy.open_cells.insert(cell.first);
					}
					convert_voxel_light_lists(asset, volume, occupancy);
				}

				const auto smodel_count = asset->dpvs.smodelCount;
				const auto sample_count = smodel_count * smodel_probe_samples;
				probe_data.gpuVisibleProbesCount = sample_count;
				probe_data.gpuVisibleProbePositions = allocator.allocate_array<target::GfxGpuLightGridProbePosition>(std::max(sample_count, 1u));
				probe_data.gpuVisibleProbesData = allocator.allocate_array<target::GfxSHProbeData>(sample_count + 0x2000);

				for (unsigned int i = 0; i < smodel_count; i++)
				{
					float position[3];
					std::memcpy(position, asset->dpvs.smodelInsts[i].lightingOrigin, sizeof(position));

					float sh[28]{};
					sample(position, sh);
					unsigned short coefficients[32];
					lightgrid_probes::encode_probe_sh(sh, coefficients);

					for (unsigned int k = 0; k < smodel_probe_samples; k++)
					{
						const auto slot = i * smodel_probe_samples + k;
						std::memcpy(probe_data.gpuVisibleProbePositions[slot].origin, position, sizeof(position));
						std::memcpy(&probe_data.gpuVisibleProbesData[slot], coefficients, sizeof(coefficients));
					}

					auto& instance = new_asset->dpvs.smodelDrawInsts[i];
					instance.firstProbe = i * smodel_probe_samples;
					instance.unk2 = 0;
					instance.probeCount = static_cast<unsigned short>(smodel_probe_samples);
				}

				static_assert(sizeof(target::GfxSHProbeData) == sizeof(unsigned short) * 32);
				probe_data.probeCount = volume.probe_count;
				probe_data.probes = allocator.allocate_array<target::GfxSHProbeData>(volume.probe_count);
				std::memcpy(probe_data.probes, volume.probes.data(), sizeof(unsigned short) * volume.probes.size());
				probe_data.probePositions = allocator.allocate_array<target::GfxGpuLightGridProbePosition>(volume.probe_count);
				std::memcpy(probe_data.probePositions, volume.probe_positions.data(), sizeof(float) * volume.probe_positions.size());

				probe_data.tetrahedronCount = volume.tetrahedron_count;
				probe_data.tetrahedrons = allocator.allocate_array<target::GfxGpuLightGridTetrahedron>(volume.tetrahedron_count);
				std::memcpy(probe_data.tetrahedrons, volume.tetrahedrons.data(), sizeof(unsigned int) * volume.tetrahedrons.size());
				probe_data.tetrahedronNeighbors = allocator.allocate_array<target::GfxGpuLightGridTetrahedronNeighbors>(volume.tetrahedron_count);
				std::memcpy(probe_data.tetrahedronNeighbors, volume.tetrahedron_neighbors.data(), sizeof(unsigned int) * volume.tetrahedron_neighbors.size());

				unsigned int visible_tetrahedrons = 0;
				for (unsigned int t = 0; t < volume.tetrahedron_count; t++)
				{
					for (auto k = 0; k < 4; k++)
					{
						if (volume.tetrahedrons[t * 4 + k] & 0x80000000u)
						{
							visible_tetrahedrons++;
							break;
						}
					}
				}

				probe_data.tetrahedronCountVisible = visible_tetrahedrons;
				probe_data.tetrahedronVisibility = nullptr;
				if (visible_tetrahedrons)
				{
					probe_data.tetrahedronVisibility = allocator.allocate_array<target::GfxGpuLightGridTetrahedronVisibility>(visible_tetrahedrons);
					std::memset(probe_data.tetrahedronVisibility, 0xFF, sizeof(target::GfxGpuLightGridTetrahedronVisibility) * visible_tetrahedrons);
				}

				probe_data.voxelStartTetrahedronCount = static_cast<unsigned int>(volume.voxel_start_tetrahedron.size());
				probe_data.voxelStartTetrahedron = allocator.allocate_array<target::GfxGpuLightGridVoxelStartTetrahedron>(probe_data.voxelStartTetrahedronCount);
				std::memcpy(probe_data.voxelStartTetrahedron, volume.voxel_start_tetrahedron.data(), sizeof(unsigned int) * volume.voxel_start_tetrahedron.size());

				zone.numProbes = volume.zone_num_probes;
				zone.firstProbe = volume.zone_first_probe;
				zone.numTetrahedrons = volume.zone_num_tetrahedrons;
				zone.firstTetrahedron = volume.zone_first_tetrahedron;
				zone.firstVoxelTetrahedronIndex = volume.zone_first_voxel_tetrahedron_index;
				zone.numVoxelTetrahedronIndices = volume.zone_num_voxel_tetrahedron_indices;
				std::memcpy(zone.fallbackProbeData.coeffs, volume.zone_fallback_coeffs, sizeof(zone.fallbackProbeData.coeffs));

				new_asset->voxelTreeCount = 1;
				new_asset->voxelTree = allocator.allocate_array<target::GfxVoxelTree>(1);
				auto& tree = new_asset->voxelTree[0];
				for (auto i = 0; i < 3; i++)
				{
					tree.zoneBound.midPoint[i] = (volume.bound_min[i] + volume.bound_max[i]) * 0.5f;
					tree.zoneBound.halfSize[i] = (volume.bound_max[i] - volume.bound_min[i]) * 0.5f;
				}

				tree.voxelTopDownViewNodeCount = static_cast<int>(volume.top_down_view_nodes.size());
				tree.voxelInternalNodeCount = static_cast<int>(volume.internal_nodes.size());
				tree.voxelLeafNodeCount = static_cast<int>(volume.leaf_nodes.size());
				tree.lightListArraySize = static_cast<int>(volume.light_list.size());

				tree.voxelTreeHeader = allocator.allocate<target::GfxVoxelTreeHeader>();
				std::memcpy(tree.voxelTreeHeader->rootNodeDimension, volume.root_node_dimension, sizeof(int[4]));
				std::memcpy(tree.voxelTreeHeader->nodeCoordBitShift, volume.node_coord_bit_shift, sizeof(int[4]));
				std::memcpy(&tree.voxelTreeHeader->boundMin, volume.bound_min, sizeof(float[4]));
				std::memcpy(&tree.voxelTreeHeader->boundMax, volume.bound_max, sizeof(float[4]));

				static_assert(sizeof(target::GfxVoxelTopDownViewNode) == sizeof(lightgrid_probes::top_down_view_node));
				static_assert(sizeof(target::GfxVoxelInternalNode) == sizeof(lightgrid_probes::voxel_tree_node));
				tree.voxelTopDownViewNodeArray = allocator.allocate_array<target::GfxVoxelTopDownViewNode>(tree.voxelTopDownViewNodeCount);
				std::memcpy(tree.voxelTopDownViewNodeArray, volume.top_down_view_nodes.data(), sizeof(target::GfxVoxelTopDownViewNode) * tree.voxelTopDownViewNodeCount);
				tree.voxelInternalNodeArray = allocator.allocate_array<target::GfxVoxelInternalNode>(tree.voxelInternalNodeCount);
				std::memcpy(tree.voxelInternalNodeArray, volume.internal_nodes.data(), sizeof(target::GfxVoxelInternalNode) * tree.voxelInternalNodeCount);
				tree.voxelLeafNodeArray = allocator.allocate_array<target::GfxVoxelLeafNode>(tree.voxelLeafNodeCount);
				std::memcpy(tree.voxelLeafNodeArray, volume.leaf_nodes.data(), sizeof(unsigned short) * volume.leaf_nodes.size());
				tree.lightListArray = allocator.allocate_array<unsigned short>(tree.lightListArraySize);
				std::memcpy(tree.lightListArray, volume.light_list.data(), sizeof(unsigned short) * volume.light_list.size());
				tree.voxelInternalNodeDynamicLightList = allocator.allocate_array<unsigned int>(2 * tree.voxelInternalNodeCount);
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

			convert_shadow_casters(asset, new_asset, allocator);

			convert_light_regions(asset, new_asset, allocator);
			convert_dpvs(asset, new_asset, allocator);
			convert_probe_volume(asset, new_asset, allocator);
			convert_light_lists(asset, new_asset, collect_occupancy(asset), allocator);

			std::memcpy(new_asset->dpvsDyn.dynEntClientWordCount, asset->dpvsDyn.dynEntClientWordCount, sizeof(asset->dpvsDyn.dynEntClientWordCount));
			std::memcpy(new_asset->dpvsDyn.dynEntClientCount, asset->dpvsDyn.dynEntClientCount, sizeof(asset->dpvsDyn.dynEntClientCount));
			new_asset->dpvsDyn.dynEntClientCount[0] += 64;
			new_asset->dpvsDyn.dynEntClientWordCount[0] += 2;
			ZONETOOL_INFO("gfxworld: dynent clients %u + %u (words %u + %u)",
				new_asset->dpvsDyn.dynEntClientCount[0], new_asset->dpvsDyn.dynEntClientCount[1],
				new_asset->dpvsDyn.dynEntClientWordCount[0], new_asset->dpvsDyn.dynEntClientWordCount[1]);

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

			generate_umbra_tome(asset, new_asset, allocator);

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
