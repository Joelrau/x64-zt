#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld_umbra.hpp"
#include "comworld.hpp"

#include <iw3/iw3.hpp>
#include <utils/umbra/tome.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace gfxworld
	{
		namespace
		{
			namespace target = zonetool::iw7;
			namespace source = zonetool::iw5;
			namespace umbra = utils::umbra;

			constexpr auto tome_version_magic = 0xD6000014u;
			constexpr auto lod_base_distance = 128.0f;

			// Umbra's own tree range. Shadow views query from the light, so the empty space around the map needs cells too.
			constexpr auto view_volume_half_extent = 262144.0f;

			// VOLUME counts the box interior, so a box target stays listed while the camera is inside it.
			constexpr auto box_target = umbra::SCENE_OBJECT_TARGET | umbra::SCENE_OBJECT_VOLUME;

			unsigned int tome_crc32(const void* data, const std::size_t size)
			{
				const auto bytes = static_cast<const unsigned char*>(data);
				auto crc = 0xFFFFFFFFu;
				for (std::size_t i = 0; i < size; i++)
				{
					crc ^= bytes[i];
					for (auto bit = 0; bit < 8; bit++)
					{
						crc = (crc & 1u) ? ((crc >> 1) ^ 0x82F63B78u) : (crc >> 1);
					}
				}
				return ~crc;
			}

			// An empty tome outside the world: every query fails and the engine draws everything.
			void draw_everything_tome(target::GfxWorld* world, utils::memory::allocator& allocator)
			{
				static_assert(sizeof(target::Umbra::ImpTome) == 368);
				constexpr auto origin = 200000.0f;
				constexpr auto extent = 64.0f;

				const auto tome = allocator.allocate<target::Umbra::ImpTome>();
				tome->m_versionMagic = tome_version_magic;
				tome->m_size = sizeof(target::Umbra::ImpTome);
				tome->m_lodBaseDistance = lod_base_distance;
				tome->m_flags = 0;
				tome->m_treeMin = {origin, origin, origin};
				tome->m_treeMax = {origin + extent, origin + extent, origin + extent};
				tome->m_boundsMin = tome->m_treeMin;
				tome->m_boundsMax = tome->m_treeMax;
				tome->m_clusterCoordScale = 1.0f;
				tome->m_crc32 = tome_crc32(&tome->m_size, tome->m_size - offsetof(target::Umbra::ImpTome, m_size));

				world->umbraTomeSize = tome->m_size;
				world->umbraTomeData = reinterpret_cast<char*>(tome);
			}

			bool finite3(const float* v)
			{
				return std::isfinite(v[0]) && std::isfinite(v[1]) && std::isfinite(v[2]);
			}

			enum class verdict
			{
				opaque,
				no_material,
				no_state_bits,
				camera_region,
				blend,
				alpha_test,
				no_depth_write,
				polygon_offset,
				count
			};

			const char* verdict_name(const verdict value)
			{
				static const char* names[] =
				{
					"opaque", "no material", "no state bits", "not lit opaque", "blended",
					"alpha tested", "no depth write", "polygon offset",
				};
				return names[static_cast<int>(value)];
			}

			// Surface materials are still CoD4 materials here: the earlier stages pass the pointer through.
			verdict judge_material(const zonetool::iw3::Material* material)
			{
				if (!material || !material->info.name.get())
				{
					return verdict::no_material;
				}

				const auto state_bits = material->stateBitsTable.get();
				if (!state_bits || !material->stateBitsCount)
				{
					return verdict::no_state_bits;
				}

				if (material->cameraRegion != zonetool::iw3::CAMERA_REGION_LIT)
				{
					return verdict::camera_region;
				}

				auto base_passes = 0u;
				for (const auto technique : {zonetool::iw3::TECHNIQUE_UNLIT, zonetool::iw3::TECHNIQUE_EMISSIVE,
					zonetool::iw3::TECHNIQUE_LIT, zonetool::iw3::TECHNIQUE_LIT_SUN, zonetool::iw3::TECHNIQUE_LIT_SUN_SHADOW})
				{
					const auto entry = static_cast<unsigned char>(material->stateBitsEntry[technique]);
					if (entry == 0xFF || entry >= material->stateBitsCount)
					{
						continue;
					}

					base_passes++;
					const auto bits0 = state_bits[entry].loadBits.raw[0];
					const auto bits1 = state_bits[entry].loadBits.raw[1];

					const auto blend_op = (bits0 & zonetool::iw3::GFXS0_BLENDOP_RGB_MASK) >> zonetool::iw3::GFXS0_BLENDOP_RGB_SHIFT;
					const auto source_blend = (bits0 & zonetool::iw3::GFXS0_SRCBLEND_RGB_MASK) >> zonetool::iw3::GFXS0_SRCBLEND_RGB_SHIFT;
					const auto destination_blend = (bits0 & zonetool::iw3::GFXS0_DSTBLEND_RGB_MASK) >> zonetool::iw3::GFXS0_DSTBLEND_RGB_SHIFT;
					if (blend_op != zonetool::iw3::GFXS_BLENDOP_DISABLED
						&& !(source_blend == zonetool::iw3::GFXS_BLEND_ONE && destination_blend == zonetool::iw3::GFXS_BLEND_ZERO))
					{
						return verdict::blend;
					}

					if (!(bits0 & zonetool::iw3::GFXS0_ATEST_DISABLE))
					{
						return verdict::alpha_test;
					}

					if (!(bits1 & zonetool::iw3::GFXS1_DEPTHWRITE))
					{
						return verdict::no_depth_write;
					}

					if (bits1 & zonetool::iw3::GFXS1_POLYGON_OFFSET_MASK)
					{
						return verdict::polygon_offset;
					}
				}

				return base_passes ? verdict::opaque : verdict::no_state_bits;
			}

			bool build_surface_model(source::GfxWorld* asset, const unsigned int surface_index, umbra::tome_model& model)
			{
				const auto& tris = asset->dpvs.surfaces[surface_index].tris;
				const auto vertices = asset->draw.vd.vertices.get();
				const auto indices = asset->draw.indices.get();
				if (!tris.triCount || !tris.vertexCount || !vertices || !indices)
				{
					return false;
				}

				if (tris.firstVertex > asset->draw.vertexCount
					|| tris.vertexCount > asset->draw.vertexCount - tris.firstVertex
					|| tris.baseIndex > asset->draw.indexCount
					|| tris.triCount * 3u > asset->draw.indexCount - tris.baseIndex)
				{
					return false;
				}

				std::vector<int> remap(tris.vertexCount, -1);
				model.indices.reserve(static_cast<std::size_t>(tris.triCount) * 3);
				for (auto i = 0u; i < tris.triCount * 3u; i++)
				{
					const auto index = indices[tris.baseIndex + i];
					if (index >= tris.vertexCount)
					{
						return false;
					}

					if (remap[index] == -1)
					{
						const auto xyz = vertices[tris.firstVertex + index].xyz;
						if (!finite3(xyz))
						{
							return false;
						}

						remap[index] = static_cast<int>(model.vertices.size() / 3);
						model.vertices.insert(model.vertices.end(), xyz, xyz + 3);
					}

					model.indices.push_back(static_cast<unsigned int>(remap[index]));
				}

				return true;
			}

			struct scene
			{
				umbra::tome_input input;
				unsigned int view_box_model = 0;
				unsigned int occluders = 0;
				unsigned int surfaces = 0;
				unsigned int sky = 0;
				unsigned int without_geometry = 0;
				unsigned int triangles = 0;
				unsigned int smodels = 0;
				unsigned int bounded_lights = 0;
				unsigned int never_culled = 0;
				unsigned int verdicts[static_cast<int>(verdict::count)]{};
				std::string examples[static_cast<int>(verdict::count)];
			};

			bool gather_scene(source::GfxWorld* asset, target::GfxWorld* world, scene& out)
			{
				auto& input = out.input;
				input.name = world->baseName ? world->baseName : "world";

				if (!asset->dpvs.surfaces.get() || !world->dpvs.sortedSurfIndex)
				{
					return false;
				}

				std::unordered_map<unsigned int, unsigned int> sorted_position;
				sorted_position.reserve(world->dpvs.staticSurfaceCount);
				for (auto sorted = 0u; sorted < world->dpvs.staticSurfaceCount; sorted++)
				{
					sorted_position.emplace(world->dpvs.sortedSurfIndex[sorted], sorted);
				}

				// Both readings of skyStartSurfs are kept: a wrong extra entry only stays unculled.
				std::unordered_set<unsigned int> sky_surfaces;
				for (auto i = 0; i < asset->skyCount; i++)
				{
					const auto& sky = asset->skies[i];
					for (auto j = 0; j < sky.skySurfCount; j++)
					{
						const auto value = static_cast<unsigned int>(sky.skyStartSurfs[j]);
						sky_surfaces.insert(value);
						if (value < world->dpvs.staticSurfaceCount)
						{
							sky_surfaces.insert(world->dpvs.sortedSurfIndex[value]);
						}
					}
				}

				umbra::scene_view_volume volume{};
				for (auto axis = 0; axis < 3; axis++)
				{
					volume.mins[axis] = -view_volume_half_extent;
					volume.maxs[axis] = view_volume_half_extent;
				}
				input.view_volumes.push_back(volume);
				out.view_box_model = input.add_box_model(volume.mins, volume.maxs);

				for (auto surface = 0u; surface < asset->surfaceCount; surface++)
				{
					const auto sorted = sorted_position.find(surface);
					if (sorted == sorted_position.end())
					{
						continue;
					}

					const auto user_id = umbra::USER_ID_SURFACE | (sorted->second & umbra::USER_ID_INDEX_MASK);
					if (sky_surfaces.contains(surface))
					{
						input.objects.push_back({out.view_box_model, user_id, box_target});
						out.sky++;
						continue;
					}

					umbra::tome_model model;
					if (!build_surface_model(asset, surface, model))
					{
						input.objects.push_back({out.view_box_model, user_id, box_target});
						out.without_geometry++;
						continue;
					}

					out.triangles += static_cast<unsigned int>(model.indices.size() / 3);
					input.models.emplace_back(std::move(model));
					const auto model_index = static_cast<unsigned int>(input.models.size() - 1);

					const auto material = reinterpret_cast<const zonetool::iw3::Material*>(asset->dpvs.surfaces[surface].material.get());
					const auto result = judge_material(material);
					out.verdicts[static_cast<int>(result)]++;
					auto& examples = out.examples[static_cast<int>(result)];
					if (material && material->info.name.get() && examples.size() < 200)
					{
						const std::string name = material->info.name.get();
						if (examples.find(name) == std::string::npos)
						{
							examples += (examples.empty() ? "" : ", ") + name;
						}
					}

					auto flags = static_cast<unsigned int>(umbra::SCENE_OBJECT_TARGET);
					if (result == verdict::opaque)
					{
						flags |= umbra::SCENE_OBJECT_OCCLUDER;
						out.occluders++;
					}

					out.surfaces++;
					input.objects.push_back({model_index, user_id, flags});
				}

				// lodData[i + 1] = i, so static model IDs are one based.
				for (auto i = 0u; i < asset->dpvs.smodelCount && i < umbra::USER_ID_INDEX_MASK; i++)
				{
					const auto& bounds = asset->dpvs.smodelInsts[i].bounds;
					float mins[3]{}, maxs[3]{};
					for (auto axis = 0; axis < 3; axis++)
					{
						mins[axis] = bounds.midPoint[axis] - bounds.halfSize[axis];
						maxs[axis] = bounds.midPoint[axis] + bounds.halfSize[axis];
					}

					if (!finite3(mins) || !finite3(maxs))
					{
						continue;
					}

					input.objects.push_back({input.add_box_model(mins, maxs), umbra::USER_ID_SMODEL | (i + 1), box_target});
					out.smodels++;
				}

				const auto& lights = comworld::last_primary_lights();
				for (auto i = 0u; i < world->primaryLightCount; i++)
				{
					auto model_index = out.view_box_model;
					if (i < lights.size())
					{
						const auto& light = lights[i];
						if ((light.type == target::GFX_LIGHT_TYPE_SPOT || light.type == target::GFX_LIGHT_TYPE_OMNI)
							&& light.radius > 0.0f && std::isfinite(light.radius) && finite3(light.origin))
						{
							float mins[3]{}, maxs[3]{};
							for (auto axis = 0; axis < 3; axis++)
							{
								mins[axis] = light.origin[axis] - light.radius;
								maxs[axis] = light.origin[axis] + light.radius;
							}
							model_index = input.add_box_model(mins, maxs);
							out.bounded_lights++;
						}
					}

					if (model_index == out.view_box_model)
					{
						out.never_culled++;
					}
					input.objects.push_back({model_index, umbra::USER_ID_PRIMARY_LIGHT | i, box_target});
				}

				const auto add_never_culled = [&](const unsigned int type, const unsigned int count)
				{
					for (auto i = 0u; i < count; i++)
					{
						input.objects.push_back({out.view_box_model, type | i, box_target});
						out.never_culled++;
					}
				};
				add_never_culled(umbra::USER_ID_REFLECTION_PROBE, world->draw.reflectionProbeData.reflectionProbeInstanceCount);
				add_never_culled(umbra::USER_ID_VOLUMETRIC, world->draw.volumetrics.volumetricCount);
				add_never_culled(umbra::USER_ID_DECAL, world->draw.decalVolumeCollectionCount);

				return !input.objects.empty();
			}
		}

		void generate_umbra_tome(source::GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->numUmbraGates = 0;
			world->umbraGates = nullptr;
			world->umbraTomePtr = nullptr;

			const auto fall_back = [&](const std::string& reason)
			{
				ZONETOOL_WARNING("umbra: %s, using the draw-everything tome (no occlusion culling)", reason.data());
				draw_everything_tome(world, allocator);
			};

			if (const auto disable = std::getenv("ZT_UMBRA_DISABLE"); disable && *disable && *disable != '0')
			{
				fall_back("ZT_UMBRA_DISABLE is set");
				return;
			}

			if (umbra::find_generator().empty())
			{
				fall_back("umbra-tomegen.exe not found");
				return;
			}

			scene scene;
			if (!gather_scene(asset, world, scene))
			{
				fall_back("nothing to build a tome from");
				return;
			}

			ZONETOOL_INFO("umbra: %s: %u surfaces (%u occluders, %u sky, %u without geometry), %u triangles, %u static models, "
				"%u bounded lights, %u never culled; %zu models, %zu objects",
				scene.input.name.data(), scene.surfaces, scene.occluders, scene.sky, scene.without_geometry, scene.triangles,
				scene.smodels, scene.bounded_lights, scene.never_culled, scene.input.models.size(), scene.input.objects.size());
			for (auto i = 0; i < static_cast<int>(verdict::count); i++)
			{
				if (scene.verdicts[i])
				{
					ZONETOOL_INFO("umbra:   %u surfaces %s (%s)", scene.verdicts[i], verdict_name(static_cast<verdict>(i)), scene.examples[i].data());
				}
			}

			const auto log = [](const std::string& line)
			{
				ZONETOOL_INFO("umbra: %s", line.data());
			};

			// A dropped target could never be marked visible: retry once with the dropped ones as never culled boxes.
			umbra::tome_result result;
			for (auto attempt = 0;; attempt++)
			{
				if (!umbra::generate_tome(scene.input, result, log))
				{
					fall_back(result.error);
					return;
				}

				const auto ids = umbra::read_user_ids(result.data.data(), result.data.size());
				const std::unordered_set<unsigned int> present(ids.begin(), ids.end());

				auto dropped = 0u;
				for (auto& object : scene.input.objects)
				{
					if (!present.contains(object.user_id))
					{
						dropped++;
						object.model = scene.view_box_model;
						object.flags = box_target;
					}
				}

				if (!dropped)
				{
					break;
				}

				ZONETOOL_WARNING("umbra: optimizer dropped %u of %zu targets", dropped, scene.input.objects.size());
				if (attempt > 0)
				{
					fall_back("targets still missing after the retry");
					return;
				}
			}

			const auto& stats = result.stats;
			ZONETOOL_INFO("umbra: tome 0x%X, %u bytes, %d objects (%u user IDs), %d tiles (%d leaf), %u cells (%u without portals), "
				"%u portals, %d clusters, generated in %.1fs",
				stats.version, stats.size, stats.object_count, stats.user_id_count, stats.tile_count, stats.leaf_tile_count,
				stats.cell_count, stats.cells_without_portals, stats.portal_count, stats.cluster_count, result.generation_seconds);

			if (stats.object_count <= 0)
			{
				fall_back("generated tome has no objects");
				return;
			}

			const auto data = allocator.allocate_array<char>(result.data.size());
			std::memcpy(data, result.data.data(), result.data.size());
			world->umbraTomeSize = static_cast<unsigned int>(result.data.size());
			world->umbraTomeData = data;
		}
	}
}
