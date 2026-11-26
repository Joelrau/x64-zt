#include <std_include.hpp>
#include "converter.hpp"

#include "assets/clipmap.hpp"
#include "assets/collisiontile.hpp"
#include "assets/fxworld.hpp"
#include "assets/gfximage.hpp"
#include "assets/gfxworld.hpp"
#include "assets/mapents.hpp"
#include "assets/material.hpp"
#include "assets/netconststrings.hpp"
#include "assets/physics.hpp"
#include "assets/rawfile.hpp"
#include "assets/scriptable.hpp"
#include "assets/scriptfile.hpp"
#include "assets/stterrain.hpp"
#include "assets/vfx.hpp"
#include "assets/xanim.hpp"
#include "assets/xmodel.hpp"
#include "assets/xsurface.hpp"

#include <game.hpp>

namespace zonetool::s4::converter::iw8
{
	namespace
	{
		std::vector<std::string> csv_rows;
		std::filesystem::path target_folder;

		void convert_spatial_tree_node(const unsigned int* source, unsigned int* converted, const unsigned int node, const unsigned int dword_count)
		{
			if (node >= dword_count)
			{
				return;
			}

			const auto& header = reinterpret_cast<const SpatialPartition_TreeNodeHeader&>(source[node]);
			const auto count = header.count;
			auto& converted_header = reinterpret_cast<zonetool::iw8::SpatialPartition_TreeNodeHeader&>(converted[node]);
			converted_header = {};
			converted_header.isLeaf = header.isLeaf;
			converted_header.count = count;

			if (header.isLeaf || node + 1 + count * 7 > dword_count)
			{
				return;
			}

			const auto children = &source[node + 1];
			const auto source_bounds = reinterpret_cast<const float*>(&source[node + 1 + count]);
			const auto converted_bounds = reinterpret_cast<float*>(&converted[node + 1 + count]);
			for (auto axis = 0u; axis < 3; axis++)
			{
				for (auto i = 0u; i < count; i++)
				{
					converted_bounds[axis * count + i] = source_bounds[axis * 2 * count + i];
					converted_bounds[(axis + 3) * count + i] = source_bounds[(axis * 2 + 1) * count + i];
				}
			}

			for (auto i = 0u; i < count; i++)
			{
				if (children[i])
				{
					convert_spatial_tree_node(source, converted, node + children[i], dword_count);
				}
			}
		}
	}

	zonetool::iw8::SpatialPartition_Tree* convert_spatial_tree(const SpatialPartition_Tree* tree, utils::memory::allocator& allocator)
	{
		static_assert(sizeof(zonetool::iw8::SpatialPartition_Tree) == sizeof(SpatialPartition_Tree));
		if (!tree)
		{
			return nullptr;
		}

		const auto converted = allocator.allocate<zonetool::iw8::SpatialPartition_Tree>();
		std::memcpy(converted, tree, sizeof(SpatialPartition_Tree));
		if (!tree->tree || !tree->treeSize)
		{
			return converted;
		}

		converted->tree = allocator.allocate_array<char>(tree->treeSize);
		std::memcpy(converted->tree, tree->tree, tree->treeSize);
		convert_spatial_tree_node(reinterpret_cast<const unsigned int*>(tree->tree), reinterpret_cast<unsigned int*>(converted->tree), 0, tree->treeSize / 4);
		return converted;
	}

	const std::filesystem::path& iw8_folder()
	{
		return target_folder;
	}

	void set_iw8_folder(const std::filesystem::path& folder)
	{
		target_folder = folder;
	}

	void add_csv_row(const std::string_view type, const std::string_view name)
	{
		auto row = std::format("{},{}", type, name);
		if (std::ranges::find(csv_rows, row) == csv_rows.end())
		{
			csv_rows.emplace_back(std::move(row));
		}
	}

	const char* bsp_name(const char* map_name, utils::memory::allocator& allocator)
	{
		const std::string_view name = map_name;
		return allocator.duplicate_string(std::format("maps/{}{}.d3dbsp", name.starts_with("mp_") ? "mp/" : "", name));
	}

