#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "mapents.hpp"

namespace zonetool::h1::converter::s2::clipmap
{
	namespace
	{
		namespace target = zonetool::s2;

		// H1 uses verts + seg * 1024 while S2 uses verts + seg * 4096
		constexpr auto h1_vertex_segment_size = 1024u;
		constexpr auto segment_ratio = 4u;
		constexpr std::uint8_t transient_zone = 0;
		constexpr std::uint8_t no_transient_zone = 0xFF;
		constexpr auto transient_zone_table_offset = 0x60;
		constexpr auto smodel_tree_offset = 0x50;
		constexpr auto checksum_offset = 0x3B0;

		struct s2_material
		{
			const char* name;
			std::uint32_t surfaceFlags;
			char __pad0[4];
			std::uint64_t contents;
		};
		static_assert(sizeof(s2_material) == sizeof(target::ClipMaterial));

		struct s2_leaf_brush_node
		{
			std::uint8_t axis;
			std::uint8_t transientZone;
			char __pad0[2];
			std::int32_t leafBrushCount;
			std::uint64_t contents;
			union
			{
				std::uint32_t* brushes;
				cLeafBrushNodeChildren_t children;
			} data;
		};
		static_assert(sizeof(s2_leaf_brush_node) == sizeof(target::cbrushside_t));

		struct s2_aabb_node
		{
			float midPoint[3];
			std::uint8_t childCount;
			std::uint8_t transientZone;
			char __pad0[2];
			float halfSize[3];
			std::uint32_t index;
		};
		static_assert(sizeof(s2_aabb_node) == 32);

		struct s2_smodel_node
		{
			Bounds bounds;
			std::uint32_t first;
			std::uint8_t count;
			std::uint8_t transientZone;
			char __pad0[2];
		};
		static_assert(sizeof(s2_smodel_node) == 32);

		struct s2_smodel_tree
		{
			std::uint16_t numStaticModels;
			std::uint16_t nodeCount;
			char __pad0[4];
			s2_smodel_node* nodes;
		};
		static_assert(sizeof(s2_smodel_tree) == sizeof(target::ClipLeafBrushes));

		struct s2_node
		{
			cplane_s* plane;
			std::int16_t children[2];
			std::uint8_t transientZone;
			char __pad0[3];
		};
		static_assert(sizeof(s2_node) == sizeof(target::cNode_t));

		struct s2_leaf
		{
			std::uint32_t firstCollAabbIndex;
			std::uint32_t collAabbCount;
			std::uint64_t brushContents;
			std::uint64_t terrainContents;
			Bounds bounds;
			std::uint32_t leafBrushNode;
			std::uint8_t brushTransientZone;
			std::uint8_t aabbTransientZone;
			char __pad0[2];
		};
		static_assert(sizeof(s2_leaf) == 56);

		struct s2_cmodel
		{
			Bounds bounds;
			float radius;
			char __pad0[4];
			target::ClipInfo* info;
			s2_leaf leaf;
			std::uint16_t unk60;
			char __pad1[6];
		};
		static_assert(sizeof(s2_cmodel) == sizeof(target::cmodel_t));

		struct s2_brush
		{
			cbrushside_t* sides;
			std::uint32_t numSides;
			short axialMaterialNum[2][3];
		};
		static_assert(sizeof(s2_brush) == sizeof(target::ClipMapTrZoneNode));

		struct s2_partition
		{
			std::uint8_t triCount;
			std::uint8_t borderCount;
			std::uint8_t firstVertSegment;
			char __pad0;
			std::int32_t firstTri;
			std::int32_t firstBorder;
			std::uint32_t materialIndex;
		};
		static_assert(sizeof(s2_partition) == 16);

		struct s2_transient_zone_record
		{
			std::uint32_t brushStart;
			std::uint32_t brushEnd;
			std::uint32_t __unknown[5];
		};

		struct s2_transient_zone_table
		{
			std::uint32_t count;
			s2_transient_zone_record records[16];
		};
		static_assert(transient_zone_table_offset + sizeof(s2_transient_zone_table) <= offsetof(target::ClipInfo, trZones));

		static_assert(sizeof(cplane_s) == sizeof(target::cplane_s));
		static_assert(sizeof(cbrushside_t) == sizeof(std::uint64_t));
		static_assert(sizeof(CollisionBorder) == 28);
		static_assert(sizeof(cStaticModel_s) == sizeof(target::ClipMapTrZoneStaticModel));
		static_assert(sizeof(Stage) == sizeof(target::ClipStaticModelName));
		static_assert(sizeof(MapTriggers) == sizeof(target::ClipStaticModelData));

