#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "xmodel.hpp"

#include <h1/assets/clipmap.hpp>

namespace zonetool::s2::converter::h1::clipmap
{
	namespace
	{
		constexpr auto transient_zone_count = 16u;

		clipMap_t* pending_clip_map = nullptr;
		std::array<ClipMapTrZone*, transient_zone_count> loaded_transient_zones{};

		template <typename T>
		T* to_array(std::vector<T>& values, utils::memory::allocator& allocator)
		{
			if (values.empty())
			{
				return nullptr;
			}

			const auto array = allocator.allocate_array<T>(values.size());
			std::memcpy(array, values.data(), sizeof(T) * values.size());
			return array;
		}

		std::array<ClipMapTrZone*, transient_zone_count> get_transient_zones(const clipMap_t* asset)
		{
			auto zones = loaded_transient_zones;
			for (auto i = 0u; i < transient_zone_count; i++)
			{
				if (asset->info.trZones[i] && asset->info.trZones[i]->bCollisionData.brushes)
				{
					zones[i] = asset->info.trZones[i];
				}
			}

			return zones;
		}

		void convert_materials(const ClipInfo& info, zonetool::h1::ClipInfo& new_info, utils::memory::allocator& allocator)
		{
			new_info.numMaterials = info.numMaterials;
			new_info.materials = allocator.allocate_array<zonetool::h1::ClipMaterial>(info.numMaterials);
			for (auto i = 0u; i < info.numMaterials; i++)
			{
				new_info.materials[i].name = info.materials[i].name;
				new_info.materials[i].surfaceFlags = static_cast<int>(convert_surface_flags(info.materials[i].surfaceFlags));
				new_info.materials[i].contents = static_cast<int>(info.materials[i].contents);
			}
		}

		zonetool::h1::Stage* default_stage(utils::memory::allocator& allocator)
		{
			const auto stage = allocator.allocate<zonetool::h1::Stage>();
			stage->name = "stage 0";
			stage->triggerIndex = 0x400;
			stage->sunPrimaryLightIndex = 1;
			stage->radiometricScale = 0.001f;
			return stage;
		}

		class converter
		{
		public:
			converter(clipMap_t* asset, utils::memory::allocator& allocator)
				: asset(asset)
				, info(asset->info)
				, allocator(allocator)
				, zones(get_transient_zones(asset))
			{
			}

			zonetool::h1::clipMap_t* convert()
			{
				const auto new_asset = this->allocator.allocate<zonetool::h1::clipMap_t>();
				auto& new_info = new_asset->info;
				new_asset->name = this->asset->name;

				new_info.planeCount = this->info.planeCount;
				new_info.planes = reinterpret_cast<zonetool::h1::cplane_s*>(this->info.planes);
				convert_materials(this->info, new_info, this->allocator);

				this->convert_brushes(new_info);
				this->convert_leaf_brush_nodes(new_info);
				this->convert_partitions(new_info);
				this->convert_leafs(new_asset);
				this->convert_static_models(new_info);

				new_info.pCollisionTree.aabbTreeCount = static_cast<int>(this->aabb_trees.size());
				new_info.pCollisionTree.aabbTrees = to_array(this->aabb_trees, this->allocator);

				new_asset->numNodes = this->asset->numNodes;
				new_asset->nodes = reinterpret_cast<zonetool::h1::cNode_t*>(this->asset->nodes);

				if (this->asset->mapEnts)
				{
					new_asset->mapEnts = this->allocator.allocate<zonetool::h1::MapEnts>();
					new_asset->mapEnts->name = this->asset->mapEnts->name;
				}

				new_asset->stageCount = 1;
				new_asset->stages = default_stage(this->allocator);
				return new_asset;
			}

		private:
			clipMap_t* asset;
			ClipInfo& info;
			utils::memory::allocator& allocator;
			std::array<ClipMapTrZone*, transient_zone_count> zones;

