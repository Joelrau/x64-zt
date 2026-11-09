#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "mapents.hpp"
#include "physpreset.hpp"
#include "sound.hpp"

namespace zonetool::h1::converter::s2::clipmap
{
	namespace
	{
		clipMap_t* pending_clip_map = nullptr;

		constexpr zonetool::s2::Bounds care_package_brush{{0.5f, -0.5f, 15.0f}, {29.5f, 15.5f, 14.0f}};
		constexpr std::uint64_t care_package_contents = 0x8031640;

		std::string transient_zone_name(const std::string& clip_name)
		{
			auto base = clip_name.substr(clip_name.find_last_of('/') + 1);
			base = base.substr(0, base.find(".d3dbsp"));
			return base + "_000";
		}

		vertex_range partition_vertex_range(const PatchesCollisionData& data, const int first_tri, const int end_tri, const std::uint32_t vertex_base)
		{
			vertex_range range{std::numeric_limits<std::uint16_t>::max() + vertex_base, vertex_base};
			for (auto index = 3 * first_tri; index < 3 * end_tri; index++)
			{
				range.lowest = std::min(range.lowest, data.triIndices[index] + vertex_base);
				range.highest = std::max(range.highest, data.triIndices[index] + vertex_base);
			}

			return range;
		}

		std::uint32_t append_vertices(std::vector<std::array<float, 3>>& verts, const PatchesCollisionData& data, const vertex_range& range)
		{
			const auto base = (static_cast<std::uint32_t>(verts.size()) + 4096u - 1) / 4096u * 4096u;
			verts.resize(base);
			for (auto vertex = range.lowest; vertex <= range.highest; vertex++)
			{
				verts.push_back({data.verts[vertex][0], data.verts[vertex][1], data.verts[vertex][2]});
			}

			return base;
		}

		void convert_materials(const ClipInfo& info, zonetool::s2::ClipInfo& new_info, utils::memory::allocator& allocator)
		{
			const auto materials = allocator.allocate_array<zonetool::s2::ClipMaterial>(info.numMaterials);
			for (auto i = 0u; i < info.numMaterials; i++)
			{
				const auto& material = info.materials[i];
				materials[i].name = material.name;
				materials[i].surfaceFlags = convert_surface_flags(static_cast<std::uint32_t>(material.surfaceFlags));
				materials[i].contents = static_cast<std::uint32_t>(material.contents);
			}

			new_info.numMaterials = info.numMaterials;
			new_info.materials = materials;
		}

		unsigned int* convert_leaf_brushes(const ClipInfo& info, const LeafBrush* brushes, const std::int32_t count, unsigned int* leaf_brushes,
			utils::memory::allocator& allocator)
		{
			if (!brushes)
			{
				return nullptr;
			}

			const auto& tree = info.bCollisionTree;
			if (brushes >= tree.leafbrushes && brushes + count <= tree.leafbrushes + tree.numLeafBrushes)
			{
				return &leaf_brushes[brushes - tree.leafbrushes];
			}

			const auto result = allocator.allocate_array<unsigned int>(count);
			for (auto i = 0; i < count; i++)
			{
				result[i] = brushes[i];
			}

			return result;
		}

		void convert_leaf_brush_nodes(const ClipInfo& info, zonetool::s2::ClipInfo& new_info, utils::memory::allocator& allocator)
		{
			const auto& tree = info.bCollisionTree;
			const auto leaf_brushes = allocator.allocate_array<unsigned int>(tree.numLeafBrushes);
			for (auto i = 0u; i < tree.numLeafBrushes; i++)
			{
				leaf_brushes[i] = tree.leafbrushes[i];
			}

			const auto nodes = allocator.allocate_array<zonetool::s2::cLeafBrushNode_s>(tree.leafbrushNodesCount);
			for (auto i = 0u; i < tree.leafbrushNodesCount; i++)
			{
				const auto& node = tree.leafbrushNodes[i];
				auto& new_node = nodes[i];
				new_node.axis = node.axis;
				new_node.transientZone = 0;
				new_node.leafBrushCount = node.leafBrushCount;
				new_node.contents = static_cast<std::uint32_t>(node.contents);
				if (node.leafBrushCount > 0)
				{
					new_node.data.leaf.brushes = convert_leaf_brushes(info, node.data.leaf.brushes, node.leafBrushCount, leaf_brushes, allocator);
				}
				else
				{
					new_node.data.children = std::bit_cast<zonetool::s2::cLeafBrushNodeChildren_t>(node.data.children);
				}
			}

			new_info.bCollisionTree.leafbrushNodesCount = tree.leafbrushNodesCount;
			new_info.bCollisionTree.leafbrushNodes = nodes;
			new_info.bCollisionTree.numLeafBrushes = tree.numLeafBrushes;
			new_info.bCollisionTree.leafbrushes = leaf_brushes;
		}