	void write_csv(const std::string& zone_name)
	{
		if (csv_rows.empty())
		{
			return;
		}

		std::ranges::stable_sort(csv_rows, {}, [](const std::string& row)
		{
			return row.starts_with("require,") ? 0 : row.contains(",,") ? 1 : 2;
		});

		std::string text;
		for (const auto& row : csv_rows)
		{
			text += row + "\n";
		}

		filesystem::file csv(zone_name + ".csv");
		csv.open("wb");
		csv.write(text);
		csv.close();
		csv_rows.clear();
	}
}

namespace zonetool::convert::s4_iw8
{
	namespace
	{
		template <typename Target, typename Source>
		void dump_identical(void* header, const zonetool::iw8::XAssetType type, void (*dump)(Target*))
		{
			static_assert(sizeof(Source) == sizeof(Target));

			const auto asset = static_cast<Source*>(header);
			utils::memory::allocator allocator;
			const auto converted = allocator.allocate<Target>();
			std::memcpy(converted, asset, sizeof(Target));

			dump(converted);
			zonetool::s4::converter::iw8::add_csv_row(zonetool::iw8::type_to_string(type), asset->name);
		}

		template <typename Target, typename Source>
		void dump_identical_bsp(void* header, const zonetool::iw8::XAssetType type, void (*dump)(Target*))
		{
			static_assert(sizeof(Source) == sizeof(Target));

			utils::memory::allocator allocator;
			const auto converted = allocator.allocate<Target>();
			std::memcpy(converted, header, sizeof(Target));
			converted->name = zonetool::s4::converter::iw8::bsp_name(converted->name, allocator);

			dump(converted);
			zonetool::s4::converter::iw8::add_csv_row(zonetool::iw8::type_to_string(type), converted->name);
		}

		void add_scriptable_dynents(zonetool::iw8::DynEntityList& list, utils::memory::allocator& allocator)
		{
			namespace target = zonetool::iw8;

			const zonetool::s4::MapEnts* map_ents{};
			db_enum_assets(zonetool::s4::ASSET_TYPE_MAP_ENTS, [&](void* header)
			{
				map_ents = static_cast<const zonetool::s4::MapEnts*>(header);
			});

			if (!map_ents || list.index[0] != 0)
			{
				return;
			}

			const auto dynents = zonetool::s4::converter::iw8::mapents::make_scriptable_dynents(map_ents);
			if (dynents.empty())
			{
				return;
			}

			const auto old_count = list.dynEntCount[0];
			const auto count = static_cast<unsigned int>(dynents.size());
			const auto defs = allocator.allocate_array<target::DynEntityDef>(old_count + count);
			if (list.dynEntDefList[0])
			{
				std::memcpy(defs, list.dynEntDefList[0], sizeof(target::DynEntityDef) * old_count);
			}
			std::memcpy(defs + old_count, dynents.data(), sizeof(target::DynEntityDef) * count);
			list.dynEntDefList[0] = defs;
			list.dynEntCount[0] += count;
			list.dynEntCountTotal += count;

			std::map<unsigned int, std::pair<unsigned int, unsigned int>> buckets;
			const auto source_population = list.dynEntSpatialPopulation[1];
			for (auto i = 0u; source_population && i < source_population->partitionCount; i++)
			{
				buckets[source_population->partitionIds[i]] = {source_population->buckets[i].spatialNodeNext, source_population->bucketTails[i].spatialNodeNext};
			}

			for (auto index = old_count; index < old_count + count; index++)
			{
				auto& dynent = defs[index];
				const auto [bucket, inserted] = buckets.try_emplace(dynent.spatialPartitionIndex, index, index);
				dynent.spatialNode.spatialNodeNext = inserted ? 0xFFFFFFFF : bucket->second.first;
				bucket->second.first = index;
			}

			const auto population = allocator.allocate<target::SpatialPartition_TransientPopulation>();
			if (source_population)
			{
				*population = *source_population;
			}

			population->partitionCount = static_cast<unsigned int>(buckets.size());
			population->currentPopulation += count;
			population->buckets = allocator.allocate_array<target::SpatialPartition_Population_Node>(population->partitionCount);
			population->bucketTails = allocator.allocate_array<target::SpatialPartition_Population_Node>(population->partitionCount);
			population->partitionIds = allocator.allocate_array<unsigned int>(population->partitionCount);

			auto i = 0u;
			for (const auto& [partition, chain] : buckets)
			{
				population->partitionIds[i] = partition;
				population->buckets[i].spatialNodeNext = chain.first;
				population->bucketTails[i].spatialNodeNext = chain.second;
				i++;
			}

			list.dynEntSpatialPopulation[1] = population;
		}