		clipMap_t* pending_clip_map = nullptr;

		std::string transient_zone_name(const std::string& clip_name)
		{
			auto base = clip_name.substr(clip_name.find_last_of('/') + 1);
			base = base.substr(0, base.find(".d3dbsp"));
			return base + "_000";
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

			target::clipMap_t* convert()
			{
				const auto new_asset = this->allocator.allocate<target::clipMap_t>();
				new_asset->name = this->asset->name;

				auto& new_info = new_asset->info;
				new_info.planeCount = this->info.planeCount;
				new_info.planes = reinterpret_cast<target::cplane_s*>(this->info.planes);
				this->convert_materials(new_info);
				this->convert_leaf_brush_nodes(new_info);
				this->convert_aabb_trees(new_info);
				this->convert_smodel_tree(new_info);

				auto& table = field<s2_transient_zone_table>(&new_info, transient_zone_table_offset);
				table.count = 1;
				table.records[0].brushStart = 0;
				table.records[0].brushEnd = this->info.bCollisionData.numBrushes;

				new_info.trZones[0] = this->convert_transient_zone();

				new_asset->pInfo = this->allocator.allocate<target::ClipInfo>();
				*new_asset->pInfo = new_info;

				this->convert_nodes(new_asset);
				this->convert_leafs(new_asset);
				this->convert_stages(new_asset);
				new_asset->mapEnts = reinterpret_cast<target::MapEnts*>(this->asset->mapEnts);
				field<std::uint32_t>(new_asset, checksum_offset) = this->asset->checksum;

				if (this->asset->dynEntCount[0] || this->asset->dynEntCount[1])
				{
					ZONETOOL_WARNING("clipMap \"%s\": %u dynamic entities are not converted", this->asset->name,
						this->asset->dynEntCount[0] + this->asset->dynEntCount[1]);
				}

				return new_asset;
			}

		private:
			clipMap_t* asset;
			ClipInfo& info;
			utils::memory::allocator& allocator;
			std::vector<std::uint32_t> partition_materials;

			void convert_materials(target::ClipInfo& new_info) const
			{
				const auto materials = this->allocator.allocate_array<s2_material>(this->info.numMaterials);
				for (auto i = 0u; i < this->info.numMaterials; i++)
				{
					const auto& material = this->info.materials[i];
					materials[i].name = material.name;
					materials[i].surfaceFlags = convert_surface_flags(static_cast<std::uint32_t>(material.surfaceFlags));
					materials[i].contents = static_cast<std::uint32_t>(material.contents);
				}

				new_info.numMaterials = this->info.numMaterials;
				new_info.materials = reinterpret_cast<target::ClipMaterial*>(materials);
			}

			void convert_leaf_brush_nodes(target::ClipInfo& new_info) const
			{
				const auto& tree = this->info.bCollisionTree;
				const auto indices = this->allocator.allocate_array<std::uint32_t>(tree.numLeafBrushes);
				for (auto i = 0u; i < tree.numLeafBrushes; i++)
				{
					indices[i] = tree.leafbrushes[i];
				}

				const auto nodes = this->allocator.allocate_array<s2_leaf_brush_node>(tree.leafbrushNodesCount);
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
						new_node.data.brushes = this->convert_leaf_brushes(node.data.leaf.brushes, node.leafBrushCount, indices);
					}
					else
					{
						new_node.data.children = node.data.children;
					}
				}

				new_info.brushes.sideCount = tree.leafbrushNodesCount;
				new_info.brushes.sides = reinterpret_cast<target::cbrushside_t*>(nodes);
				new_info.brushes.indexCount = tree.numLeafBrushes;
				new_info.brushes.indices = reinterpret_cast<int*>(indices);
			}

			// Zones load the brushes of each leaf node inline, so they do not always point into leafbrushes
			std::uint32_t* convert_leaf_brushes(const LeafBrush* brushes, const std::int32_t count, std::uint32_t* indices) const
			{
				if (!brushes)
				{
					return nullptr;
				}

				const auto& tree = this->info.bCollisionTree;
				if (brushes >= tree.leafbrushes && brushes + count <= tree.leafbrushes + tree.numLeafBrushes)
				{
					return &indices[brushes - tree.leafbrushes];
				}

				const auto result = this->allocator.allocate_array<std::uint32_t>(count);
				for (auto i = 0; i < count; i++)
				{
					result[i] = brushes[i];
				}

				return result;
			}