		std::vector<std::uint32_t> convert_aabb_trees(const clipMap_t* asset, zonetool::s2::ClipInfo& new_info, utils::memory::allocator& allocator)
		{
			const auto& tree = asset->info.pCollisionTree;
			std::vector<std::uint32_t> partition_materials(asset->info.pCollisionData.partitionCount);

			const auto nodes = allocator.allocate_array<zonetool::s2::CollisionAabbTree>(tree.aabbTreeCount);
			for (auto i = 0; i < tree.aabbTreeCount; i++)
			{
				const auto& node = tree.aabbTrees[i];
				auto& new_node = nodes[i];
				std::memcpy(new_node.midPoint, node.midPoint, sizeof(new_node.midPoint));
				std::memcpy(new_node.halfSize, node.halfSize, sizeof(new_node.halfSize));
				new_node.transientZone = 0;
				new_node.u.firstChildIndex = static_cast<unsigned int>(node.u.firstChildIndex);
				if (node.childCount > std::numeric_limits<std::uint8_t>::max())
				{
					ZONETOOL_WARNING("clipMap \"%s\": aabb node %d has %u children, S2 allows 255", asset->name, i, node.childCount);
				}

				new_node.childCount = static_cast<std::uint8_t>(node.childCount);
				if (!node.childCount && node.u.partitionIndex >= 0 && node.u.partitionIndex < asset->info.pCollisionData.partitionCount)
				{
					partition_materials[node.u.partitionIndex] = node.materialIndex;
				}
			}

			new_info.pCollisionTree.aabbTreeCount = tree.aabbTreeCount;
			new_info.pCollisionTree.aabbTrees = nodes;
			return partition_materials;
		}

		void convert_smodel_tree(const ClipInfo& info, zonetool::s2::ClipInfo& new_info, utils::memory::allocator& allocator)
		{
			const auto& tree = info.sCollisionTree;
			const auto nodes = allocator.allocate_array<zonetool::s2::SModelAabbNode>(tree.smodelNodeCount);
			for (auto i = 0u; i < tree.smodelNodeCount; i++)
			{
				const auto& node = tree.smodelNodes[i];
				nodes[i].bounds = std::bit_cast<zonetool::s2::Bounds>(node.bounds);
				nodes[i].firstChild = node.firstChild;
				nodes[i].childCount = static_cast<std::uint8_t>(node.childCount);
				nodes[i].transientZone = 0;
			}

			new_info.sCollisionTree.numStaticModels = tree.numStaticModels;
			new_info.sCollisionTree.smodelNodeCount = tree.smodelNodeCount;
			new_info.sCollisionTree.smodelNodes = nodes;
		}

		void convert_brushes(const ClipInfo& info, zonetool::s2::BrushesCollisionData& new_data, utils::memory::allocator& allocator)
		{
			const auto& data = info.bCollisionData;
			const auto brushes = allocator.allocate_array<zonetool::s2::cbrush_t>(data.numBrushes);
			const auto contents = allocator.allocate_array<unsigned long long>(data.numBrushes);
			for (auto i = 0u; i < data.numBrushes; i++)
			{
				const auto& brush = data.brushes[i];
				brushes[i].sides = brush.numsides ? retype<zonetool::s2::cbrushside_t>(brush.sides) : nullptr;
				brushes[i].numsides = brush.numsides;
				std::memcpy(brushes[i].axialMaterialNum, brush.axialMaterialNum, sizeof(brush.axialMaterialNum));
				contents[i] = static_cast<std::uint32_t>(data.brushContents[i]);
			}

			new_data.brushSides = retype<zonetool::s2::cbrushside_t>(data.brushSides);
			new_data.numBrushSides = data.numBrushSides;
			new_data.numBrushes = data.numBrushes;
			new_data.brushes = brushes;
			new_data.brushBounds = retype<zonetool::s2::Bounds>(data.brushBounds);
			new_data.brushContents = contents;
		}

