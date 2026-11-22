#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			// v102 fx: IW7 legacy FxEffectDef (ASSET_TYPE_FX) -> H1 FxEffectDef
			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator);

			// v109 fx: converts the effect now (zone memory is only valid during the load) into owned H1 data;
			//          the json is written by flush() once the whole zone was seen
			void dump(FxEffectDef* asset);
			void dump_vfx(ParticleSystemDef* asset);

			// v102 fx: drops elements whose material/model/effect was not dumped, writes effects/*.json
			void flush();

			// v102 fx: book-keeping of what was dumped for H1 in this session (materials, models, effects)
			void note_dumped(XAssetType type, const char* name);
			bool was_dumped(XAssetType type, const char* name);

			// v102 fx: effect names that ended up as H1 effects json (valid loadfx targets)
			const std::unordered_set<std::string>& converted_effects();

			// v104 vfx: shared with the particle system converter (material goes to the additive emissive techset)
			bool is_fx_emissive_material(const Material* mat);

			// v114 fx: an effect material from another loaded zone (IW7 common_core_mp): dump it (json + images) with the map
			void ensure_external_material(const Material* mat);
		}
	}
}