			std::array<std::uint32_t, transient_zone_count> partition_base{};
			std::vector<zonetool::h1::CollisionAabbTree> aabb_trees;
			std::map<std::uint32_t, std::vector<std::uint16_t>> subtree_materials;
			std::map<std::pair<std::uint32_t, std::uint16_t>, std::optional<zonetool::h1::CollisionAabbTree>> filtered_trees;
			std::map<std::pair<std::uint32_t, std::uint32_t>, std::pair<std::uint32_t, std::uint32_t>> converted_roots;

			void convert_brushes(zonetool::h1::ClipInfo& new_info)
			{
				const auto& table = this->info.transientZones;
				std::array<std::uint32_t, transient_zone_count> side_base{};
				std::vector<zonetool::h1::cbrushside_t> sides;
				for (auto z = 0u; z < transient_zone_count; z++)
				{
					side_base[z] = static_cast<std::uint32_t>(sides.size());
					if (const auto zone = this->zones[z])
					{
						const auto zone_sides = reinterpret_cast<const zonetool::h1::cbrushside_t*>(zone->bCollisionData.brushSides);
						sides.insert(sides.end(), zone_sides, zone_sides + zone->bCollisionData.numBrushSides);
					}
				}

				new_info.bCollisionData.numBrushSides = static_cast<unsigned int>(sides.size());
				new_info.bCollisionData.brushSides = to_array(sides, this->allocator);

				const auto brush_count = table.count ? table.zones[std::min(table.count, transient_zone_count) - 1].brushEnd : 0;
				auto& data = new_info.bCollisionData;
				data.numBrushes = brush_count;
				data.brushes = this->allocator.allocate_array<zonetool::h1::cbrush_t>(brush_count);
				data.brushBounds = this->allocator.allocate_array<zonetool::h1::Bounds>(brush_count);
				data.brushContents = this->allocator.allocate_array<int>(brush_count);

				for (auto z = 0u; z < std::min(table.count, transient_zone_count); z++)
				{
					const auto& record = table.zones[z];
					const auto zone = this->zones[z];
					if (!zone)
					{
						if (record.brushEnd > record.brushStart)
						{
							ZONETOOL_WARNING("clipMap \"%s\": transient zone %u is not loaded, %u brushes are empty", this->asset->name, z, record.brushEnd - record.brushStart);
						}

						continue;
					}

					const auto& zone_data = zone->bCollisionData;
					for (auto id = record.brushStart; id < record.brushEnd; id++)
					{
						const auto local = id - record.brushStart;
						if (local >= zone_data.numBrushes)
						{
							break;
						}

						const auto& brush = zone_data.brushes[local];
						auto& new_brush = data.brushes[id];
						new_brush.numsides = static_cast<unsigned short>(brush.numsides);
						new_brush.sides = brush.numsides ? &data.brushSides[side_base[z] + (brush.sides - zone_data.brushSides)] : nullptr;
						std::memcpy(new_brush.axialMaterialNum, brush.axialMaterialNum, sizeof(new_brush.axialMaterialNum));
						data.brushBounds[id] = std::bit_cast<zonetool::h1::Bounds>(zone_data.brushBounds[local]);
						data.brushContents[id] = static_cast<int>(zone_data.brushContents[local]);
					}
				}
			}

			void convert_leaf_brush_nodes(zonetool::h1::ClipInfo& new_info)
			{
				auto& tree = new_info.bCollisionTree;
				const auto& brush_tree = this->info.bCollisionTree;
				tree.numLeafBrushes = brush_tree.numLeafBrushes;
				tree.leafbrushes = this->allocator.allocate_array<zonetool::h1::LeafBrush>(tree.numLeafBrushes);
				for (auto i = 0u; i < tree.numLeafBrushes; i++)
				{
					tree.leafbrushes[i] = static_cast<zonetool::h1::LeafBrush>(brush_tree.leafbrushes[i]);
				}

				tree.leafbrushNodesCount = brush_tree.leafbrushNodesCount;
				tree.leafbrushNodes = this->allocator.allocate_array<zonetool::h1::cLeafBrushNode_s>(tree.leafbrushNodesCount);
				for (auto i = 0u; i < tree.leafbrushNodesCount; i++)
				{
					const auto& node = brush_tree.leafbrushNodes[i];
					auto& new_node = tree.leafbrushNodes[i];
					new_node.axis = node.axis;
					new_node.leafBrushCount = static_cast<short>(node.leafBrushCount);
					new_node.contents = static_cast<int>(node.contents);
					if (node.leafBrushCount > 0)
					{
						new_node.data.leaf.brushes = node.data.leaf.brushes ? &tree.leafbrushes[node.data.leaf.brushes - brush_tree.leafbrushes] : nullptr;
					}
					else
					{
						new_node.data.children = std::bit_cast<zonetool::h1::cLeafBrushNodeChildren_t>(node.data.children);
					}
				}
			}