			void convert_aabb_trees(target::ClipInfo& new_info)
			{
				const auto& tree = this->info.pCollisionTree;
				this->partition_materials.assign(this->info.pCollisionData.partitionCount, 0);

				const auto nodes = this->allocator.allocate_array<s2_aabb_node>(tree.aabbTreeCount);
				for (auto i = 0; i < tree.aabbTreeCount; i++)
				{
					const auto& node = tree.aabbTrees[i];
					auto& new_node = nodes[i];
					std::memcpy(new_node.midPoint, node.midPoint, sizeof(new_node.midPoint));
					std::memcpy(new_node.halfSize, node.halfSize, sizeof(new_node.halfSize));
					new_node.transientZone = transient_zone;
					new_node.index = static_cast<std::uint32_t>(node.u.firstChildIndex);
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

				new_info.brushBounds.count = tree.aabbTreeCount;
				new_info.brushBounds.data = reinterpret_cast<char(*)[32]>(nodes);
			}

			void convert_smodel_tree(target::ClipInfo& new_info) const
			{
				const auto& tree = this->info.sCollisionTree;
				const auto nodes = this->allocator.allocate_array<s2_smodel_node>(tree.smodelNodeCount);
				for (auto i = 0u; i < tree.smodelNodeCount; i++)
				{
					const auto& node = tree.smodelNodes[i];
					nodes[i].bounds = node.bounds;
					nodes[i].first = node.firstChild;
					nodes[i].count = static_cast<std::uint8_t>(node.childCount);
					nodes[i].transientZone = transient_zone;
				}

				auto& new_tree = field<s2_smodel_tree>(&new_info, smodel_tree_offset);
				new_tree.numStaticModels = tree.numStaticModels;
				new_tree.nodeCount = tree.smodelNodeCount;
				new_tree.nodes = nodes;
			}

			target::ClipMapTrZone* convert_transient_zone()
			{
				const auto zone = this->allocator.allocate<target::ClipMapTrZone>();
				zone->name = this->allocator.duplicate_string(transient_zone_name(this->asset->name));
				this->convert_brushes(zone->tree);
				this->convert_mesh(zone->mesh);

				const auto& models = this->info.sCollisionData;
				const auto new_models = this->allocator.allocate_array<target::ClipMapTrZoneStaticModel>(models.numStaticModels);
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

			void convert_brushes(target::ClipMapTrZoneTree& tree) const
			{
				const auto& data = this->info.bCollisionData;
				const auto brushes = this->allocator.allocate_array<s2_brush>(data.numBrushes);
				const auto contents = this->allocator.allocate_array<std::uint64_t>(data.numBrushes);
				for (auto i = 0u; i < data.numBrushes; i++)
				{
					const auto& brush = data.brushes[i];
					brushes[i].sides = brush.numsides ? brush.sides : nullptr;
					brushes[i].numSides = brush.numsides;
					std::memcpy(brushes[i].axialMaterialNum, brush.axialMaterialNum, sizeof(brush.axialMaterialNum));
					contents[i] = static_cast<std::uint32_t>(data.brushContents[i]);
				}

				tree.nodeRefs = reinterpret_cast<std::uint64_t*>(data.brushSides);
				tree.nodeRefCount = data.numBrushSides;
				tree.nodeCount = data.numBrushes;
				tree.nodes = reinterpret_cast<target::ClipMapTrZoneNode*>(brushes);
				tree.bounds = reinterpret_cast<char(*)[24]>(data.brushBounds);
				tree.nodeData = contents;
			}

			void convert_mesh(target::ClipMapTrZoneMesh& mesh) const
			{
				const auto& data = this->info.pCollisionData;
				const auto tri_indices = this->allocator.allocate_array<std::uint16_t>(3ull * data.triCount);
				std::memcpy(tri_indices, data.triIndices, sizeof(std::uint16_t) * 3 * data.triCount);

				std::vector<bool> converted_tris(data.triCount);
				auto overflow_count = 0u;
				const auto partitions = this->allocator.allocate_array<s2_partition>(data.partitionCount);
				for (auto i = 0; i < data.partitionCount; i++)
				{
					const auto& partition = data.partitions[i];
					auto& new_partition = partitions[i];
					new_partition.triCount = partition.triCount;
					new_partition.borderCount = partition.borderCount;
					new_partition.firstVertSegment = static_cast<std::uint8_t>(partition.firstVertSegment / segment_ratio);
					new_partition.firstTri = partition.firstTri;
					new_partition.materialIndex = this->partition_materials[i];

					const auto border = partition.borders ? partition.borders - data.borders : -1;
					new_partition.firstBorder = border >= 0 && border < data.borderCount ? static_cast<std::int32_t>(border) : -1;

					const auto index_offset = (partition.firstVertSegment % segment_ratio) * h1_vertex_segment_size;
					for (auto tri = partition.firstTri; tri < partition.firstTri + partition.triCount && tri < data.triCount; tri++)
					{
						if (converted_tris[tri])
						{
							continue;
						}

						converted_tris[tri] = true;
						for (auto corner = 0; corner < 3; corner++)
						{
							const auto index = tri_indices[3 * tri + corner] + index_offset;
							overflow_count += index > std::numeric_limits<std::uint16_t>::max();
							tri_indices[3 * tri + corner] = static_cast<std::uint16_t>(index);
						}
					}
				}

				if (overflow_count)
				{
					ZONETOOL_WARNING("clipMap \"%s\": %u collision tri indices do not fit 16 bits", this->asset->name, overflow_count);
				}

				mesh.vertCount = data.vertCount;
				mesh.verts = reinterpret_cast<target::vec3_t*>(data.verts);
				mesh.triCount = data.triCount;
				mesh.triIndices = tri_indices;
				mesh.triEdgeIsWalkable = data.triEdgeIsWalkable;
				mesh.partitionCount = data.borderCount;
				mesh.partitions = reinterpret_cast<char(*)[28]>(data.borders);
				mesh.aabbCount = data.partitionCount;
				mesh.aabbs = reinterpret_cast<char(*)[16]>(partitions);
			}

			void convert_nodes(target::clipMap_t* new_asset) const
			{
				const auto nodes = this->allocator.allocate_array<s2_node>(this->asset->numNodes);
				for (auto i = 0u; i < this->asset->numNodes; i++)
				{
					nodes[i].plane = this->asset->nodes[i].plane;
					nodes[i].children[0] = this->asset->nodes[i].children[0];
					nodes[i].children[1] = this->asset->nodes[i].children[1];
					nodes[i].transientZone = no_transient_zone;
				}

				new_asset->numNodes = this->asset->numNodes;
				new_asset->nodes = reinterpret_cast<target::cNode_t*>(nodes);
			}

			static s2_leaf convert_leaf(const cLeaf_t& leaf)
			{
				s2_leaf new_leaf{};
				new_leaf.firstCollAabbIndex = leaf.firstCollAabbIndex;
				new_leaf.collAabbCount = leaf.collAabbCount;
				new_leaf.brushContents = static_cast<std::uint32_t>(leaf.brushContents);
				new_leaf.terrainContents = static_cast<std::uint32_t>(leaf.terrainContents);
				new_leaf.bounds = leaf.bounds;
				new_leaf.leafBrushNode = static_cast<std::uint32_t>(leaf.leafBrushNode);
				new_leaf.brushTransientZone = leaf.brushContents ? transient_zone : no_transient_zone;
				new_leaf.aabbTransientZone = leaf.collAabbCount ? transient_zone : no_transient_zone;
				return new_leaf;
			}

			void convert_leafs(target::clipMap_t* new_asset) const
			{
				const auto leafs = this->allocator.allocate_array<s2_leaf>(this->asset->numLeafs);
				for (auto i = 0u; i < this->asset->numLeafs; i++)
				{
					leafs[i] = convert_leaf(this->asset->leafs[i]);
				}

				new_asset->numLeafs = this->asset->numLeafs;
				new_asset->leafs = reinterpret_cast<char(*)[56]>(leafs);

				const auto cmodels = this->allocator.allocate_array<s2_cmodel>(this->asset->numSubModels);
				for (auto i = 0u; i < this->asset->numSubModels; i++)
				{
					const auto& cmodel = this->asset->cmodels[i];
					cmodels[i].bounds = cmodel.bounds;
					cmodels[i].radius = cmodel.radius;
					cmodels[i].info = new_asset->pInfo;
					cmodels[i].leaf = convert_leaf(cmodel.leaf);
					cmodels[i].unk60 = i ? 0xFFFF : 0;
				}

				new_asset->numSubModels = this->asset->numSubModels;
				new_asset->cmodels = reinterpret_cast<target::cmodel_t*>(cmodels);
			}

			void convert_stages(target::clipMap_t* new_asset) const
			{
				new_asset->staticModelNameCount = this->asset->stageCount;
				new_asset->staticModelNames = reinterpret_cast<target::ClipStaticModelName*>(this->asset->stages);

				const auto trigger = mapents::convert_triggers(this->asset->stageTrigger, this->allocator);
				std::memcpy(&new_asset->staticModelData, &trigger, sizeof(trigger));
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
		target::clip_map_trzone::dump(zone);
		add_csv_row("col_map_trzone", zone->name);
		target::clip_map::dump(new_asset);
		add_csv_row("col_map_mp", new_asset->name);
		pending_clip_map = nullptr;
	}
}