		void dump_dyn_entity_list(zonetool::s4::DynEntityList* asset)
		{
			namespace target = zonetool::iw8;
			static_assert(sizeof(target::DynEntityDef) == sizeof(zonetool::s4::DynEntityDef));
			static_assert(sizeof(target::DynEntityList) == sizeof(zonetool::s4::DynEntityList));

			utils::memory::allocator allocator;
			const auto converted = allocator.allocate<target::DynEntityList>();
			std::memcpy(converted, asset, sizeof(target::DynEntityList));
			add_scriptable_dynents(*converted, allocator);

			target::dyn_entity_list::dump(converted);
			zonetool::s4::converter::iw8::add_csv_row(target::type_to_string(target::ASSET_TYPE_DYN_ENTITY_LIST), asset->name);
		}

		void dump_glass_world(const zonetool::s4::GlassWorld* asset)
		{
			static_assert(sizeof(zonetool::iw8::G_GlassData) == sizeof(zonetool::s4::G_GlassData));

			utils::memory::allocator allocator;
			const auto converted = allocator.allocate<zonetool::iw8::GlassWorld>();
			converted->name = zonetool::s4::converter::iw8::bsp_name(asset->name, allocator);
			if (asset->g_glassData)
			{
				converted->g_glassData = allocator.allocate<zonetool::iw8::G_GlassData>();
				std::memcpy(converted->g_glassData, asset->g_glassData, sizeof(zonetool::iw8::G_GlassData));
				converted->g_glassData->spatialTree = zonetool::s4::converter::iw8::convert_spatial_tree(asset->g_glassData->spatialTree, allocator);
			}

			zonetool::iw8::glass_world::dump(converted);
			zonetool::s4::converter::iw8::add_csv_row(zonetool::iw8::type_to_string(zonetool::iw8::ASSET_TYPE_GLASSWORLD), converted->name);
		}

		void dump_decal_volume_mask(const zonetool::s4::GfxDecalVolumeMask* asset)
		{
			utils::memory::allocator allocator;
			const auto converted = allocator.allocate<zonetool::iw8::GfxDecalVolumeMask>();
			converted->name = asset->name;
			converted->blendMap = zonetool::s4::converter::iw8::gfximage::image_reference(asset->blendMap, allocator);

			zonetool::iw8::decal_volume_mask::dump(converted);
			zonetool::s4::converter::iw8::add_csv_row(zonetool::iw8::type_to_string(zonetool::iw8::ASSET_TYPE_DECAL_VOLUME_MASK), asset->name);
		}

		void dump_decal_volume_material(const zonetool::s4::GfxDecalVolumeMaterial* asset)
		{
			utils::memory::allocator allocator;
			const auto converted = allocator.allocate<zonetool::iw8::GfxDecalVolumeMaterial>();
			converted->name = asset->name;
			for (auto i = 0u; i < std::size(asset->channels); i++)
			{
				const auto* image = asset->channels[i];
				if (!image || !image->name || image->name[0] == '$')
				{
					continue;
				}

				if (zonetool::s4::converter::iw8::gfximage::dump_used(image))
				{
					converted->channels[i] = zonetool::s4::converter::iw8::gfximage::image_reference(image, allocator);
				}
				else
				{
					ZONETOOL_WARNING("decalvolumematerial \"%s\" loses the image \"%s\"", asset->name, image->name);
				}
			}

			converted->flags = asset->flags;
			std::memcpy(converted->colorTint, asset->colorTint, sizeof(asset->colorTint));
			converted->alphaDissolveParms = asset->alphaDissolveParms;
			converted->emissiveScale = half_float::half_to_float(asset->packedEmissiveScale);
			converted->packedDisplacementScaleAndBias = asset->packedDisplacementScaleAndBias;
			converted->displacementCutoffDistance = asset->displacementCutoffDistance;
			converted->displacementCutoffFalloff = asset->displacementCutoffFalloff;
			converted->packedTemperatureBaseAndScale = asset->packedTemperatureBaseAndScale;
			converted->textureAtlasRowCount = asset->textureAtlasRowCount;
			converted->textureAtlasColumnCount = asset->textureAtlasColumnCount;

			zonetool::iw8::decal_volume_material::dump(converted);
			zonetool::s4::converter::iw8::add_csv_row(zonetool::iw8::type_to_string(zonetool::iw8::ASSET_TYPE_DECAL_VOLUME_MATERIAL), asset->name);
		}