			void convert_partitions(zonetool::h1::ClipInfo& new_info)
			{
				std::vector<std::array<float, 3>> verts;
				std::vector<std::uint16_t> tri_indices;
				std::vector<std::uint8_t> walkable_bits;
				std::vector<zonetool::h1::CollisionBorder> borders;
				std::vector<zonetool::h1::CollisionPartition> partitions;
				std::vector<std::pair<std::uint32_t, std::int32_t>> partition_borders;

				for (auto z = 0u; z < transient_zone_count; z++)
				{
					this->partition_base[z] = static_cast<std::uint32_t>(partitions.size());
					const auto zone = this->zones[z];
					if (!zone)
					{
						continue;
					}

					const auto& mesh = zone->pCollisionData;
					verts.resize((verts.size() + 4095) / 4096 * 4096);
					const auto segment_base = static_cast<std::uint32_t>(verts.size() / 1024);
					const auto tri_base = static_cast<std::uint32_t>(tri_indices.size() / 3);
					const auto border_base = static_cast<std::uint32_t>(borders.size());

					const auto zone_verts = reinterpret_cast<const std::array<float, 3>*>(mesh.verts);
					verts.insert(verts.end(), zone_verts, zone_verts + mesh.vertCount);
					tri_indices.insert(tri_indices.end(), mesh.triIndices, mesh.triIndices + 3ull * mesh.triCount);
					walkable_bits.resize(tri_indices.size());
					for (auto bit = 0u; bit < 3 * mesh.triCount; bit++)
					{
						walkable_bits[3ull * tri_base + bit] = (mesh.triEdgeIsWalkable[bit >> 3] >> (bit & 7)) & 1;
					}

					const auto zone_borders = reinterpret_cast<const zonetool::h1::CollisionBorder*>(mesh.borders);
					borders.insert(borders.end(), zone_borders, zone_borders + mesh.borderCount);

					for (auto i = 0u; i < mesh.partitionCount; i++)
					{
						const auto& partition = mesh.partitions[i];
						auto& new_partition = partitions.emplace_back();
						new_partition.triCount = partition.triCount;
						new_partition.borderCount = partition.borderCount;
						new_partition.firstVertSegment = static_cast<unsigned char>(segment_base + partition.firstVertSegment * 4);
						new_partition.firstTri = static_cast<int>(tri_base + partition.firstTri);
						partition_borders.emplace_back(border_base, partition.borderCount ? partition.firstBorder : -1);
					}
				}

				if (verts.size() > 1024 * 256ull)
				{
					ZONETOOL_ERROR("clipMap \"%s\": %zu collision vertices do not fit 256 segments", this->asset->name, verts.size());
				}

				auto& data = new_info.pCollisionData;
				data.vertCount = static_cast<unsigned int>(verts.size());
				data.verts = reinterpret_cast<zonetool::h1::vec3_t*>(to_array(verts, this->allocator));
				data.triCount = static_cast<int>(tri_indices.size() / 3);
				data.triIndices = to_array(tri_indices, this->allocator);
				data.triEdgeIsWalkable = this->allocator.allocate_array<unsigned char>(((3ull * data.triCount + 31) >> 3) & ~3ull);
				for (auto bit = 0u; bit < walkable_bits.size(); bit++)
				{
					data.triEdgeIsWalkable[bit >> 3] |= static_cast<unsigned char>(walkable_bits[bit] << (bit & 7));
				}

				data.borderCount = static_cast<int>(borders.size());
				data.borders = to_array(borders, this->allocator);
				data.partitionCount = static_cast<int>(partitions.size());
				data.partitions = to_array(partitions, this->allocator);
				for (auto i = 0u; i < partitions.size(); i++)
				{
					const auto [border_base, first_border] = partition_borders[i];
					data.partitions[i].borders = first_border >= 0 ? &data.borders[border_base + first_border] : nullptr;
				}
			}

