#pragma once

namespace zonetool::s2
{
	struct fx_world_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(FxWorld, name));
			visit_glass_system(w.sub(offsetof(FxWorld, glassSys)));
		}

	private:
		template <typename V>
		static void visit_glass_system(const V& glass)
		{
			glass.entries(offsetof(FxGlassSystem, defs), sizeof(FxGlassDef),
				glass.template at<unsigned int>(offsetof(FxGlassSystem, defCount)), [](const auto& def, std::size_t)
			{
				def.asset(ASSET_TYPE_PHYSPRESET, offsetof(FxGlassDef, physPreset));
				def.asset(ASSET_TYPE_MATERIAL, offsetof(FxGlassDef, material));
				def.asset(ASSET_TYPE_MATERIAL, offsetof(FxGlassDef, materialShattered));
				def.asset(ASSET_TYPE_FX, offsetof(FxGlassDef, pieceBreakEffect));
				def.asset(ASSET_TYPE_FX, offsetof(FxGlassDef, shatterEffect));
				def.asset(ASSET_TYPE_FX, offsetof(FxGlassDef, shatterSmallEffect));
				def.asset(ASSET_TYPE_FX, offsetof(FxGlassDef, crackDecalEffect));
				def.sound(offsetof(FxGlassDef, damagedSound));
				def.sound(offsetof(FxGlassDef, destroyedSound));
				def.sound(offsetof(FxGlassDef, destroyedQuietSound));
			});

			const auto count = [&](const std::size_t offset)
			{
				return glass.template at<unsigned int>(offset);
			};
			const auto piece_limit = count(offsetof(FxGlassSystem, pieceLimit));
			const auto piece_word_count = count(offsetof(FxGlassSystem, pieceWordCount));
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, piecePlaces), 3, piece_limit);
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, pieceStates), 3, piece_limit);
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, pieceDynamics), 7, piece_limit);
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, geoData), 1, count(offsetof(FxGlassSystem, geoDataLimit)));
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, isInUse), 3, piece_word_count);
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, cellBits), 3, piece_word_count * count(offsetof(FxGlassSystem, cellCount)));
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, visData), 15, (piece_limit + 15) & ~15u);
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, linkOrg), 3, piece_limit);
			glass.runtime_array(S2_ARRAY_FIELD(FxGlassSystem, halfThickness), 15, (piece_limit + 3) & ~3u);

			const auto init_piece_count = count(offsetof(FxGlassSystem, initPieceCount));
			glass.array(S2_ARRAY_FIELD(FxGlassSystem, lightingHandles), 1, init_piece_count);
			glass.array(S2_ARRAY_FIELD(FxGlassSystem, initGeoData), 1, count(offsetof(FxGlassSystem, initGeoDataCount)));
			glass.array(S2_ARRAY_FIELD(FxGlassSystem, initPieceStates), 3, init_piece_count);
		}
	};

	class fx_world : public field_layout_asset<FxWorld, ASSET_TYPE_FXWORLD, fx_world_layout>
	{
	};
}
