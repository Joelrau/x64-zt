#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "mapents.hpp"

namespace zonetool::h1::converter::s2::clipmap
{
	namespace
	{
		constexpr std::uint8_t transient_zone = 0;
		constexpr std::uint8_t no_transient_zone = 0xFF;

		const std::unordered_set<std::string> s2_global_phys_presets
		{
			"default", "phys_brick", "phys_cloth", "phys_flesh", "phys_fruit", "phys_glass", "phys_metal_chain",
			"phys_metal_hollow", "phys_metal_hollow_sml", "phys_metal_solid", "phys_metal_thin", "phys_metal_vehicle",
			"phys_metal_weapon", "phys_plastic_hollow", "phys_plastic_solid", "phys_wood_debris", "phys_wood_solid",
			"phys_wood_solid_donotcollide",
		};

		clipMap_t* pending_clip_map = nullptr;

		std::string transient_zone_name(const std::string& clip_name)
		{
			auto base = clip_name.substr(clip_name.find_last_of('/') + 1);
			base = base.substr(0, base.find(".d3dbsp"));
			return base + "_000";
		}

		std::uint32_t lowest_vertex(const PatchesCollisionData& data, const int first_tri, const int end_tri, const std::uint32_t vertex_base)
		{
			auto lowest = std::numeric_limits<std::uint16_t>::max() + vertex_base;
			for (auto index = 3 * first_tri; index < 3 * end_tri; index++)
			{
				lowest = std::min(lowest, data.triIndices[index] + vertex_base);
			}

			return lowest;
		}

		class converter
		{
		public:
			converter(clipMap_t* asset, utils::memory::allocator& allocator)
				: asset(asset)
				, info(asset->info)
				, allocator(allocator)
			{
			}

			zonetool::s2::clipMap_t* convert()
			{
				const auto new_asset = this->allocator.allocate<zonetool::s2::clipMap_t>();
				new_asset->name = this->asset->name;

				auto& new_info = new_asset->info;
				new_info.planeCount = this->info.planeCount;
				new_info.planes = reinterpret_cast<zonetool::s2::cplane_s*>(this->info.planes);
				this->convert_materials(new_info);
				this->convert_leaf_brush_nodes(new_info);
				this->convert_aabb_trees(new_info);
				this->convert_smodel_tree(new_info);

				new_info.transientZones.count = 1;
				new_info.transientZones.zones[0].brushStart = 0;
				new_info.transientZones.zones[0].brushEnd = this->info.bCollisionData.numBrushes;
				new_info.transientZones.zones[0].partitionCount = this->info.pCollisionData.partitionCount;
				new_info.trZones[0] = this->convert_transient_zone();

				new_asset->pInfo = this->allocator.allocate<zonetool::s2::ClipInfo>();
				*new_asset->pInfo = new_info;

				this->convert_nodes(new_asset);
				this->convert_leafs(new_asset);
				this->convert_stages(new_asset);
				new_asset->mapEnts = reinterpret_cast<zonetool::s2::MapEnts*>(this->asset->mapEnts);
				new_asset->checksum = this->asset->checksum;

				this->convert_dyn_ents(new_asset);

				return new_asset;
			}

		private:
			clipMap_t* asset;
			ClipInfo& info;
			utils::memory::allocator& allocator;
			std::vector<std::uint32_t> partition_materials;
			std::unordered_map<std::string, zonetool::s2::PhysPreset*> phys_presets;

			zonetool::s2::PhysPreset* convert_phys_preset(const PhysPreset* preset)
			{
				const std::string name = preset && preset->name && s2_global_phys_presets.contains(preset->name) ? preset->name : "default";
				auto& result = this->phys_presets[name];
				if (!result)
				{
					result = this->allocator.allocate<zonetool::s2::PhysPreset>();
					result->name = this->allocator.duplicate_string(name);
					add_csv_row("physpreset", name, true);
				}

				return result;
			}

			static zonetool::s2::DynEntityType convert_dyn_ent_type(const DynEntityType type)
			{
				// scriptable dynents index the scriptable instances of the map (0x140678250)
				if (type == DYNENT_TYPE_SCRIPTABLEINST || type == DYNENT_TYPE_SCRIPTABLEPHYSICS)
				{
					return zonetool::s2::DYNENT_TYPE_CLUTTER;
				}

				return static_cast<zonetool::s2::DynEntityType>(type);
			}