			const CollisionAabbTree& aabb_node(const std::uint32_t index) const
			{
				return this->info.pCollisionTree.aabbTrees[index];
			}

			const CollisionPartition* partition_of(const CollisionAabbTree& node) const
			{
				if (node.transientZone >= transient_zone_count || !this->zones[node.transientZone])
				{
					return nullptr;
				}

				const auto& mesh = this->zones[node.transientZone]->pCollisionData;
				if (node.u.partitionIndex >= mesh.partitionCount)
				{
					return nullptr;
				}

				return &mesh.partitions[node.u.partitionIndex];
			}

			const std::vector<std::uint16_t>& materials_of(const std::uint32_t index)
			{
				if (const auto itr = this->subtree_materials.find(index); itr != this->subtree_materials.end())
				{
					return itr->second;
				}

				std::vector<std::uint16_t> materials;
				const auto& node = this->aabb_node(index);
				if (node.childCount)
				{
					for (auto i = 0u; i < node.childCount; i++)
					{
						const auto& child = this->materials_of(node.u.firstChildIndex + i);
						materials.insert(materials.end(), child.begin(), child.end());
					}

					std::sort(materials.begin(), materials.end());
					materials.erase(std::unique(materials.begin(), materials.end()), materials.end());
				}
				else if (const auto partition = this->partition_of(node))
				{
					materials.emplace_back(static_cast<std::uint16_t>(partition->materialIndex));
				}

				return this->subtree_materials[index] = std::move(materials);
			}

			zonetool::h1::CollisionAabbTree make_aabb_node(const CollisionAabbTree& node, const std::uint16_t material) const
			{
				zonetool::h1::CollisionAabbTree new_node{};
				std::memcpy(new_node.midPoint, node.midPoint, sizeof(new_node.midPoint));
				std::memcpy(new_node.halfSize, node.halfSize, sizeof(new_node.halfSize));
				new_node.materialIndex = material;
				return new_node;
			}

			std::optional<zonetool::h1::CollisionAabbTree> filtered_tree(const std::uint32_t index, const std::uint16_t material)
			{
				const auto key = std::make_pair(index, material);
				if (const auto itr = this->filtered_trees.find(key); itr != this->filtered_trees.end())
				{
					return itr->second;
				}

				std::optional<zonetool::h1::CollisionAabbTree> result;
				const auto& node = this->aabb_node(index);
				if (node.childCount)
				{
					std::vector<zonetool::h1::CollisionAabbTree> children;
					for (auto i = 0u; i < node.childCount; i++)
					{
						if (const auto child = this->filtered_tree(node.u.firstChildIndex + i, material))
						{
							children.emplace_back(*child);
						}
					}

					if (children.size() == 1)
					{
						result = children.front();
					}
					else if (!children.empty())
					{
						result = this->make_aabb_node(node, material);
						result->childCount = static_cast<unsigned short>(children.size());
						result->u.firstChildIndex = static_cast<int>(this->aabb_trees.size());
						this->aabb_trees.insert(this->aabb_trees.end(), children.begin(), children.end());
					}
				}
				else if (const auto partition = this->partition_of(node); partition && partition->materialIndex == material)
				{
					result = this->make_aabb_node(node, material);
					result->u.partitionIndex = static_cast<int>(this->partition_base[node.transientZone] + node.u.partitionIndex);
				}

				return this->filtered_trees[key] = result;
			}