		void convert_patches(const clipMap_t* asset, const std::vector<std::uint32_t>& partition_materials, zonetool::s2::PatchesCollisionData& new_data,
			utils::memory::allocator& allocator)
		{
			const auto& data = asset->info.pCollisionData;
			const auto tri_indices = allocator.allocate_array<std::uint16_t>(3ull * data.triCount);
			std::memcpy(tri_indices, data.triIndices, sizeof(std::uint16_t) * 3 * data.triCount);

			std::vector<std::array<float, 3>> verts(data.vertCount);
			std::memcpy(verts.data(), data.verts, sizeof(vec3_t) * data.vertCount);

			std::vector<std::optional<std::uint32_t>> tri_segments(data.triCount);
			auto overflow_count = 0u;
			auto shared_tri_conflicts = 0u;
			const auto partitions = allocator.allocate_array<zonetool::s2::CollisionPartition>(data.partitionCount);
			for (auto i = 0; i < data.partitionCount; i++)
			{
				const auto& partition = data.partitions[i];
				const auto first_tri = partition.firstTri;
				const auto end_tri = std::min(partition.firstTri + partition.triCount, data.triCount);
				const auto h1_vertex_base = partition.firstVertSegment * 1024u;
				const auto range = partition_vertex_range(data, first_tri, end_tri, h1_vertex_base);
				auto first_vertex = range.lowest / 4096u * 4096u;
				auto s2_vertex_base = first_vertex;
				if (range.highest - first_vertex > std::numeric_limits<std::uint16_t>::max())
				{
					s2_vertex_base = append_vertices(verts, data, range);
					first_vertex = range.lowest;
				}

				const auto s2_segment = s2_vertex_base / 4096u;

				auto& new_partition = partitions[i];
				new_partition.triCount = partition.triCount;
				new_partition.borderCount = partition.borderCount;
				new_partition.firstVertSegment = static_cast<std::uint8_t>(s2_segment);
				new_partition.firstTri = partition.firstTri;
				new_partition.materialIndex = partition_materials[i];

				const auto border = partition.borders ? partition.borders - data.borders : -1;
				new_partition.firstBorder = border >= 0 && border < data.borderCount ? static_cast<std::int32_t>(border) : -1;

				for (auto tri = first_tri; tri < end_tri; tri++)
				{
					if (tri_segments[tri])
					{
						shared_tri_conflicts += *tri_segments[tri] != s2_segment;
						continue;
					}

					tri_segments[tri] = s2_segment;
					for (auto corner = 0; corner < 3; corner++)
					{
						const auto index = data.triIndices[3 * tri + corner] + h1_vertex_base - first_vertex;
						overflow_count += index > std::numeric_limits<std::uint16_t>::max();
						tri_indices[3 * tri + corner] = static_cast<std::uint16_t>(index);
					}
				}
			}

			if (shared_tri_conflicts)
			{
				ZONETOOL_WARNING("clipMap \"%s\": %u shared collision tris need two vertex segments", asset->name, shared_tri_conflicts);
			}

			if (overflow_count)
			{
				ZONETOOL_WARNING("clipMap \"%s\": %u collision tri indices do not fit 16 bits", asset->name, overflow_count);
			}

			const auto new_verts = allocator.allocate_array<zonetool::s2::vec3_t>(verts.size());
			std::memcpy(new_verts, verts.data(), sizeof(zonetool::s2::vec3_t) * verts.size());
			new_data.vertCount = static_cast<unsigned int>(verts.size());
			new_data.verts = new_verts;
			new_data.triCount = data.triCount;
			new_data.triIndices = tri_indices;
			new_data.triEdgeIsWalkable = data.triEdgeIsWalkable;
			new_data.borderCount = data.borderCount;
			new_data.borders = retype<zonetool::s2::CollisionBorder>(data.borders);
			new_data.partitionCount = data.partitionCount;
			new_data.partitions = partitions;
		}

