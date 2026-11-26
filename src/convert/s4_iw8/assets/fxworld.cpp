#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"
#include "material.hpp"

#include <iw8/assets/fxworld.hpp>

namespace zonetool::s4::converter::iw8::fxworld
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename T>
		T* make_array(const std::size_t count, utils::memory::allocator& allocator)
		{
			return count ? allocator.allocate_array<T>(count) : nullptr;
		}

		void convert_glass_system(const FxGlassSystem& source, target::FxGlassSystem& system, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(target::FxGlassGeometryData) == sizeof(FxGlassGeometryData));
			static_assert(sizeof(target::FxGlassInitPieceState) == sizeof(FxGlassInitPieceState));

			system.time = source.time;
			system.prevTime = source.prevTime;
			system.pieceLimit = source.pieceLimit;
			system.pieceWordCount = source.pieceWordCount;
			system.cellCount = source.pieceLimit ? 2 : 0; // gfxworld cells + 1
			system.activePieceCount = source.activePieceCount;
			system.firstFreePiece = source.firstFreePiece;
			system.geoDataLimit = source.geoDataLimit;
			system.geoDataCount = source.geoDataCount;
			system.initGeoDataCount = source.initGeoDataCount;

			system.piecePlaces = make_array<target::FxGlassPiecePlace>(system.pieceLimit, allocator);
			system.pieceStates = make_array<target::FxGlassPieceState>(system.pieceLimit, allocator);
			system.pieceDynamics = make_array<target::FxGlassPieceDynamics>(system.pieceLimit, allocator);
			system.geoData = make_array<target::FxGlassGeometryData>(system.geoDataLimit, allocator);
			system.isInUse = make_array<unsigned int>(system.pieceWordCount, allocator);
			system.cellBits = make_array<unsigned int>(system.pieceWordCount * system.cellCount, allocator);
			system.visData = make_array<unsigned char>((system.pieceLimit + 15) & ~15u, allocator);
			system.linkOrg = make_array<vec3_t>(system.pieceLimit, allocator);
			system.halfThickness = make_array<float>((system.pieceLimit + 3) & ~3u, allocator);

			system.initGeoData = reinterpret_cast<target::FxGlassGeometryData*>(source.initGeoData);
			system.needToCompactData = source.needToCompactData;
			system.initCount = source.initCount;
			system.effectChanceAccum = source.effectChanceAccum;
			system.lastPieceDeletionTime = source.lastPieceDeletionTime;
			system.initPieceCount = source.initPieceCount;
			system.initPieceStates = reinterpret_cast<target::FxGlassInitPieceState*>(source.initPieceStates);
		}

		void convert_glass_globals(const FxWorld* asset, target::FxGlassGlobals& globals, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(target::FxGlassTransientLookup) == sizeof(FxGlassTransientLookup));

			const auto& source = asset->glassGlob;
			globals.defCount = source.defCount;
			globals.defs = make_array<target::FxGlassDef>(source.defCount, allocator);
			globals.distances = make_array<float>(source.defCount, allocator);
			globals.glassTransientLookupCount = source.glassTransientLookupCount;
			globals.glassTransientLookup = reinterpret_cast<target::FxGlassTransientLookup*>(source.glassTransientLookup);

			auto lost_effects = 0u;
			for (auto i = 0u; source.defs && i < source.defCount; i++)
			{
				const auto& def = source.defs[i];
				auto& result = globals.defs[i];

				result.halfThickness = def.halfThickness;
				std::memcpy(result.texVecs, def.texVecs, sizeof(result.texVecs));
				result.color.packed = def.color.packed;
				result.material = material::reference(def.material, allocator);
				result.materialShattered = material::reference(def.materialShattered, allocator);
				if (def.physicsAsset)
				{
					const auto physics_asset = allocator.allocate<target::PhysicsAsset>();
					physics_asset->name = def.physicsAsset->name;
					result.physicsAsset = physics_asset;
				}

				lost_effects += (def.pieceBreakEffect.particleSystemDef != nullptr) + (def.shatterEffect.particleSystemDef != nullptr)
					+ (def.shatterSmallEffect.particleSystemDef != nullptr) + (def.crackDecalEffect.particleSystemDef != nullptr);

				result.highMipRadiusInvSq = def.highMipRadiusInvSq;
				result.shatteredHighMipRadiusInvSq = def.shatteredHighMipRadiusInvSq;
				result.numCrackRings = def.numCrackRings;
				result.transientIndex = def.transientIndex;
			}

			if (source.defCount)
			{
				ZONETOOL_WARNING("fxworld \"%s\": %u glass defs lose %u effects and their sounds, S4 has only the alias hash", asset->name, source.defCount, lost_effects);
			}
		}
	}

	void dump(FxWorld* asset)
	{
		utils::memory::allocator allocator;
		const auto world = allocator.allocate<target::FxWorld>();
		world->name = bsp_name(asset->name, allocator);

		convert_glass_globals(asset, world->glassGlob, allocator);
		convert_glass_system(asset->glassSys, world->glassSys, allocator);

		world->transientZoneCount = asset->transientZoneCount;
		for (auto i = 0u; i < asset->transientZoneCount && i < std::size(world->transientZones); i++)
		{
			if (asset->transientZones[i])
			{
				world->transientZones[i] = allocator.allocate<target::FxWorldTransientZone>();
				world->transientZones[i]->name = asset->transientZones[i]->name;
			}
		}

		std::memcpy(world->activeTransientZones, asset->activeTransientZones, sizeof(world->activeTransientZones));
		world->activeTransientZoneCount = asset->activeTransientZoneCount;

		target::fx_world::dump(world);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_FXWORLD), world->name);
	}

	void dump(FxWorldTransientZone* asset)
	{
		utils::memory::allocator allocator;
		const auto zone = allocator.allocate<target::FxWorldTransientZone>();
		zone->name = asset->name;
		zone->transientZoneIndex = asset->transientZoneIndex;
		convert_glass_system(asset->glassSys, zone->glassSys, allocator);

		target::fx_world_transient_zone::dump(zone);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_FXWORLD_TRANSIENT_ZONE), zone->name);
	}
}
