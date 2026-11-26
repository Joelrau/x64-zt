#include <std_include.hpp>

#include "../include.hpp"
#include "physics.hpp"

#include "havok.hpp"

#include <iw8/assets/physics.hpp>

namespace zonetool::s4::converter::iw8::physics
{
	namespace target = zonetool::iw8;

	unsigned int convert_contents(const unsigned int contents)
	{
		if ((contents & PHYSICS_CONTENTS_SOLID_MASK) != PHYSICS_CONTENTS_SOLID_MASK)
		{
			return contents;
		}

		return (contents & ~PHYSICS_CONTENTS_SOLID_MASK) | target::PHYSICS_CONTENTS_SOLID;
	}

	target::PhysicsAsset* convert(const PhysicsAsset* asset, utils::memory::allocator& allocator)
	{
		static_assert(sizeof(target::PhysicsAssetSimulationCategory) == sizeof(PhysicsAssetSimulationCategory));

		const auto havok_data = havok::convert_physics_data(std::string_view(asset->havokData, asset->havokDataSize), asset->name);
		if (!havok_data)
		{
			return nullptr;
		}

		const auto result = allocator.allocate<target::PhysicsAsset>();
		result->name = asset->name;
		result->havokData = allocator.allocate_array<char>(havok_data->size());
		result->havokDataSize = static_cast<unsigned int>(havok_data->size());
		std::memcpy(result->havokData, havok_data->data(), havok_data->size());
		result->useCategory = static_cast<target::PhysicsAssetUseCategory>(asset->useCategory);
		result->numRigidBodies = asset->numRigidBodies;
		result->numConstraints = asset->numConstraints;
		result->simulationCategories = reinterpret_cast<target::PhysicsAssetSimulationCategory*>(asset->simulationCategories);
		result->bodyContents = allocator.allocate_array<unsigned int>(asset->numRigidBodies);
		for (auto i = 0; asset->bodyContents && i < asset->numRigidBodies; i++)
		{
			result->bodyContents[i] = convert_contents(asset->bodyContents[i]);
		}

		result->numSFXEventAssets = asset->numSFXEventAssets;
		result->numVFXEventAssets = asset->numVFXEventAssets;
		result->sfxEventAssets = allocator.allocate_array<target::PhysicsSFXEventAsset*>(asset->numSFXEventAssets);
		result->vfxEventAssets = allocator.allocate_array<target::PhysicsVFXEventAsset*>(asset->numVFXEventAssets);

		result->containsDynamicBodies = asset->containsDynamicBodies;
		return result;
	}

	void dump(PhysicsAsset* asset)
	{
		utils::memory::allocator allocator;
		if (const auto converted = convert(asset, allocator))
		{
			target::physics_asset::dump(converted);
			add_csv_row(target::type_to_string(target::ASSET_TYPE_PHYSICSASSET), asset->name);
		}
	}

	void dump(XModelDetailCollision* asset)
	{
		static_assert(sizeof(target::XModelDetailCollision) == sizeof(XModelDetailCollision));

		const auto havok_data = havok::convert_physics_data(std::string_view(asset->physicsLODData, asset->physicsLODDataSize), asset->name);
		if (!havok_data)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto converted = allocator.allocate<target::XModelDetailCollision>();
		converted->name = asset->name;
		converted->physicsLODData = allocator.allocate_array<char>(havok_data->size());
		converted->physicsLODDataSize = static_cast<unsigned int>(havok_data->size());
		std::memcpy(converted->physicsLODData, havok_data->data(), havok_data->size());
		converted->physicsLODDataNameCount = asset->physicsLODDataNameCount;
		converted->physicsLODDataNames = asset->physicsLODDataNames;

		target::xmodel_detail_collision::dump(converted);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_XMODEL_DETAIL_COLLISION), asset->name);
	}

	void dump(PhysicsFXShape* asset)
	{
		static std::unordered_set<std::string> dumped;
		if (!dumped.insert(asset->name).second)
		{
			return;
		}

		const auto havok_data = havok::convert_shape_list(std::string_view(asset->havokData, asset->havokDataSize), asset->name);
		if (!havok_data)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto converted = allocator.allocate<target::PhysicsFXShape>();
		converted->name = asset->name;
		converted->havokData = allocator.allocate_array<char>(havok_data->size());
		converted->havokDataSize = static_cast<unsigned int>(havok_data->size());
		std::memcpy(converted->havokData, havok_data->data(), havok_data->size());
		converted->soundAlias = asset->soundAlias;
		converted->soundMinImpulse = asset->soundMinImpulse;
		converted->soundMaxImpulse = asset->soundMaxImpulse;
		converted->soundMinImpulseVolume = asset->soundMinImpulseVolume;
		converted->soundMaxImpulseVolume = asset->soundMaxImpulseVolume;

		target::physics_fx_shape::dump(converted);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_PHYSICS_FX_SHAPE), asset->name);
	}
}