		zonetool::s2::ClipMapTrZone* convert_transient_zone(const clipMap_t* asset, const std::vector<std::uint32_t>& partition_materials,
			utils::memory::allocator& allocator)
		{
			const auto zone = allocator.allocate<zonetool::s2::ClipMapTrZone>();
			zone->name = allocator.duplicate_string(transient_zone_name(asset->name));
			convert_brushes(asset->info, zone->bCollisionData, allocator);
			convert_patches(asset, partition_materials, zone->pCollisionData, allocator);

			const auto& models = asset->info.sCollisionData;
			const auto new_models = allocator.allocate_array<zonetool::s2::ClipMapTrZoneStaticModel>(models.numStaticModels);
			for (auto i = 0u; i < models.numStaticModels; i++)
			{
				auto model = models.staticModelList[i];
				model.lightingHandle = 0;
				std::memcpy(&new_models[i], &model, sizeof(model));
			}

			zone->staticModels.count = models.numStaticModels;
			zone->staticModels.models = new_models;
			return zone;
		}

		void convert_nodes(const clipMap_t* asset, zonetool::s2::clipMap_t* new_asset, utils::memory::allocator& allocator)
		{
			const auto nodes = allocator.allocate_array<zonetool::s2::cNode_t>(asset->numNodes);
			for (auto i = 0u; i < asset->numNodes; i++)
			{
				nodes[i].plane = retype<zonetool::s2::cplane_s>(asset->nodes[i].plane);
				nodes[i].children[0] = asset->nodes[i].children[0];
				nodes[i].children[1] = asset->nodes[i].children[1];
				nodes[i].transientZone = 0xFF;
			}

			new_asset->numNodes = asset->numNodes;
			new_asset->nodes = nodes;
		}

		zonetool::s2::cLeaf_t convert_leaf(const cLeaf_t& leaf)
		{
			zonetool::s2::cLeaf_t new_leaf{};
			new_leaf.firstCollAabbIndex = leaf.firstCollAabbIndex;
			new_leaf.collAabbCount = leaf.collAabbCount;
			new_leaf.brushContents = static_cast<std::uint32_t>(leaf.brushContents);
			new_leaf.terrainContents = static_cast<std::uint32_t>(leaf.terrainContents);
			new_leaf.bounds = std::bit_cast<zonetool::s2::Bounds>(leaf.bounds);
			new_leaf.leafBrushNode = leaf.leafBrushNode;
			new_leaf.brushTransientZone = leaf.brushContents ? 0 : 0xFF;
			new_leaf.aabbTransientZone = leaf.collAabbCount ? 0 : 0xFF;
			return new_leaf;
		}

		void convert_leafs(const clipMap_t* asset, zonetool::s2::clipMap_t* new_asset, utils::memory::allocator& allocator)
		{
			const auto leafs = allocator.allocate_array<zonetool::s2::cLeaf_t>(asset->numLeafs);
			for (auto i = 0u; i < asset->numLeafs; i++)
			{
				leafs[i] = convert_leaf(asset->leafs[i]);
			}

			new_asset->numLeafs = asset->numLeafs;
			new_asset->leafs = leafs;

			const auto cmodels = allocator.allocate_array<zonetool::s2::cmodel_t>(asset->numSubModels);
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				const auto& cmodel = asset->cmodels[i];
				cmodels[i].bounds = std::bit_cast<zonetool::s2::Bounds>(cmodel.bounds);
				cmodels[i].radius = cmodel.radius;
				cmodels[i].info = new_asset->pInfo;
				cmodels[i].leaf = convert_leaf(cmodel.leaf);
				cmodels[i].navObstacleIdx = i ? 0xFFFF : 0;
			}

			new_asset->numSubModels = asset->numSubModels;
			new_asset->cmodels = cmodels;
		}

		void convert_stages(const clipMap_t* asset, zonetool::s2::clipMap_t* new_asset, utils::memory::allocator& allocator)
		{
			new_asset->stageCount = asset->stageCount;
			new_asset->stages = retype<zonetool::s2::Stage>(asset->stages);
			new_asset->stageTrigger = mapents::convert_triggers(asset->stageTrigger, allocator);
		}