			std::pair<std::uint32_t, std::uint32_t> convert_roots(const std::uint32_t first, const std::uint32_t count)
			{
				const auto key = std::make_pair(first, count);
				if (const auto itr = this->converted_roots.find(key); itr != this->converted_roots.end())
				{
					return itr->second;
				}

				std::vector<zonetool::h1::CollisionAabbTree> roots;
				for (auto root = first; root < first + count && root < static_cast<std::uint32_t>(this->info.pCollisionTree.aabbTreeCount); root++)
				{
					const auto materials = this->materials_of(root);
					for (const auto material : materials)
					{
						if (const auto tree = this->filtered_tree(root, material))
						{
							roots.emplace_back(*tree);
						}
					}
				}

				const auto result = std::make_pair(static_cast<std::uint32_t>(this->aabb_trees.size()), static_cast<std::uint32_t>(roots.size()));
				this->aabb_trees.insert(this->aabb_trees.end(), roots.begin(), roots.end());
				return this->converted_roots[key] = result;
			}

			zonetool::h1::cLeaf_t convert_leaf(const cLeaf_t& leaf)
			{
				zonetool::h1::cLeaf_t new_leaf{};
				const auto [first, count] = leaf.collAabbCount ? this->convert_roots(leaf.firstCollAabbIndex, leaf.collAabbCount) : std::make_pair(0u, 0u);
				new_leaf.firstCollAabbIndex = first;
				new_leaf.collAabbCount = count;
				new_leaf.brushContents = static_cast<int>(leaf.brushContents);
				new_leaf.terrainContents = count ? static_cast<int>(leaf.terrainContents) : 0;
				new_leaf.bounds = std::bit_cast<zonetool::h1::Bounds>(leaf.bounds);
				new_leaf.leafBrushNode = leaf.leafBrushNode;
				return new_leaf;
			}

			void convert_leafs(zonetool::h1::clipMap_t* new_asset)
			{
				new_asset->numLeafs = this->asset->numLeafs;
				new_asset->leafs = this->allocator.allocate_array<zonetool::h1::cLeaf_t>(this->asset->numLeafs);
				for (auto i = 0u; i < this->asset->numLeafs; i++)
				{
					new_asset->leafs[i] = this->convert_leaf(this->asset->leafs[i]);
				}

				new_asset->numSubModels = this->asset->numSubModels;
				new_asset->cmodels = this->allocator.allocate_array<zonetool::h1::cmodel_t>(this->asset->numSubModels);
				for (auto i = 0u; i < this->asset->numSubModels; i++)
				{
					auto& new_cmodel = new_asset->cmodels[i];
					const auto& cmodel = this->asset->cmodels[i];
					new_cmodel.bounds = std::bit_cast<zonetool::h1::Bounds>(cmodel.bounds);
					new_cmodel.radius = cmodel.radius;
					new_cmodel.leaf = this->convert_leaf(cmodel.leaf);
				}
			}

			static void add_bounds(float (&mins)[3], float (&maxs)[3], const zonetool::h1::Bounds& bounds)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					mins[axis] = std::min(mins[axis], bounds.midPoint[axis] - bounds.halfSize[axis]);
					maxs[axis] = std::max(maxs[axis], bounds.midPoint[axis] + bounds.halfSize[axis]);
				}
			}

			static zonetool::h1::Bounds make_bounds(const float (&mins)[3], const float (&maxs)[3])
			{
				zonetool::h1::Bounds bounds{};
				for (auto axis = 0; axis < 3; axis++)
				{
					bounds.midPoint[axis] = (mins[axis] + maxs[axis]) * 0.5f;
					bounds.halfSize[axis] = (maxs[axis] - mins[axis]) * 0.5f;
				}

				return bounds;
			}

