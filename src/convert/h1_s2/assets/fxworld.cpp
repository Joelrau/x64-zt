#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"

namespace zonetool::h1::converter::s2::fxworld
{
	namespace
	{
		// every stock S2 glass def uses it, H1 uses glass_chunk
		constexpr auto glass_phys_preset = "phys_glass";

		zonetool::s2::FxGlassDef* convert_defs(const FxGlassSystem& glass, utils::memory::allocator& allocator)
		{
			if (!glass.defCount)
			{
				return nullptr;
			}

			const auto phys_preset = make_stub<zonetool::s2::PhysPreset>(glass_phys_preset, allocator);
			const auto defs = allocator.allocate_array<zonetool::s2::FxGlassDef>(glass.defCount);
			for (auto i = 0u; i < glass.defCount; i++)
			{
				const auto& def = glass.defs[i];
				auto& result = defs[i];
				result.halfThickness = def.halfThickness;
				std::memcpy(result.texVecs, def.texVecs, sizeof(result.texVecs));
				for (auto c = 0; c < 4; c++)
				{
					result.color[c] = def.color.array[c] / 255.0f;
				}

				result.material = reinterpret_cast<zonetool::s2::Material*>(def.material);
				result.materialShattered = reinterpret_cast<zonetool::s2::Material*>(def.materialShattered);
				result.physPreset = def.physPreset ? phys_preset : nullptr;
				result.pieceBreakEffect = reinterpret_cast<zonetool::s2::FxEffectDef*>(def.pieceBreakEffect);
				result.shatterEffect = reinterpret_cast<zonetool::s2::FxEffectDef*>(def.shatterEffect);
				result.shatterSmallEffect = reinterpret_cast<zonetool::s2::FxEffectDef*>(def.shatterSmallEffect);
				result.crackDecalEffect = reinterpret_cast<zonetool::s2::FxEffectDef*>(def.crackDecalEffect);
				result.damagedSound = reinterpret_cast<zonetool::s2::snd_alias_list_t*>(def.damagedSound);
				result.destroyedSound = reinterpret_cast<zonetool::s2::snd_alias_list_t*>(def.destroyedSound);
				result.destroyedQuietSound = reinterpret_cast<zonetool::s2::snd_alias_list_t*>(def.destroyedQuietSound);
				result.invHighMipRadius = def.invHighMipRadius;
				result.shatteredInvHighMipRadius = def.shatteredInvHighMipRadius;
				result.numCrackRings = def.numCrackRings;
				result.isOpaque = def.isOpaque;

				if (def.physPreset)
				{
					add_csv_row("physpreset", glass_phys_preset, true);
				}
			}

			return defs;
		}

		zonetool::s2::FxWorld* convert(FxWorld* asset, utils::memory::allocator& allocator)
		{
			const auto& glass = asset->glassSys;
			const auto world = allocator.allocate<zonetool::s2::FxWorld>();
			world->name = asset->name;

			auto& result = world->glassSys;
			result.defCount = glass.defCount;
			result.pieceLimit = glass.pieceLimit;
			result.pieceWordCount = glass.pieceWordCount;
			result.cellCount = glass.cellCount;
			result.firstFreePiece = 0xFFFF;
			result.geoDataLimit = glass.geoDataLimit;
			result.initGeoDataCount = glass.initGeoDataCount;
			result.initPieceCount = glass.initPieceCount;
			result.defs = convert_defs(glass, allocator);

			reserve_runtime(result.piecePlaces, glass.pieceLimit, allocator);
			reserve_runtime(result.pieceStates, glass.pieceLimit, allocator);
			reserve_runtime(result.pieceDynamics, glass.pieceLimit, allocator);
			reserve_runtime(result.geoData, glass.geoDataLimit, allocator);
			reserve_runtime(result.isInUse, glass.pieceWordCount, allocator);
			reserve_runtime(result.cellBits, glass.pieceWordCount * glass.cellCount, allocator);
			reserve_runtime(result.visData, glass.pieceLimit, allocator);
			reserve_runtime(result.linkOrg, glass.pieceLimit, allocator);
			reserve_runtime(result.halfThickness, glass.pieceLimit, allocator);

			result.lightingHandles = glass.lightingHandles;
			result.initGeoData = retype<zonetool::s2::FxGlassGeometryData>(glass.initGeoData);
			result.initPieceStates = retype<zonetool::s2::FxGlassInitPieceState>(glass.initPieceStates);
			return world;
		}
	}

	void dump(FxWorld* asset)
	{
		utils::memory::allocator allocator;
		zonetool::s2::fx_world::dump(convert(asset, allocator));
		add_csv_row("fx_map", asset->name);
	}
}
