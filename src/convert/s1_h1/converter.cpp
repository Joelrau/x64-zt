#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::convert::s1_h1
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
			namespace source = zonetool::s1;
			namespace target = zonetool::s1::converter::h1;

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
			case source::ASSET_TYPE_SCRIPTFILE:
				target::scriptfile::dump(header_as<source::ScriptFile>(header));
				break;
			case source::ASSET_TYPE_SOUND:
				target::sound::dump(header_as<source::snd_alias_list_t>(header));
				break;
			case source::ASSET_TYPE_TECHNIQUE_SET:
				target::techset::dump(header_as<source::MaterialTechniqueSet>(header));
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
			case source::ASSET_TYPE_PATHDATA:
				target::aipaths::dump(header_as<source::PathData>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				target::gfxworld::dump(header_as<source::GfxWorld>(header));
				break;
			case source::ASSET_TYPE_CLUT:
				zonetool::h1::clut::dump(header_as<zonetool::h1::Clut>(header));
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
			case source::ASSET_TYPE_LASER:
				zonetool::h1::laser_def::dump(header_as<zonetool::h1::LaserDef>(header));
				break;
			case source::ASSET_TYPE_LOCALIZE_ENTRY:
				zonetool::h1::localize::dump(header_as<zonetool::h1::LocalizeEntry>(header));
				break;
			case source::ASSET_TYPE_LPF_CURVE:
				zonetool::h1::lpf_curve::dump(header_as<zonetool::h1::SndCurve>(header));
				break;
			case source::ASSET_TYPE_NET_CONST_STRINGS:
				zonetool::h1::net_const_strings::dump(header_as<zonetool::h1::NetConstStrings>(header));
				break;
			case source::ASSET_TYPE_RAWFILE:
				zonetool::h1::rawfile::dump(header_as<zonetool::h1::RawFile>(header));
				break;
			case source::ASSET_TYPE_REVERB_CURVE:
				zonetool::h1::reverb_curve::dump(header_as<zonetool::h1::SndCurve>(header));
				break;
			case source::ASSET_TYPE_SCRIPTABLE:
				zonetool::h1::scriptable_def::dump(header_as<zonetool::h1::ScriptableDef>(header));
				break;
			case source::ASSET_TYPE_SKELETON_SCRIPT:
				zonetool::h1::skeleton_script::dump(header_as<zonetool::h1::SkeletonScript>(header));
				break;
			case source::ASSET_TYPE_SOUND_CONTEXT:
				zonetool::h1::sound_context::dump(header_as<zonetool::h1::SndContext>(header));
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
			case source::ASSET_TYPE_TRACER:
				zonetool::h1::tracer_def::dump(header_as<zonetool::h1::TracerDef>(header));
				break;
			case source::ASSET_TYPE_XANIMPARTS:
				zonetool::h1::xanim_parts::dump(header_as<zonetool::h1::XAnimParts>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				zonetool::h1::xmodel::dump(header_as<zonetool::h1::XModel>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				zonetool::h1::xsurface::dump(header_as<zonetool::h1::XModelSurfs>(header));
				break;
			case source::ASSET_TYPE_REVERB_PRESET:
				zonetool::h1::reverb_preset::dump(header_as<zonetool::h1::ReverbPreset>(header));
				break;
			case source::ASSET_TYPE_SOUND_SUBMIX:
				zonetool::h1::sound_submix::dump(header_as<zonetool::h1::SndSubmixList>(header));
				break;
			case source::ASSET_TYPE_SURFACE_FX:
				zonetool::h1::surface_fx::dump(header_as<zonetool::h1::SurfaceFxTable>(header));
				break;
			case source::ASSET_TYPE_VEHICLE:
				zonetool::h1::vehicle_def::dump(header_as<zonetool::h1::VehicleDef>(header));
				break;
			case source::ASSET_TYPE_LEADERBOARD:
				zonetool::h1::leaderboard::dump(header_as<zonetool::h1::LeaderboardDef>(header));
				break;
			case source::ASSET_TYPE_EQUIPMENT_SND_TABLE:
				zonetool::h1::equip_snd_table::dump(header_as<zonetool::h1::EquipmentSoundTable>(header));
				break;
			case source::ASSET_TYPE_VECTORFIELD:
				zonetool::h1::vector_field::dump(header_as<zonetool::h1::VectorField>(header));
				break;
			case source::ASSET_TYPE_ANIMCLASS:
				zonetool::h1::anim_class::dump(header_as<zonetool::h1::AnimationClass>(header));
				break;
			case source::ASSET_TYPE_PHYSCOLLMAP:
				zonetool::h1::phys_collmap::dump(header_as<zonetool::h1::PhysCollmap>(header));
				break;
			case source::ASSET_TYPE_PHYSCONSTRAINT:
				zonetool::h1::phys_constraint::dump(header_as<zonetool::h1::PhysConstraint>(header));
				break;
			case source::ASSET_TYPE_PHYSPRESET:
				zonetool::h1::phys_preset::dump(header_as<zonetool::h1::PhysPreset>(header));
				break;
			case source::ASSET_TYPE_PHYSWATERPRESET:
				zonetool::h1::phys_water_preset::dump(header_as<zonetool::h1::PhysWaterPreset>(header));
				break;
			case source::ASSET_TYPE_PHYSWORLDMAP:
				zonetool::h1::phys_world::dump(header_as<zonetool::h1::PhysWorld>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				zonetool::h1::clip_map::dump(header_as<zonetool::h1::clipMap_t>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				zonetool::h1::com_world::dump(header_as<zonetool::h1::ComWorld>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				zonetool::h1::fx_world::dump(header_as<zonetool::h1::FxWorld>(header));
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
	}

	const converter& definition()
	{
		static const converter s1_h1
		{
			.source = "s1",
			.target = "h1",
			.dump_asset = dump_asset,
		};

		return s1_h1;
	}
}
