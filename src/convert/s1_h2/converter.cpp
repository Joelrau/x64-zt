#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::convert::s1_h2
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
			namespace to_h1 = zonetool::s1::converter::h1;
			namespace to_h2 = zonetool::h1::converter::h2;

			utils::memory::allocator allocator;

			switch (type)
			{
			case source::ASSET_TYPE_FX:
				to_h2::fxeffectdef::dump(to_h1::fxeffectdef::convert(header_as<source::FxEffectDef>(header), allocator));
				break;
			case source::ASSET_TYPE_MATERIAL:
				to_h2::material::dump(to_h1::material::convert(header_as<source::Material>(header), allocator));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				to_h2::mapents::dump(to_h1::mapents::convert(header_as<source::MapEnts>(header), allocator));
				break;
			case source::ASSET_TYPE_SOUND:
				to_h2::sound::dump(to_h1::sound::convert(header_as<source::snd_alias_list_t>(header), allocator));
				break;
			case source::ASSET_TYPE_TECHNIQUE_SET:
				to_h2::techset::dump(to_h1::techset::convert(header_as<source::MaterialTechniqueSet>(header), allocator));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				to_h2::gfxworld::dump(to_h1::gfxworld::convert(header_as<source::GfxWorld>(header), allocator));
				break;
			case source::ASSET_TYPE_IMAGE:
				to_h2::gfximage::dump(header_as<zonetool::h1::GfxImage>(header));
				break;
			case source::ASSET_TYPE_LASER:
				to_h2::laserdef::dump(header_as<zonetool::h1::LaserDef>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				to_h2::xmodel::dump(header_as<zonetool::h1::XModel>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				to_h2::comworld::dump(header_as<zonetool::h1::ComWorld>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				to_h2::fxworld::dump(header_as<zonetool::h1::FxWorld>(header));
				break;
			case source::ASSET_TYPE_SCRIPTFILE:
				to_h1::scriptfile::dump(header_as<source::ScriptFile>(header));
				break;
			case source::ASSET_TYPE_COMPUTESHADER:
				to_h1::techset::dump(header_as<source::ComputeShader>(header));
				break;
			case source::ASSET_TYPE_DOMAINSHADER:
				to_h1::techset::dump(header_as<source::MaterialDomainShader>(header));
				break;
			case source::ASSET_TYPE_HULLSHADER:
				to_h1::techset::dump(header_as<source::MaterialHullShader>(header));
				break;
			case source::ASSET_TYPE_PIXELSHADER:
				to_h1::techset::dump(header_as<source::MaterialPixelShader>(header));
				break;
			case source::ASSET_TYPE_VERTEXSHADER:
				to_h1::techset::dump(header_as<source::MaterialVertexShader>(header));
				break;
			case source::ASSET_TYPE_PATHDATA:
				to_h1::aipaths::dump(header_as<source::PathData>(header));
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
			case source::ASSET_TYPE_LOCALIZE_ENTRY:
				zonetool::h1::localize::dump(header_as<zonetool::h1::LocalizeEntry>(header));
				break;
			case source::ASSET_TYPE_LPF_CURVE:
				zonetool::h1::lpf_curve::dump(header_as<zonetool::h1::SndCurve>(header));
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
			case source::ASSET_TYPE_XMODEL_SURFS:
				zonetool::h1::xsurface::dump(header_as<zonetool::h1::XModelSurfs>(header));
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
			case source::ASSET_TYPE_GLASSWORLD:
				zonetool::h1::glass_world::dump(header_as<zonetool::h1::GlassWorld>(header));
				break;
			case source::ASSET_TYPE_LOADED_SOUND:
				source::loaded_sound::dump(header_as<source::LoadedSound>(header));
				break;
			case source::ASSET_TYPE_ATTACHMENT:
				source::weapon_attachment::dump(header_as<source::WeaponAttachment>(header));
				break;
			case source::ASSET_TYPE_WEAPON:
				source::weapon_def::dump(header_as<source::WeaponDef>(header));
				break;
			default:
				break;
			}
		}
	}

	const converter& definition()
	{
		static const converter s1_h2
		{
			.source = "s1",
			.target = "h2",
			.dump_asset = dump_asset,
		};

		return s1_h2;
	}
}
