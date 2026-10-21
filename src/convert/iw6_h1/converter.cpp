#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::iw6::converter::h1
{
	namespace
	{
		std::vector<std::string> csv_rows;
	}

	void add_csv_row(const std::string_view type, const std::string_view name)
	{
		auto row = std::format("{},{}", type, name);
		if (std::ranges::find(csv_rows, row) == csv_rows.end())
		{
			csv_rows.emplace_back(std::move(row));
		}
	}

	void append_csv_rows(const std::string& zone_name)
	{
		if (csv_rows.empty())
		{
			return;
		}

		filesystem::file csv(zone_name + ".csv");
		csv.open("ab");

		for (const auto& row : csv_rows)
		{
			csv.write(row + "\n");
		}

		csv.close();
		csv_rows.clear();
	}
}

namespace zonetool::convert::iw6_h1
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
			namespace source = zonetool::iw6;
			namespace target = zonetool::iw6::converter::h1;

			switch (type)
			{
			case source::ASSET_TYPE_FX:
				target::fxeffectdef::dump(header_as<source::FxEffectDef>(header));
				break;
			case source::ASSET_TYPE_IMAGE:
				target::gfximage::dump(header_as<source::GfxImage>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				target::material::dump(header_as<source::Material>(header));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				target::mapents::dump(header_as<source::MapEnts>(header));
				break;
			case source::ASSET_TYPE_NET_CONST_STRINGS:
				target::net_const_strings::dump(header_as<source::NetConstStrings>(header));
				break;
			case source::ASSET_TYPE_RAWFILE:
				target::rawfile::dump(header_as<source::RawFile>(header));
				break;
			case source::ASSET_TYPE_SCRIPTABLE:
				target::scriptabledef::dump(header_as<source::ScriptableDef>(header));
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
			case source::ASSET_TYPE_TRACER:
				target::tracer_def::dump(header_as<source::TracerDef>(header));
				break;
			case source::ASSET_TYPE_XANIMPARTS:
				target::xanim::dump(header_as<source::XAnimParts>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				target::xsurface::dump(header_as<source::XModelSurfs>(header));
				break;
			case source::ASSET_TYPE_PHYSCOLLMAP:
				target::physcollmap::dump(header_as<source::PhysCollmap>(header));
				break;
			case source::ASSET_TYPE_PHYSPRESET:
				target::physpreset::dump(header_as<source::PhysPreset>(header));
				break;
			case source::ASSET_TYPE_COMPUTESHADER:
				target::computeshader::dump(header_as<source::ComputeShader>(header));
				break;
			case source::ASSET_TYPE_DOMAINSHADER:
				target::domainshader::dump(header_as<source::MaterialDomainShader>(header));
				break;
			case source::ASSET_TYPE_HULLSHADER:
				target::hullshader::dump(header_as<source::MaterialHullShader>(header));
				break;
			case source::ASSET_TYPE_PIXELSHADER:
				target::pixelshader::dump(header_as<source::MaterialPixelShader>(header));
				break;
			case source::ASSET_TYPE_VERTEXSHADER:
				target::vertexshader::dump(header_as<source::MaterialVertexShader>(header));
				break;
			case source::ASSET_TYPE_VEHICLE:
				target::vehicledef::dump(header_as<source::VehicleDef>(header));
				break;
			case source::ASSET_TYPE_WEAPON:
				target::weapondef::dump(header_as<source::WeaponCompleteDef>(header));
				break;
			case source::ASSET_TYPE_ATTACHMENT:
				target::weaponattachment::dump(header_as<source::WeaponAttachment>(header));
				break;
			case source::ASSET_TYPE_TONEMAPPING:
				target::tonemapping::dump(header_as<source::ToneMapping>(header));
				break;
			case source::ASSET_TYPE_PATHDATA:
				target::aipaths::dump(header_as<source::PathData>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				target::clipmap::dump(header_as<source::clipMap_t>(header));
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
			case source::ASSET_TYPE_DOPPLER_PRESET:
				zonetool::h1::doppler_preset::dump(header_as<zonetool::h1::DopplerPreset>(header));
				break;
			case source::ASSET_TYPE_PARTICLE_SIM_ANIMATION:
				zonetool::h1::fx_particle_sim_animation::dump(header_as<zonetool::h1::FxParticleSimAnimation>(header));
				break;
			case source::ASSET_TYPE_LIGHT_DEF:
				zonetool::h1::gfx_light_def::dump(header_as<zonetool::h1::GfxLightDef>(header));
				break;
			case source::ASSET_TYPE_LOCALIZE_ENTRY:
				zonetool::h1::localize::dump(header_as<zonetool::h1::LocalizeEntry>(header));
				break;
			case source::ASSET_TYPE_LPF_CURVE:
				zonetool::h1::lpf_curve::dump(header_as<zonetool::h1::SndCurve>(header));
				break;
			case source::ASSET_TYPE_REVERB_CURVE:
				zonetool::h1::reverb_curve::dump(header_as<zonetool::h1::SndCurve>(header));
				break;
			case source::ASSET_TYPE_SOUND_CURVE:
				zonetool::h1::sound_curve::dump(header_as<zonetool::h1::SndCurve>(header));
				break;
			case source::ASSET_TYPE_STRINGTABLE:
				zonetool::h1::string_table::dump(header_as<zonetool::h1::StringTable>(header));
				break;
			case source::ASSET_TYPE_STRUCTURED_DATA_DEF:
				zonetool::h1::structured_data_def_set::dump(header_as<zonetool::h1::StructuredDataDefSet>(header));
				break;
			case source::ASSET_TYPE_ANIMCLASS:
				zonetool::h1::anim_class::dump(header_as<zonetool::h1::AnimationClass>(header));
				break;
			case source::ASSET_TYPE_MENU:
				zonetool::h1::menu_def::dump(header_as<zonetool::h1::menuDef_t>(header));
				break;
			case source::ASSET_TYPE_MENULIST:
				zonetool::h1::menu_list::dump(header_as<zonetool::h1::MenuList>(header));
				break;
			case source::ASSET_TYPE_GLASSWORLD:
				zonetool::h1::glass_world::dump(header_as<zonetool::h1::GlassWorld>(header));
				break;
			case source::ASSET_TYPE_LOADED_SOUND:
				source::loaded_sound::dump(header_as<source::LoadedSound>(header));
				break;
			default:
				break;
			}
		}

		void finish(const std::string& zone_name)
		{
			zonetool::iw6::converter::h1::net_const_strings::add_missing_level_strings();
			zonetool::iw6::converter::h1::append_csv_rows(zone_name);
		}

		std::vector<std::string> required_zones(const std::string&)
		{
			return {"code_post_gfx_mp", "techsets_code_post_gfx_mp", "techsets_common_core_mp"};
		}
	}

	const converter& definition()
	{
		static const converter iw6_h1
		{
			.source = "iw6",
			.target = "h1",
			.dump_asset = dump_asset,
			.finish = finish,
			.required_zones = required_zones,
		};

		return iw6_h1;
	}
}