			void build_smodel_node(std::vector<zonetool::h1::cStaticModel_s>& models, std::vector<zonetool::h1::SModelAabbNode>& nodes,
				const std::size_t node_index, const std::size_t begin, const std::size_t end)
			{
				float mins[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
				float maxs[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
				for (auto i = begin; i < end; i++)
				{
					add_bounds(mins, maxs, models[i].absBounds);
				}

				nodes[node_index].bounds = make_bounds(mins, maxs);
				if (end - begin <= 16)
				{
					nodes[node_index].firstChild = static_cast<unsigned short>(begin);
					nodes[node_index].childCount = static_cast<unsigned short>(end - begin);
					return;
				}

				auto axis = 0;
				for (auto i = 1; i < 3; i++)
				{
					if (maxs[i] - mins[i] > maxs[axis] - mins[axis])
					{
						axis = i;
					}
				}

				const auto middle = begin + (end - begin) / 2;
				std::nth_element(models.begin() + begin, models.begin() + middle, models.begin() + end, [axis](const auto& a, const auto& b)
				{
					return a.absBounds.midPoint[axis] < b.absBounds.midPoint[axis];
				});

				const auto first_child = nodes.size();
				nodes.resize(nodes.size() + 2);
				nodes[node_index].firstChild = static_cast<unsigned short>(models.size() + first_child);
				nodes[node_index].childCount = 2;
				this->build_smodel_node(models, nodes, first_child, begin, middle);
				this->build_smodel_node(models, nodes, first_child + 1, middle, end);
			}

			void convert_static_models(zonetool::h1::ClipInfo& new_info)
			{
				std::vector<zonetool::h1::cStaticModel_s> models;
				for (const auto zone : this->zones)
				{
					if (!zone)
					{
						continue;
					}

					for (auto i = 0u; i < zone->staticModels.count; i++)
					{
						const auto& model = zone->staticModels.models[i];
						if (!model.model || !xmodel::is_dumped(model.model->name))
						{
							continue;
						}

						auto& new_model = models.emplace_back();
						std::memcpy(&new_model, &model, sizeof(new_model));
						new_model.xmodel = this->allocator.allocate<zonetool::h1::XModel>();
						new_model.xmodel->name = model.model->name;
						new_model.lightingHandle = 0;
					}
				}

				std::vector<zonetool::h1::SModelAabbNode> nodes;
				if (!models.empty())
				{
					nodes.resize(1);
					this->build_smodel_node(models, nodes, 0, 0, models.size());
				}

				if (models.size() + nodes.size() > std::numeric_limits<unsigned short>::max())
				{
					ZONETOOL_ERROR("clipMap \"%s\": %zu static models and %zu nodes do not fit 16-bit ids", this->asset->name, models.size(), nodes.size());
				}

				new_info.sCollisionData.numStaticModels = static_cast<unsigned int>(models.size());
				new_info.sCollisionData.staticModelList = to_array(models, this->allocator);
				new_info.sCollisionTree.numStaticModels = static_cast<unsigned short>(models.size());
				new_info.sCollisionTree.smodelNodeCount = static_cast<unsigned short>(nodes.size());
				new_info.sCollisionTree.smodelNodes = to_array(nodes, this->allocator);
			}
		};
	}

	void dump(clipMap_t* asset)
	{
		pending_clip_map = asset;
	}

	void add_transient_zone(ClipMapTrZone* zone)
	{
		if (zone->index >= transient_zone_count)
		{
			ZONETOOL_WARNING("ClipMapTrZone \"%s\" has no valid slot (%u)", zone->name, zone->index);
			return;
		}

		loaded_transient_zones[zone->index] = zone;
	}

	void finish()
	{
		if (!pending_clip_map)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto new_asset = converter(pending_clip_map, allocator).convert();
		const auto& info = new_asset->info;
		ZONETOOL_INFO("clipMap: %u brushes, %u leafbrush nodes, %d partitions, %u verts, %d aabb trees, %u static models, %u smodel nodes",
			info.bCollisionData.numBrushes, info.bCollisionTree.leafbrushNodesCount, info.pCollisionData.partitionCount, info.pCollisionData.vertCount,
			info.pCollisionTree.aabbTreeCount, info.sCollisionData.numStaticModels, info.sCollisionTree.smodelNodeCount);
		zonetool::h1::clip_map::dump(new_asset);
		add_csv_row("col_map_mp", new_asset->name);
		pending_clip_map = nullptr;
		loaded_transient_zones = {};
	}
}
