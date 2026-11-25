#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>
#include <utils/string.hpp>
#include <formats/iwd.hpp>

#include "assets/xanim_parts.hpp"
#include "assets/xmodel.hpp"
#include "assets/gfx_image.hpp"
#include "assets/comworld.hpp"
#include "assets/particle_system.hpp"
#include "assets/material.hpp"
#include "assets/clipmap.hpp"
#include "assets/gfxworld.hpp"
#include "assets/glass_world.hpp"
#include "assets/level.hpp"

namespace zonetool::convert::iw5_iw7
{
	namespace
	{
		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		std::unordered_map<std::string, std::string> level_script_sources;
		std::string level_script_world;

		std::string level_script_source(const std::string& map)
		{
			namespace stage = zonetool::iw5::converter::iw7;

			const auto fx = stage::level::port_fx(level_script_sources[std::format("maps/mp/{}_fx.gsc", map)],
				level_script_sources[std::format("maps/createfx/{}_fx.gsc", map)]);

			return std::format(R"(main()
{{{}
    scripts\mp\load::main();
    scripts\mp\compass::setupminimap( "compass_map_{}" );
    game["attackers"] = "allies";
    game["defenders"] = "axis";
    game["allies_outfit"] = "urban";
    game["axis_outfit"] = "woodland";
}}
)", fx, map);
		}

		void dump_level(const char* world_name)
		{
			namespace stage = zonetool::iw5::converter::iw7;

			const auto map = utils::string::to_lower(std::filesystem::path(world_name).stem().string());
			level_script_world = world_name;
			stage::level::dump_net_const_strings();
			stage::level::dump_empty_fx_world(world_name);
			stage::level::dump_script(std::format("scripts/mp/maps/{}/{}", map, map), level_script_source(map)); // TODO: port the IW5 level threads
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::iw5;
			namespace stage = zonetool::iw5::converter::iw7;

			stage::gfx_image::read_file = iwd::read_file;

			switch (type)
			{
			case source::ASSET_TYPE_XANIMPARTS:
				stage::xanim_parts::dump(header_as<source::XAnimParts>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				stage::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_IMAGE:
				stage::gfx_image::dump(header_as<source::GfxImage>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				stage::comworld::dump(header_as<source::ComWorld>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				stage::clipmap::dump(header_as<source::clipMap_t>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
			{
				const auto world = header_as<source::GfxWorld>(header);
				stage::gfxworld::dump(world);
				dump_level(world->name.get());
				break;
			}
			case source::ASSET_TYPE_GLASSWORLD:
				stage::glass_world::dump(header_as<source::GlassWorld>(header));
				break;
			case source::ASSET_TYPE_FX:
				stage::particle_system::dump(header_as<source::FxEffectDef>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				stage::material::dump(header_as<source::Material>(header));
				break;
			case source::ASSET_TYPE_RAWFILE:
			{
				const auto rawfile = header_as<source::RawFile>(header);
				const auto name = utils::string::to_lower(rawfile->name.get());
				if (name.ends_with(".vision") && rawfile->buffer)
				{
					stage::level::dump_vision(name, source::rawfile::text(rawfile));
					break;
				}

				source::rawfile::dump(rawfile);
				if (name.ends_with("_fx.gsc") && rawfile->buffer)
				{
					level_script_sources[name] = source::rawfile::text(rawfile);
					if (!level_script_world.empty())
					{
						const auto world = level_script_world;
						dump_level(world.data());
					}
				}
				break;
			}
			case source::ASSET_TYPE_STRINGTABLE:
				source::string_table::dump(header_as<source::StringTable>(header));
				break;
			default:
				break;
			}
		}

		std::vector<std::string> required_zones(const std::string&)
		{
			return {"common_mp"};
		}
	}

	const converter& definition()
	{
		static const converter iw5_iw7
		{
			.source = "iw5",
			.target = "iw7",
			.dump_asset = dump_asset,
			.required_zones = required_zones,
		};

		return iw5_iw7;
	}
}