			void convert_dyn_ents(zonetool::s2::clipMap_t* new_asset)
			{
				for (auto i = 0; i < 2; i++)
				{
					const auto count = this->asset->dynEntCount[i];
					new_asset->dynEntCount[i] = count;
					if (!count)
					{
						continue;
					}

					const auto defs = this->allocator.allocate_array<zonetool::s2::DynEntityDef>(count);
					for (auto j = 0u; j < count; j++)
					{
						const auto& def = this->asset->dynEntDefList[i][j];
						auto& new_def = defs[j];
						new_def.type = convert_dyn_ent_type(def.type);
						new_def.pose = std::bit_cast<zonetool::s2::GfxPlacement>(def.pose);
						new_def.xModel = reinterpret_cast<zonetool::s2::XModel*>(def.baseModel);
						new_def.brushModel = def.brushModel;
						new_def.physicsBrushModel = def.physicsBrushModel;
						new_def.scriptableIndex = def.scriptableIndex;
						new_def.health = def.health;
						new_def.physPreset = this->convert_phys_preset(def.physPreset);
						new_def.hinge = reinterpret_cast<zonetool::s2::DynEntityHingeDef*>(def.hinge);
						new_def.linkTo = reinterpret_cast<zonetool::s2::DynEntityLinkToDef*>(def.linkTo);
						new_def.contents = static_cast<std::uint32_t>(def.contents);
						std::memcpy(new_def.unk_0x70, def.unk, sizeof(new_def.unk_0x70));
					}

					new_asset->dynEntDefList[i] = defs;
					new_asset->dynEntPoseList[i] = this->allocator.allocate_array<char[60]>(count);
					new_asset->dynEntClientList[i] = this->allocator.allocate_array<char[48]>(count);
					new_asset->dynEntCollList[i] = this->allocator.allocate_array<char[20]>(count);
				}
			}

			void convert_materials(zonetool::s2::ClipInfo& new_info) const
			{
				const auto materials = this->allocator.allocate_array<zonetool::s2::ClipMaterial>(this->info.numMaterials);
				for (auto i = 0u; i < this->info.numMaterials; i++)
				{
					const auto& material = this->info.materials[i];
					materials[i].name = material.name;
					materials[i].surfaceFlags = convert_surface_flags(static_cast<std::uint32_t>(material.surfaceFlags));
					materials[i].contents = static_cast<std::uint32_t>(material.contents);
				}

				new_info.numMaterials = this->info.numMaterials;
				new_info.materials = materials;
			}