		void convert_dyn_ents(const clipMap_t* asset, zonetool::s2::clipMap_t* new_asset, utils::memory::allocator& allocator)
		{
			for (auto i = 0; i < 2; i++)
			{
				const auto count = asset->dynEntCount[i];
				new_asset->dynEntCount[i] = count;
				if (!count)
				{
					continue;
				}

				const auto defs = allocator.allocate_array<zonetool::s2::DynEntityDef>(count);
				for (auto j = 0u; j < count; j++)
				{
					const auto& def = asset->dynEntDefList[i][j];
					auto& new_def = defs[j];
					new_def.type = static_cast<zonetool::s2::DynEntityType>(def.type);
					new_def.pose = std::bit_cast<zonetool::s2::GfxPlacement>(def.pose);
					new_def.xModel = reinterpret_cast<zonetool::s2::XModel*>(def.baseModel);
					new_def.brushModel = def.brushModel;
					new_def.physicsBrushModel = def.physicsBrushModel;
					new_def.scriptableIndex = def.scriptableIndex;
					new_def.health = def.health;
					new_def.destroyFx = reinterpret_cast<zonetool::s2::FxEffectDef*>(def.destroyFx);
					new_def.sound = reinterpret_cast<zonetool::s2::snd_alias_list_t*>(def.sound);
					sound::dump_used(def.sound, "dynent sound");
					new_def.physPreset = physpreset::reference(def.physPreset, def.baseModel, allocator);
					new_def.hinge = retype<zonetool::s2::DynEntityHingeDef>(def.hinge);
					new_def.linkTo = retype<zonetool::s2::DynEntityLinkToDef>(def.linkTo);
					new_def.contents = static_cast<std::uint32_t>(def.contents);
					new_def.impactDamageThreshold = def.unk[0];
					new_def.impactDamageScale = def.unk[1];
				}

				new_asset->dynEntDefList[i] = defs;
				new_asset->dynEntPoseList[i] = allocator.allocate_array<zonetool::s2::DynEntityPose>(count);
				new_asset->dynEntClientList[i] = allocator.allocate_array<zonetool::s2::DynEntityClient>(count);
				new_asset->dynEntCollList[i] = allocator.allocate_array<zonetool::s2::DynEntityColl>(count);
			}
		}

		template <typename T>
		T* append(const T* source, const std::size_t count, const T& value, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate_array<T>(count + 1);
			if (count)
			{
				std::memcpy(result, source, sizeof(T) * count);
			}

			result[count] = value;
			return result;
		}

		int add_care_package_brush(zonetool::s2::ClipInfo& info, utils::memory::allocator& allocator)
		{
			const auto material = static_cast<short>(info.numMaterials);
			info.materials = append(info.materials, info.numMaterials, {"clip_nosight_metal", 0x1D440A0, {}, care_package_contents}, allocator);
			info.numMaterials++;

			auto& brushes = info.trZones[0]->bCollisionData;
			const auto brush_index = brushes.numBrushes;
			zonetool::s2::cbrush_t brush{};
			std::fill_n(&brush.axialMaterialNum[0][0], 6, material);
			brushes.brushes = append(brushes.brushes, brush_index, brush, allocator);
			brushes.brushBounds = append(brushes.brushBounds, brush_index, care_package_brush, allocator);
			brushes.brushContents = append(brushes.brushContents, brush_index, care_package_contents, allocator);
			brushes.numBrushes++;
			info.transientZones.zones[0].brushEnd = brushes.numBrushes;

			auto& tree = info.bCollisionTree;
			zonetool::s2::cLeafBrushNode_s node{};
			node.transientZone = 0;
			node.leafBrushCount = 1;
			node.contents = care_package_contents;
			node.data.leaf.brushes = allocator.allocate<unsigned int>();
			*node.data.leaf.brushes = brush_index;
			tree.leafbrushNodes = append(tree.leafbrushNodes, tree.leafbrushNodesCount, node, allocator);
			return static_cast<int>(tree.leafbrushNodesCount++);
		}

