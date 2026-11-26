#include <std_include.hpp>

#include "../include.hpp"
#include "collisiontile.hpp"
#include "havok.hpp"

#include <iw8/assets/collisiontile.hpp>

namespace zonetool::s4::converter::iw8::collisiontile
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::StaticModelCollisionCompressedModelList* convert_static_models(const StaticModelCollisionCompressedModelList* list, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(target::StaticModelCollisionCompressedInstance) == sizeof(StaticModelCollisionCompressedInstance));

			if (!list)
			{
				return nullptr;
			}

			const auto result = allocator.allocate<target::StaticModelCollisionCompressedModelList>();
			result->numModels = list->numModels;
			result->structureMemoryCost = list->structureMemoryCost;
			result->numInstances = list->numInstances;
			result->assetCost = list->assetCost;
			result->models = allocator.allocate_array<target::StaticModelCollisionCompressedModel>(list->numModels);
			for (auto i = 0u; list->models && i < list->numModels; i++)
			{
				const auto& model = list->models[i];
				auto& converted = result->models[i];
				converted.name = static_cast<target::scr_string_t>(model.name);
				converted.physicsAsset = name_reference<target::PhysicsAsset>(model.physicsAsset, allocator);
				converted.detailCollision = name_reference<target::XModelDetailCollision>(model.detailCollision, allocator);
				converted.shapeTags[0] = model.shapeTags[0];
				converted.shapeTags[1] = model.shapeTags[1];
				converted.numInstances = model.numInstances;
				converted.instances = reinterpret_cast<target::StaticModelCollisionCompressedInstance*>(model.instances);
			}

			return result;
		}
	}

	void dump(CollisionTile* asset)
	{
		const auto shape_data = havok::convert_shape_list(std::string_view(asset->havokTileShapeData, asset->havokTileShapeDataSize), asset->name);
		if (!shape_data)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto tile = allocator.allocate<target::CollisionTile>();
		tile->name = asset->name;
		tile->index = asset->index;
		tile->gridId[0] = asset->gridId[0];
		tile->gridId[1] = asset->gridId[1];
		std::memcpy(tile->origin, asset->origin, sizeof(tile->origin));
		tile->staticModelCollision = convert_static_models(asset->staticModelCollision, allocator);
		tile->havokTileShapeDataSize = static_cast<unsigned int>(shape_data->size());
		tile->havokTileShapeData = allocator.allocate_array<char>(shape_data->size());
		std::memcpy(tile->havokTileShapeData, shape_data->data(), shape_data->size());
		tile->worldCollisionShapeIdx = asset->worldCollisionShapeIdx;

		target::collision_tile::dump(tile);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_COLLISION_TILE), asset->name);
	}
}
