#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::convert::h2_h1
{
	namespace
	{
		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::h2;
			namespace target = zonetool::h2::converter::h1;

			switch (type)
			{
			case source::ASSET_TYPE_FX:
				target::fxeffectdef::dump(header_as<source::FxEffectDef>(header));
				break;
			case source::ASSET_TYPE_IMAGE:
				target::gfximage::dump(header_as<source::GfxImage>(header));
				break;
			case source::ASSET_TYPE_LASER:
				target::laserdef::dump(header_as<source::LaserDef>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				target::material::dump(header_as<source::Material>(header));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				target::mapents::dump(header_as<source::MapEnts>(header));
				break;
			case source::ASSET_TYPE_SCRIPTFILE:
				target::scriptfile::dump(header_as<source::ScriptFile>(header));
				break;
			case source::ASSET_TYPE_SOUND:
				target::sound::dump(header_as<source::snd_alias_list_t>(header));
				break;
			case source::ASSET_TYPE_TECHNIQUE_SET:
				target::techset::dump(header_as<source::MaterialTechniqueSet>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_COMPUTESHADER:
				target::techset::dump(header_as<source::ComputeShader>(header));
				break;
			case source::ASSET_TYPE_DOMAINSHADER:
				target::techset::dump(header_as<source::MaterialDomainShader>(header));
				break;
			case source::ASSET_TYPE_HULLSHADER:
				target::techset::dump(header_as<source::MaterialHullShader>(header));
				break;
			case source::ASSET_TYPE_PIXELSHADER:
				target::techset::dump(header_as<source::MaterialPixelShader>(header));
				break;
			case source::ASSET_TYPE_VERTEXSHADER:
				target::techset::dump(header_as<source::MaterialVertexShader>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				target::comworld::dump(header_as<source::ComWorld>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				target::fxworld::dump(header_as<source::FxWorld>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				target::gfxworld::dump(header_as<source::GfxWorld>(header));
				break;
			case source::ASSET_TYPE_CLUT:
				source::clut::dump(header_as<source::Clut>(header));
				break;
			case source::ASSET_TYPE_DOPPLER_PRESET:
				source::doppler_preset::dump(header_as<source::DopplerPreset>(header));
				break;
			case source::ASSET_TYPE_PARTICLE_SIM_ANIMATION:
				source::fx_particle_sim_animation::dump(header_as<source::FxParticleSimAnimation>(header));
				break;
			case source::ASSET_TYPE_LIGHT_DEF:
				source::gfx_light_def::dump(header_as<source::GfxLightDef>(header));
				break;
			case source::ASSET_TYPE_LOADED_SOUND:
				source::loaded_sound::dump(header_as<source::LoadedSound>(header));
				break;
			case source::ASSET_TYPE_LOCALIZE_ENTRY:
				source::localize::dump(header_as<source::LocalizeEntry>(header));
				break;
			case source::ASSET_TYPE_LPF_CURVE:
				source::lpf_curve::dump(header_as<source::SndCurve>(header));
				break;
			case source::ASSET_TYPE_LUA_FILE:
				source::lua_file::dump(header_as<source::LuaFile>(header));
				break;
			case source::ASSET_TYPE_NET_CONST_STRINGS:
				source::net_const_strings::dump(header_as<source::NetConstStrings>(header));
				break;
			case source::ASSET_TYPE_RAWFILE:
				source::rawfile::dump(header_as<source::RawFile>(header));
				break;
			case source::ASSET_TYPE_REVERB_CURVE:
				source::reverb_curve::dump(header_as<source::SndCurve>(header));
				break;
			case source::ASSET_TYPE_SCRIPTABLE:
				source::scriptable_def::dump(header_as<source::ScriptableDef>(header));
				break;
			case source::ASSET_TYPE_SKELETON_SCRIPT:
				source::skeleton_script::dump(header_as<source::SkeletonScript>(header));
				break;
			case source::ASSET_TYPE_SOUND_CONTEXT:
				source::sound_context::dump(header_as<source::SndContext>(header));
				break;
			case source::ASSET_TYPE_SOUND_CURVE:
				source::sound_curve::dump(header_as<source::SndCurve>(header));
				break;
			case source::ASSET_TYPE_STRINGTABLE:
				source::string_table::dump(header_as<source::StringTable>(header));
				break;
			case source::ASSET_TYPE_STRUCTURED_DATA_DEF:
				source::structured_data_def_set::dump(header_as<source::StructuredDataDefSet>(header));
				break;
			case source::ASSET_TYPE_TRACER:
				source::tracer_def::dump(header_as<source::TracerDef>(header));
				break;
			case source::ASSET_TYPE_TTF:
				source::ttf_def::dump(header_as<source::TTFDef>(header));
				break;
			case source::ASSET_TYPE_ATTACHMENT:
				source::weapon_attachment::dump(header_as<source::WeaponAttachment>(header));
				break;
			case source::ASSET_TYPE_WEAPON:
				source::weapon_def::dump(header_as<source::WeaponDef>(header));
				break;
			case source::ASSET_TYPE_VEHICLE:
				source::vehicle_def::dump(header_as<source::VehicleDef>(header));
				break;
			case source::ASSET_TYPE_SOUNDSUBMIX:
				source::sound_submix::dump(header_as<source::SndSubmixList>(header));
				break;
			case source::ASSET_TYPE_REVERB_PRESET:
				source::reverb_preset::dump(header_as<source::ReverbPreset>(header));
				break;
			case source::ASSET_TYPE_XANIM:
				source::xanim_parts::dump(header_as<source::XAnimParts>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				source::xsurface::dump(header_as<source::XModelSurfs>(header));
				break;
			case source::ASSET_TYPE_PHYSCOLLMAP:
				source::phys_collmap::dump(header_as<source::PhysCollmap>(header));
				break;
			case source::ASSET_TYPE_PHYSCONSTRAINT:
				source::phys_constraint::dump(header_as<source::PhysConstraint>(header));
				break;
			case source::ASSET_TYPE_PHYSPRESET:
				source::phys_preset::dump(header_as<source::PhysPreset>(header));
				break;
			case source::ASSET_TYPE_PHYSWATERPRESET:
				source::phys_water_preset::dump(header_as<source::PhysWaterPreset>(header));
				break;
			case source::ASSET_TYPE_PHYSWORLDMAP:
				source::phys_world::dump(header_as<source::PhysWorld>(header));
				break;
			case source::ASSET_TYPE_MENU:
				source::menu_def::dump(header_as<source::menuDef_t>(header));
				break;
			case source::ASSET_TYPE_MENULIST:
				source::menu_list::dump(header_as<source::MenuList>(header));
				break;
			case source::ASSET_TYPE_PATHDATA:
				source::path_data::dump(header_as<source::PathData>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				source::clip_map::dump(header_as<source::clipMap_t>(header));
				break;
			case source::ASSET_TYPE_GLASSWORLD:
				source::glass_world::dump(header_as<source::GlassWorld>(header));
				break;
			default:
				break;
			}
		}
	}

	const converter& definition()
	{
		static const converter h2_h1
		{
			.source = "h2",
			.target = "h1",
			.dump_asset = dump_asset,
		};

		return h2_h1;
	}
}