		void dump_tactical_graph(const zonetool::s4::TacticalGraphData* asset)
		{
			namespace target = zonetool::iw8;
			static_assert(sizeof(target::tacpoint_t) == sizeof(zonetool::s4::tacpoint_t));
			static_assert(sizeof(target::TacVisGraphRow) == sizeof(zonetool::s4::TacVisGraphRow));
			static_assert(sizeof(target::tacpoint_search_node_t) == sizeof(zonetool::s4::tacpoint_search_node_t));

			utils::memory::allocator allocator;
			const auto converted = allocator.allocate<target::TacticalGraphData>();
			converted->name = zonetool::s4::converter::iw8::bsp_name(asset->name, allocator);
			converted->m_NumTacGraphs = asset->m_NumTacGraphs;
			converted->m_TacGraphs = allocator.allocate_array<target::TacticalGraph>(asset->m_NumTacGraphs);
			for (auto i = 0; asset->m_TacGraphs && i < asset->m_NumTacGraphs; i++)
			{
				const auto& graph = asset->m_TacGraphs[i];
				if (graph.m_NumPoints && graph.m_NumVisRows != graph.m_NumPoints - 1)
				{
					ZONETOOL_WARNING("tacgraph \"%s\" graph %d: %u vis rows for %u points, IW8 needs %u", asset->name, i, graph.m_NumVisRows, graph.m_NumPoints, graph.m_NumPoints - 1);
					return;
				}

				auto& result = converted->m_TacGraphs[i];
				result.m_Points = reinterpret_cast<target::tacpoint_t*>(graph.m_Points);
				result.m_NumPoints = graph.m_NumPoints;
				result.m_NumVisBytes = graph.m_NumVisBytes;
				result.m_VisGraph = reinterpret_cast<target::TacVisGraphRow*>(graph.m_VisGraph);
				std::memcpy(&result.m_SearchRoot, &graph.m_SearchRoot, sizeof(result.m_SearchRoot));
			}

			target::tactical_graph::dump(converted);
			zonetool::s4::converter::iw8::add_csv_row(target::type_to_string(target::ASSET_TYPE_TACGRAPH), converted->name);
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::s4;
			namespace target = zonetool::iw8;

			switch (type)
			{
			case source::ASSET_TYPE_IMAGE:
				zonetool::s4::converter::iw8::gfximage::dump(static_cast<source::GfxImage*>(header));
				break;
			case source::ASSET_TYPE_XANIMPARTS:
				zonetool::s4::converter::iw8::xanim::dump(static_cast<source::XAnimParts*>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				zonetool::s4::converter::iw8::xsurface::queue(static_cast<source::XModelSurfs*>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				zonetool::s4::converter::iw8::xmodel::dump(static_cast<source::XModel*>(header));
				break;
			case source::ASSET_TYPE_PHYSICSASSET:
				zonetool::s4::converter::iw8::physics::dump(static_cast<source::PhysicsAsset*>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				zonetool::s4::converter::iw8::clipmap::dump(static_cast<source::clipMap_t*>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				zonetool::s4::converter::iw8::comworld::dump(static_cast<source::ComWorld*>(header));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				zonetool::s4::converter::iw8::mapents::dump(static_cast<source::MapEnts*>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				zonetool::s4::converter::iw8::gfxworld::dump(static_cast<source::GfxWorld*>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				zonetool::s4::converter::iw8::material::dump(static_cast<source::Material*>(header));
				break;
			case source::ASSET_TYPE_DECAL_VOLUME_MASK:
				dump_decal_volume_mask(static_cast<source::GfxDecalVolumeMask*>(header));
				break;
			case source::ASSET_TYPE_DECAL_VOLUME_MATERIAL:
				dump_decal_volume_material(static_cast<source::GfxDecalVolumeMaterial*>(header));
				break;
			case source::ASSET_TYPE_RAWFILE:
				if (zonetool::s4::converter::iw8::rawfile::is_vision(static_cast<source::RawFile*>(header)))
				{
					zonetool::s4::converter::iw8::rawfile::dump_vision(static_cast<source::RawFile*>(header));
					break;
				}

				dump_identical<target::RawFile, source::RawFile>(header, target::ASSET_TYPE_RAWFILE, target::rawfile::dump);
				break;
			case source::ASSET_TYPE_LIGHT_DEF:
				dump_identical<target::GfxLightDef, source::GfxLightDef>(header, target::ASSET_TYPE_LIGHT_DEF, target::light_def::dump);
				break;
			case source::ASSET_TYPE_VECTORFIELD:
				dump_identical<target::VectorField, source::VectorField>(header, target::ASSET_TYPE_VECTORFIELD, target::vector_field::dump);
				break;
			case source::ASSET_TYPE_XMODEL_DETAIL_COLLISION:
				zonetool::s4::converter::iw8::physics::dump(static_cast<source::XModelDetailCollision*>(header));
				break;
			case source::ASSET_TYPE_EDGE_LIST:
				dump_identical<target::MapEdgeList, source::MapEdgeList>(header, target::ASSET_TYPE_EDGE_LIST, target::map_edge_list::dump);
				break;
			case source::ASSET_TYPE_GLASSWORLD:
				dump_glass_world(static_cast<source::GlassWorld*>(header));
				break;
			case source::ASSET_TYPE_PATHDATA:
				dump_identical_bsp<target::PathData, source::PathData>(header, target::ASSET_TYPE_PATHDATA, target::path_data::dump);
				break;
			case source::ASSET_TYPE_NAVMESH:
				dump_identical_bsp<target::NavMeshData, source::NavMeshData>(header, target::ASSET_TYPE_NAVMESH, target::nav_mesh::dump);
				break;
			case source::ASSET_TYPE_KEY_VALUE_PAIRS:
				dump_identical<target::KeyValuePairs, source::KeyValuePairs>(header, target::ASSET_TYPE_KEY_VALUE_PAIRS, target::key_value_pairs::dump);
				break;
			case source::ASSET_TYPE_LOCALIZE_ENTRY:
				dump_identical<target::LocalizeEntry, source::LocalizeEntry>(header, target::ASSET_TYPE_LOCALIZE_ENTRY, target::localize::dump);
				break;
			case source::ASSET_TYPE_RETICLE:
				dump_identical<target::ReticleDef, source::ReticleDef>(header, target::ASSET_TYPE_RETICLE, target::reticle_def::dump);
				break;
			case source::ASSET_TYPE_RUMBLE:
				dump_identical<target::RumbleInfo, source::RumbleInfo>(header, target::ASSET_TYPE_RUMBLE, target::rumble::dump);
				break;
			case source::ASSET_TYPE_RUMBLE_GRAPH:
				dump_identical<target::RumbleGraph, source::RumbleGraph>(header, target::ASSET_TYPE_RUMBLE_GRAPH, target::rumble_graph::dump);
				break;
			case source::ASSET_TYPE_PHYSICS_FX_SHAPE:
				zonetool::s4::converter::iw8::physics::dump(static_cast<source::PhysicsFXShape*>(header));
				break;
			case source::ASSET_TYPE_PHYSICS_FX_PIPELINE:
				dump_identical<target::PhysicsFXPipeline, source::PhysicsFXPipeline>(header, target::ASSET_TYPE_PHYSICS_FX_PIPELINE, target::physics_fx_pipeline::dump);
				break;
			case source::ASSET_TYPE_PHYSICSLIBRARY:
				dump_identical<target::PhysicsLibrary, source::PhysicsLibrary>(header, target::ASSET_TYPE_PHYSICSLIBRARY, target::physics_library::dump);
				break;
			case source::ASSET_TYPE_CLOTH_ASSET:
				dump_identical<target::ClothAsset, source::ClothAsset>(header, target::ASSET_TYPE_CLOTH_ASSET, target::cloth_asset::dump);
				break;
			case source::ASSET_TYPE_CAMERA:
				dump_identical<target::CameraDef, source::CameraDef>(header, target::ASSET_TYPE_CAMERA, target::camera_def::dump);
				break;
			case source::ASSET_TYPE_FOG_SPLINE:
				dump_identical<target::GfxFogSpline, source::GfxFogSpline>(header, target::ASSET_TYPE_FOG_SPLINE, target::fog_spline::dump);
				break;
			case source::ASSET_TYPE_GRADING_CLUT:
				dump_identical<target::GfxGradingClut, source::GfxGradingClut>(header, target::ASSET_TYPE_GRADING_CLUT, target::grading_clut::dump);
				break;
			case source::ASSET_TYPE_IMPACT_FX:
				dump_identical<target::FxImpactTable, source::FxImpactTable>(header, target::ASSET_TYPE_IMPACT_FX, target::impact_fx::dump);
				break;
			case source::ASSET_TYPE_SURFACE_FX:
				dump_identical<target::SurfaceFxTable, source::SurfaceFxTable>(header, target::ASSET_TYPE_SURFACE_FX, target::surface_fx::dump);
				break;
			case source::ASSET_TYPE_DYN_ENTITY_LIST:
				dump_dyn_entity_list(static_cast<source::DynEntityList*>(header));
				break;
			case source::ASSET_TYPE_XANIMCURVE:
				dump_identical<target::XAnimCurve, source::XAnimCurve>(header, target::ASSET_TYPE_XANIMCURVE, target::anim_curve::dump);
				break;
			case source::ASSET_TYPE_COLLISION_TILE:
				zonetool::s4::converter::iw8::collisiontile::dump(static_cast<source::CollisionTile*>(header));
				break;
			case source::ASSET_TYPE_NET_CONST_STRINGS:
				zonetool::s4::converter::iw8::netconststrings::dump(static_cast<source::NetConstStrings*>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				zonetool::s4::converter::iw8::fxworld::dump(static_cast<source::FxWorld*>(header));
				break;
			case source::ASSET_TYPE_FXWORLD_TRANSIENT_ZONE:
				zonetool::s4::converter::iw8::fxworld::dump(static_cast<source::FxWorldTransientZone*>(header));
				break;
			case source::ASSET_TYPE_VFX:
				zonetool::s4::converter::iw8::vfx::dump(static_cast<source::ParticleSystemDef*>(header));
				break;
			case source::ASSET_TYPE_SCRIPTABLE:
				zonetool::s4::converter::iw8::scriptable::dump(static_cast<source::ScriptableDef*>(header));
				break;
			case source::ASSET_TYPE_SCRIPTFILE:
				zonetool::s4::converter::iw8::scriptfile::dump(static_cast<source::ScriptFile*>(header));
				break;
			case source::ASSET_TYPE_ST_TERRAIN:
				zonetool::s4::converter::iw8::stterrain::dump(static_cast<source::StTerrain*>(header));
				break;
			case source::ASSET_TYPE_STRINGTABLE:
				dump_identical<target::StringTable, source::StringTable>(header, target::ASSET_TYPE_STRINGTABLE, target::string_table::dump);
				break;
			case source::ASSET_TYPE_TACGRAPH:
				dump_tactical_graph(static_cast<source::TacticalGraphData*>(header));
				break;
			default:
				break;
			}
		}

		void setup(const std::filesystem::path& target_game_path)
		{
			zonetool::s4::converter::iw8::set_iw8_folder(target_game_path);
		}

		std::vector<std::string> required_zones(const std::string& zone_name)
		{
			return {"code_post_gfx", "common", "common_core_mp", "common_base_mp", "common_mp", "global", "global_core_mp", "global_mp", "global_stream_mp",
				"techsets_common", "techsets_common_core_mp", "techsets_common_base_mp", "techsets_common_mp", "techsets_" + zone_name};
		}

		void finish(const std::string& zone_name)
		{
			zonetool::s4::converter::iw8::xsurface::dump_queued();
			zonetool::s4::converter::iw8::material::dump_donor_techsets();
			zonetool::s4::converter::iw8::scriptable::dump_anim_trees();
			zonetool::s4::converter::iw8::netconststrings::dump_level_strings();
			zonetool::s4::converter::iw8::write_csv(zone_name);
		}
	}

	const converter& definition()
	{
		static const converter s4_iw8
		{
			.source = "s4",
			.target = "iw8",
			.dump_asset = dump_asset,
			.setup = setup,
			.finish = finish,
			.required_zones = required_zones,
		};

		return s4_iw8;
	}
}