		void add_care_package_model(zonetool::s2::clipMap_t* new_asset, const int leaf_brush_node, utils::memory::allocator& allocator)
		{
			zonetool::s2::cmodel_t cmodel{};
			cmodel.bounds = care_package_bounds(1.0f);
			cmodel.radius = care_package_radius();
			cmodel.info = new_asset->pInfo;
			cmodel.leaf.firstCollAabbIndex = new_asset->numSubModels ? new_asset->cmodels[new_asset->numSubModels - 1].leaf.firstCollAabbIndex : 0;
			cmodel.leaf.brushContents = care_package_contents;
			cmodel.leaf.bounds = care_package_bounds(0.125f);
			cmodel.leaf.leafBrushNode = leaf_brush_node;
			cmodel.leaf.brushTransientZone = 0;
			cmodel.leaf.aabbTransientZone = 0xFF;
			cmodel.navObstacleIdx = 0xFFFF;

			new_asset->cmodels = append(new_asset->cmodels, new_asset->numSubModels, cmodel, allocator);
			ZONETOOL_INFO("clipMap \"%s\": added care package collision brush model *%u", new_asset->name, new_asset->numSubModels);
			new_asset->numSubModels++;
		}

		zonetool::s2::clipMap_t* convert(const clipMap_t* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::clipMap_t>();
			new_asset->name = asset->name;

			const auto& info = asset->info;
			auto& new_info = new_asset->info;
			new_info.planeCount = info.planeCount;
			new_info.planes = retype<zonetool::s2::cplane_s>(info.planes);
			convert_materials(info, new_info, allocator);
			convert_leaf_brush_nodes(info, new_info, allocator);
			const auto partition_materials = convert_aabb_trees(asset, new_info, allocator);
			convert_smodel_tree(info, new_info, allocator);

			new_info.transientZones.count = 1;
			new_info.transientZones.zones[0].brushStart = 0;
			new_info.transientZones.zones[0].brushEnd = info.bCollisionData.numBrushes;
			new_info.transientZones.zones[0].partitionCount = info.pCollisionData.partitionCount;
			new_info.trZones[0] = convert_transient_zone(asset, partition_materials, allocator);

			const auto care_package = care_package_model(asset->name);
			const auto care_package_node = care_package ? add_care_package_brush(new_info, allocator) : -1;

			new_asset->pInfo = allocator.allocate<zonetool::s2::ClipInfo>();
			*new_asset->pInfo = new_info;

			convert_nodes(asset, new_asset, allocator);
			convert_leafs(asset, new_asset, allocator);
			if (care_package)
			{
				add_care_package_model(new_asset, care_package_node, allocator);
			}

			convert_stages(asset, new_asset, allocator);
			new_asset->mapEnts = asset->mapEnts ? make_stub<zonetool::s2::MapEnts>(asset->mapEnts->name, allocator) : nullptr;
			new_asset->checksum = asset->checksum;
			convert_dyn_ents(asset, new_asset, allocator);
			return new_asset;
		}
	}

	std::optional<unsigned int> care_package_model(const std::string& map_name)
	{
		const auto* clip_map = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_CLIPMAP, map_name).clipMap;
		if (!map_name.starts_with("maps/mp/") || !clip_map || !clip_map->mapEnts || !clip_map->mapEnts->entityString)
		{
			return {};
		}

		const std::string_view entities(clip_map->mapEnts->entityString, static_cast<std::size_t>(clip_map->mapEnts->numEntityChars));
		if (entities.find("\"airdrop_pallet\"") != std::string_view::npos)
		{
			return {};
		}

		return clip_map->numSubModels;
	}

	zonetool::s2::Bounds care_package_bounds(const float padding)
	{
		auto bounds = care_package_brush;
		for (auto& half_size : bounds.halfSize)
		{
			half_size += padding;
		}

		return bounds;
	}

	float care_package_radius()
	{
		const auto bounds = care_package_bounds(1.0f);
		auto length = 0.0f;
		for (auto i = 0; i < 3; i++)
		{
			const auto corner = std::abs(bounds.midPoint[i]) + bounds.halfSize[i];
			length += corner * corner;
		}

		return std::sqrt(length);
	}

	void dump(clipMap_t* asset)
	{
		pending_clip_map = asset;
	}

	void finish()
	{
		if (!pending_clip_map)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto new_asset = convert(pending_clip_map, allocator);
		const auto zone = new_asset->info.trZones[0];
		zonetool::s2::clip_map_trzone::dump(zone);
		add_csv_row("col_map_trzone", zone->name);
		zonetool::s2::clip_map::dump(new_asset);
		add_csv_row("col_map_mp", new_asset->name);
		pending_clip_map = nullptr;
	}
}