			void convert_leaf_brush_nodes(zonetool::s2::ClipInfo& new_info) const
			{
				const auto& tree = this->info.bCollisionTree;
				const auto leaf_brushes = this->allocator.allocate_array<unsigned int>(tree.numLeafBrushes);
				for (auto i = 0u; i < tree.numLeafBrushes; i++)
				{
					leaf_brushes[i] = tree.leafbrushes[i];
				}

				const auto nodes = this->allocator.allocate_array<zonetool::s2::cLeafBrushNode_s>(tree.leafbrushNodesCount);
				for (auto i = 0u; i < tree.leafbrushNodesCount; i++)
				{
					const auto& node = tree.leafbrushNodes[i];
					auto& new_node = nodes[i];
					new_node.axis = node.axis;
					new_node.transientZone = transient_zone;
					new_node.leafBrushCount = node.leafBrushCount;
					new_node.contents = static_cast<std::uint32_t>(node.contents);
					if (node.leafBrushCount > 0)
					{
						new_node.data.leaf.brushes = this->convert_leaf_brushes(node.data.leaf.brushes, node.leafBrushCount, leaf_brushes);
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

			unsigned int* convert_leaf_brushes(const LeafBrush* brushes, const std::int32_t count, unsigned int* leaf_brushes) const
			{
				if (!brushes)
				{
					return nullptr;
				}

				const auto& tree = this->info.bCollisionTree;
				if (brushes >= tree.leafbrushes && brushes + count <= tree.leafbrushes + tree.numLeafBrushes)
				{
					return &leaf_brushes[brushes - tree.leafbrushes];
				}

				const auto result = this->allocator.allocate_array<unsigned int>(count);
				for (auto i = 0; i < count; i++)
				{
					result[i] = brushes[i];
				}

				return result;
			}

			void convert_aabb_trees(zonetool::s2::ClipInfo& new_info)
			{
				const auto& tree = this->info.pCollisionTree;
				this->partition_materials.assign(this->info.pCollisionData.partitionCount, 0);

				const auto nodes = this->allocator.allocate_array<zonetool::s2::CollisionAabbTree>(tree.aabbTreeCount);
				for (auto i = 0; i < tree.aabbTreeCount; i++)
				{
					const auto& node = tree.aabbTrees[i];
					auto& new_node = nodes[i];
					std::memcpy(new_node.midPoint, node.midPoint, sizeof(new_node.midPoint));
					std::memcpy(new_node.halfSize, node.halfSize, sizeof(new_node.halfSize));
					new_node.transientZone = transient_zone;
					new_node.u.firstChildIndex = static_cast<unsigned int>(node.u.firstChildIndex);
					if (node.childCount > std::numeric_limits<std::uint8_t>::max())
					{
						ZONETOOL_WARNING("clipMap \"%s\": aabb node %d has %u children, S2 allows 255", this->asset->name, i, node.childCount);
					}

					new_node.childCount = static_cast<std::uint8_t>(node.childCount);
					if (!node.childCount && node.u.partitionIndex >= 0 && node.u.partitionIndex < this->info.pCollisionData.partitionCount)
					{
						this->partition_materials[node.u.partitionIndex] = node.materialIndex;
					}
				}

				new_info.pCollisionTree.aabbTreeCount = tree.aabbTreeCount;
				new_info.pCollisionTree.aabbTrees = nodes;
			}

			void convert_smodel_tree(zonetool::s2::ClipInfo& new_info) const
			{
				const auto& tree = this->info.sCollisionTree;
				const auto nodes = this->allocator.allocate_array<zonetool::s2::SModelAabbNode>(tree.smodelNodeCount);
				for (auto i = 0u; i < tree.smodelNodeCount; i++)
				{
					const auto& node = tree.smodelNodes[i];
					nodes[i].bounds = std::bit_cast<zonetool::s2::Bounds>(node.bounds);
					nodes[i].firstChild = node.firstChild;
					nodes[i].childCount = static_cast<std::uint8_t>(node.childCount);
					nodes[i].transientZone = transient_zone;
				}

				new_info.sCollisionTree.numStaticModels = tree.numStaticModels;
				new_info.sCollisionTree.smodelNodeCount = tree.smodelNodeCount;
				new_info.sCollisionTree.smodelNodes = nodes;
			}

			zonetool::s2::ClipMapTrZone* convert_transient_zone()
			{
				const auto zone = this->allocator.allocate<zonetool::s2::ClipMapTrZone>();
				zone->name = this->allocator.duplicate_string(transient_zone_name(this->asset->name));
				this->convert_brushes(zone->bCollisionData);
				this->convert_patches(zone->pCollisionData);

				const auto& models = this->info.sCollisionData;
				const auto new_models = this->allocator.allocate_array<zonetool::s2::ClipMapTrZoneStaticModel>(models.numStaticModels);
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

			void convert_brushes(zonetool::s2::BrushesCollisionData& new_data) const
			{
				const auto& data = this->info.bCollisionData;
				const auto brushes = this->allocator.allocate_array<zonetool::s2::cbrush_t>(data.numBrushes);
				const auto contents = this->allocator.allocate_array<unsigned long long>(data.numBrushes);
				for (auto i = 0u; i < data.numBrushes; i++)
				{
					const auto& brush = data.brushes[i];
					brushes[i].sides = brush.numsides ? reinterpret_cast<zonetool::s2::cbrushside_t*>(brush.sides) : nullptr;
					brushes[i].numsides = brush.numsides;
					std::memcpy(brushes[i].axialMaterialNum, brush.axialMaterialNum, sizeof(brush.axialMaterialNum));
					contents[i] = static_cast<std::uint32_t>(data.brushContents[i]);
				}

				new_data.brushSides = reinterpret_cast<zonetool::s2::cbrushside_t*>(data.brushSides);
				new_data.numBrushSides = data.numBrushSides;
				new_data.numBrushes = data.numBrushes;
				new_data.brushes = brushes;
				new_data.brushBounds = reinterpret_cast<zonetool::s2::Bounds*>(data.brushBounds);
				new_data.brushContents = contents;
			}

			void convert_patches(zonetool::s2::PatchesCollisionData& new_data) const
			{
				const auto& data = this->info.pCollisionData;
				const auto tri_indices = this->allocator.allocate_array<std::uint16_t>(3ull * data.triCount);
				std::memcpy(tri_indices, data.triIndices, sizeof(std::uint16_t) * 3 * data.triCount);

				std::vector<std::optional<std::uint32_t>> tri_segments(data.triCount);
				auto overflow_count = 0u;
				auto shared_tri_conflicts = 0u;
				const auto partitions = this->allocator.allocate_array<zonetool::s2::CollisionPartition>(data.partitionCount);
				for (auto i = 0; i < data.partitionCount; i++)
				{
					const auto& partition = data.partitions[i];
					const auto first_tri = partition.firstTri;
					const auto end_tri = std::min(partition.firstTri + partition.triCount, data.triCount);
					const auto h1_vertex_base = partition.firstVertSegment * 1024u;
					const auto s2_segment = lowest_vertex(data, first_tri, end_tri, h1_vertex_base) / 4096u;
					const auto s2_vertex_base = s2_segment * 4096u;

					auto& new_partition = partitions[i];
					new_partition.triCount = partition.triCount;
					new_partition.borderCount = partition.borderCount;
					new_partition.firstVertSegment = static_cast<std::uint8_t>(s2_segment);
					new_partition.firstTri = partition.firstTri;
					new_partition.materialIndex = this->partition_materials[i];

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
							const auto index = data.triIndices[3 * tri + corner] + h1_vertex_base - s2_vertex_base;
							overflow_count += index > std::numeric_limits<std::uint16_t>::max();
							tri_indices[3 * tri + corner] = static_cast<std::uint16_t>(index);
						}
					}
				}

				if (shared_tri_conflicts)
				{
					ZONETOOL_WARNING("clipMap \"%s\": %u shared collision tris need two vertex segments", this->asset->name, shared_tri_conflicts);
				}

				if (overflow_count)
				{
					ZONETOOL_WARNING("clipMap \"%s\": %u collision tri indices do not fit 16 bits", this->asset->name, overflow_count);
				}

				new_data.vertCount = data.vertCount;
				new_data.verts = reinterpret_cast<zonetool::s2::vec3_t*>(data.verts);
				new_data.triCount = data.triCount;
				new_data.triIndices = tri_indices;
				new_data.triEdgeIsWalkable = data.triEdgeIsWalkable;
				new_data.borderCount = data.borderCount;
				new_data.borders = reinterpret_cast<zonetool::s2::CollisionBorder*>(data.borders);
				new_data.partitionCount = data.partitionCount;
				new_data.partitions = partitions;
			}

			void convert_nodes(zonetool::s2::clipMap_t* new_asset) const
			{
				const auto nodes = this->allocator.allocate_array<zonetool::s2::cNode_t>(this->asset->numNodes);
				for (auto i = 0u; i < this->asset->numNodes; i++)
				{
					nodes[i].plane = reinterpret_cast<zonetool::s2::cplane_s*>(this->asset->nodes[i].plane);
					nodes[i].children[0] = this->asset->nodes[i].children[0];
					nodes[i].children[1] = this->asset->nodes[i].children[1];
					nodes[i].transientZone = no_transient_zone;
				}

				new_asset->numNodes = this->asset->numNodes;
				new_asset->nodes = nodes;
			}

			static zonetool::s2::cLeaf_t convert_leaf(const cLeaf_t& leaf)
			{
				zonetool::s2::cLeaf_t new_leaf{};
				new_leaf.firstCollAabbIndex = leaf.firstCollAabbIndex;
				new_leaf.collAabbCount = leaf.collAabbCount;
				new_leaf.brushContents = static_cast<std::uint32_t>(leaf.brushContents);
				new_leaf.terrainContents = static_cast<std::uint32_t>(leaf.terrainContents);
				new_leaf.bounds = std::bit_cast<zonetool::s2::Bounds>(leaf.bounds);
				new_leaf.leafBrushNode = leaf.leafBrushNode;
				new_leaf.brushTransientZone = leaf.brushContents ? transient_zone : no_transient_zone;
				new_leaf.aabbTransientZone = leaf.collAabbCount ? transient_zone : no_transient_zone;
				return new_leaf;
			}

			void convert_leafs(zonetool::s2::clipMap_t* new_asset) const
			{
				const auto leafs = this->allocator.allocate_array<zonetool::s2::cLeaf_t>(this->asset->numLeafs);
				for (auto i = 0u; i < this->asset->numLeafs; i++)
				{
					leafs[i] = convert_leaf(this->asset->leafs[i]);
				}

				new_asset->numLeafs = this->asset->numLeafs;
				new_asset->leafs = leafs;

				const auto cmodels = this->allocator.allocate_array<zonetool::s2::cmodel_t>(this->asset->numSubModels);
				for (auto i = 0u; i < this->asset->numSubModels; i++)
				{
					const auto& cmodel = this->asset->cmodels[i];
					cmodels[i].bounds = std::bit_cast<zonetool::s2::Bounds>(cmodel.bounds);
					cmodels[i].radius = cmodel.radius;
					cmodels[i].info = new_asset->pInfo;
					cmodels[i].leaf = convert_leaf(cmodel.leaf);
					cmodels[i].unk_0x60 = i ? 0xFFFF : 0;
				}

				new_asset->numSubModels = this->asset->numSubModels;
				new_asset->cmodels = cmodels;
			}

			void convert_stages(zonetool::s2::clipMap_t* new_asset) const
			{
				new_asset->stageCount = this->asset->stageCount;
				new_asset->stages = reinterpret_cast<zonetool::s2::Stage*>(this->asset->stages);
				new_asset->stageTrigger = mapents::convert_triggers(this->asset->stageTrigger, this->allocator);
			}
		};
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
		const auto new_asset = converter(pending_clip_map, allocator).convert();
		const auto zone = new_asset->info.trZones[0];
		zonetool::s2::clip_map_trzone::dump(zone);
		add_csv_row("col_map_trzone", zone->name);
		zonetool::s2::clip_map::dump(new_asset);
		add_csv_row("col_map_mp", new_asset->name);
		pending_clip_map = nullptr;
	}
}
