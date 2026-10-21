#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"
#include "material.hpp"
#include "scriptable.hpp"
#include "techset.hpp"

#include <utils/compression.hpp>

#include <xsk/gsc/engine/h1.hpp>
#include <xsk/gsc/engine/s2.hpp>

namespace zonetool::h1::converter::s2::mapents
{
	namespace
	{
		xsk::gsc::h1::context& h1_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::h1::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
		}

		xsk::gsc::s2::context& s2_context()
		{
			static xsk::gsc::s2::context context{xsk::gsc::instance::server};
			return context;
		}

		int* widen_array(const short* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			if (!source || !count)
			{
				return nullptr;
			}

			const auto array = allocator.allocate_array<int>(count);
			for (auto i = 0u; i < count; i++)
			{
				array[i] = source[i];
			}

			return array;
		}

		void convert_client_triggers(const ClientTriggers& trigger, zonetool::s2::ClientTriggers& new_trigger, utils::memory::allocator& allocator)
		{
			new_trigger.trigger = convert_triggers(trigger.trigger, allocator);

			const auto count = trigger.trigger.count;
			new_trigger.numClientTriggerNodes = trigger.numClientTriggerNodes;
			new_trigger.clientTriggerAabbTree = reinterpret_cast<zonetool::s2::ClientTriggerAabbNode*>(trigger.clientTriggerAabbTree);
			new_trigger.triggerStringLength = trigger.triggerStringLength;
			new_trigger.triggerString = trigger.triggerString;
			new_trigger.visionSetTriggers = widen_array(trigger.visionSetTriggers, count, allocator);
			new_trigger.lightSetTriggers = widen_array(trigger.lightSetTriggers, count, allocator);
			new_trigger.clutTriggers = widen_array(trigger.clutTriggers, count, allocator);
			new_trigger.triggerType = trigger.triggerType;
			new_trigger.origins = reinterpret_cast<zonetool::s2::vec3_t*>(trigger.origins);
			new_trigger.scriptDelay = trigger.scriptDelay;
			new_trigger.audioTriggers = trigger.audioTriggers;
			new_trigger.blendLookup = trigger.blendLookup;
			new_trigger.npcTriggers = trigger.npcTriggers;
			new_trigger.contextTriggers = trigger.contextTriggers;
			new_trigger.waterTriggers = trigger.waterTriggers;
		}

		const std::unordered_map<std::string, std::uint32_t> s2_unnamed_keys
		{
			{"animscript", 30},
			{"height", 189},
			{"script_exploder", 33158},
			{"script_gameobjectname", 33214},
			{"script_label", 33256},
		};

		std::optional<std::string> convert_key(const std::string& name)
		{
			if (const auto id = s2_context().token_id(name); id && !name.starts_with("_id_"))
			{
				return std::to_string(id);
			}

			if (const auto entry = s2_unnamed_keys.find(name); entry != s2_unnamed_keys.end())
			{
				return std::to_string(entry->second);
			}

			ZONETOOL_WARNING("map_ents: S2 does not know key \"%s\", dropping it", name.data());
			return {};
		}

		std::optional<std::string> convert_key(const std::uint32_t h1_id)
		{
			return convert_key(h1_context().token_name(h1_id));
		}

		std::optional<std::string> convert_line(const std::string& line)
		{
			if (line.starts_with("\""))
			{
				const auto key_end = line.find('"', 1);
				if (key_end == std::string::npos)
				{
					return line;
				}

				const auto key = convert_key(line.substr(1, key_end - 1));
				return key ? std::optional{*key + line.substr(key_end + 1)} : std::nullopt;
			}

			const auto first_space = line.find_first_of(' ');
			const auto id = static_cast<std::uint32_t>(std::atoi(line.data()));
			if (line.starts_with("{") || line.starts_with("}") || first_space == std::string::npos || id == 0)
			{
				return line;
			}

			const auto key = convert_key(id);
			return key ? std::optional{*key + line.substr(first_space)} : std::nullopt;
		}

		std::string replace_entity_keys(const std::string& data)
		{
			std::string buffer{};
			for (const auto& line : utils::string::split(data.substr(0, data.find('\0')), '\n'))
			{
				const auto new_line = convert_line(line);
				if (!new_line)
				{
					continue;
				}

				if (!buffer.empty())
				{
					buffer.append("\n");
				}

				buffer.append(*new_line);
			}

			return buffer;
		}

		zonetool::s2::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::MapEnts>();
			new_asset->name = asset->name;

			const auto entity_string = replace_entity_keys({asset->entityString, static_cast<std::size_t>(asset->numEntityChars)});
			new_asset->numEntityChars = static_cast<int>(entity_string.size() + 1);
			new_asset->entityString = allocator.allocate_array<char>(new_asset->numEntityChars);
			std::memcpy(new_asset->entityString, entity_string.data(), entity_string.size());

			new_asset->trigger = convert_triggers(asset->trigger, allocator);
			convert_client_triggers(asset->clientTrigger, new_asset->clientTrigger, allocator);

			new_asset->clientTriggerBlend.numClientTriggerBlendNodes = asset->clientTriggerBlend.numClientTriggerBlendNodes;
			new_asset->clientTriggerBlend.blendNodes = reinterpret_cast<zonetool::s2::ClientTriggerBlendNode*>(asset->clientTriggerBlend.blendNodes);

			new_asset->spawnList.spawnsCount = asset->spawnList.spawnsCount;
			new_asset->spawnList.spawns = reinterpret_cast<zonetool::s2::SpawnPointEntityRecord*>(asset->spawnList.spawns);

			new_asset->splineList.splineCount = asset->splineList.splineCount;
			new_asset->splineList.splines = reinterpret_cast<zonetool::s2::SplinePointRecordList*>(asset->splineList.splines);

			if (const auto* clip_map = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_CLIPMAP, asset->name).clipMap)
			{
				new_asset->scriptableMapEnts = scriptable::convert_map_ents(clip_map->scriptableMapEnts, allocator);
			}

			return new_asset;
		}

		std::string map_script_name(const std::string& ents_name)
		{
			return ents_name.substr(0, ents_name.find(".d3dbsp"));
		}

		std::string find_minimap_material(const std::string& script_name)
		{
			const auto* script = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_SCRIPTFILE, script_name).scriptfile;
			if (!script || !script->buffer || !script->bytecode)
			{
				return {};
			}

			const auto stack = utils::compression::zlib::decompress({script->buffer, static_cast<std::size_t>(script->compressedLen)});
			const auto assembly = h1_context().disassembler().disassemble(
				std::vector<std::uint8_t>{script->bytecode, script->bytecode + script->bytecodeLen},
				std::vector<std::uint8_t>{stack.begin(), stack.end()});

			for (const auto& function : assembly->functions)
			{
				for (const auto& instruction : function->instructions)
				{
					if (instruction->opcode == xsk::gsc::opcode::OP_GetString && instruction->data[0].starts_with("compass_map_"))
					{
						return instruction->data[0];
					}
				}
			}

			return {};
		}

		void dump_map_script(const std::string& script_name)
		{
			std::string source =
				"main()\n"
				"{\n"
				"\t_id_483::main();\n";

			if (const auto minimap = find_minimap_material(script_name); !minimap.empty())
			{
				source += "\t_id_472::_id_8A2F(\"" + minimap + "\");\n"; // setupminimap
				material::dump_minimap(minimap);
			}
			else
			{
				ZONETOOL_WARNING("script \"%s\" sets no minimap, the compass shows no map", script_name.data());
			}

			source += "}\n";

			auto& context = s2_context();
			static auto initialized = false;
			if (!initialized)
			{
				context.init(xsk::gsc::build::prod, [](const xsk::gsc::context*, const std::string&) -> std::pair<xsk::gsc::buffer, std::vector<std::uint8_t>>
				{
					return {};
				});
				initialized = true;
			}

			std::vector<std::uint8_t> data{source.begin(), source.end()};
			const auto assembly = context.compiler().compile(script_name, data);
			const auto [bytecode, stack, devmap] = context.assembler().assemble(*assembly);

			const auto compressed_stack = utils::compression::zlib::compress({reinterpret_cast<const char*>(stack.data), stack.size});

			zonetool::s2::ScriptFile script{};
			script.name = script_name.data();
			script.compressedLen = static_cast<int>(compressed_stack.size());
			script.len = static_cast<int>(stack.size);
			script.bytecodeLen = static_cast<int>(bytecode.size);
			script.buffer = compressed_stack.data();
			script.bytecode = reinterpret_cast<char*>(const_cast<std::uint8_t*>(bytecode.data));

			zonetool::s2::scriptfile::dump(&script);
			add_csv_row("scriptfile", script_name);
		}

		void dump_level_net_const_strings()
		{
			for (const auto header : techset::donor_zone().find_assets(zonetool::s2::ASSET_TYPE_NET_CONST_STRINGS))
			{
				auto asset = *static_cast<zonetool::s2::NetConstStrings*>(header);
				asset.entryCount = 0;
				asset.stringList = nullptr;
				zonetool::s2::net_const_strings::dump(&asset);
				add_csv_row("netconststrings", asset.name);
			}
		}

		std::string read_rawfile(const RawFile& asset)
		{
			if (!asset.compressedLen)
			{
				return {asset.buffer, static_cast<std::size_t>(asset.len)};
			}

			return utils::compression::zlib::decompress({asset.buffer, static_cast<std::size_t>(asset.compressedLen)});
		}

		constexpr std::string_view s2_fog_keys = // sub_1403D9200 keeps the last fog file values for missing keys
			"\n\tent.atmosFogEntityScale = 1;"
			"\n\tent.atmosFogSplineEnabled = 0;";

		void dump_fog(const std::string& map_name)
		{
			const auto name = "maps/createart/" + map_name + "_fog_hdr.gsc";
			const auto* fog = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_RAWFILE, name).rawfile;
			if (!fog || !fog->buffer)
			{
				ZONETOOL_WARNING("rawfile \"%s\" not found, the map has no fog", name.data());
				return;
			}

			constexpr std::string_view fog_entry = "create_vision_set_fog(";
			auto text = read_rawfile(*fog);
			for (auto entry = text.find(fog_entry); entry != std::string::npos; entry = text.find(fog_entry, entry))
			{
				const auto line_end = text.find(';', entry);
				if (line_end == std::string::npos)
				{
					break;
				}

				text.insert(line_end + 1, s2_fog_keys);
				entry = line_end + 1 + s2_fog_keys.size();
			}

			zonetool::s2::RawFile asset{name.data(), 0, static_cast<int>(text.size()), text.data()};
			zonetool::s2::rawfile::dump(&asset);
			add_csv_row("rawfile", asset.name);
		}

		struct lightset_value
		{
			const char* key;
			const char* value;
		};

		constexpr lightset_value h1_lightset[] = // h1 registration defaults (sub_69AD0), h1 mp maps have no lightsets table
		{
			{"sunenable", "1"},
			{"sunshadowscale", "1"},
			{"spotupdatelimit", "4"},
			{"usedsuncascadecount", "2"},
			{"sunsamplesizenear", "0.25"},
			{"sunfilterradius", "4"},
			{"spotfilterradius", "4"},
			{"veil", "0"},
			{"veilstrength", "0.087"},
			{"veilbackgroundstrength", "0.913"},
			{"tonemap", "0"}, // the h1 linear operator overexposes in s2, the operator and exposure keep the stock s2 values
			{"tonemapauto", "1"},
			{"tonemapblend", "1"},
			{"tonemaplockautoexposureadjust", "0"},
			{"tonemapautoexposureadjust", "0"},
			{"tonemapexposure", "-10"},
			{"tonemapexposureadjust", "0"},
			{"tonemapmaxexposure", "-10"},
			{"tonemapadaptspeed", "0.02"},
			{"tonemapdarkev", "2.84"},
			{"tonemapmidev", "7.823"},
			{"tonemaplightev", "12.81"},
			{"tonemapdarkexposureadjust", "-3.17"},
			{"tonemapmidexposureadjust", "-0.0651"},
			{"tonemaplightexposureadjust", "1.47"},
			{"tonemapminexposureadjust", "-3.17"},
			{"tonemapmaxexposureadjust", "2.3"},
			{"tonemapwhite", "512"},
			{"tonemapshoulder", "0.95"},
			{"tonemapcrossover", "2"},
			{"tonemaptoe", "0.2"},
			{"tonemapblack", "0"},
			{"hemiaostrength", "0.5"},
			{"hemiaopower", "1"},
			{"hemiaorejectionfalloff", "0.5"},
			{"mdao", "0"},
			{"mdaooccluderculldistance", "1000"},
			{"mdaooccluderfadeoutstartdistance", "900"},
			{"mdaocapsulestrength", "1"},
			{"mdaovolumestrength", "1"},
			{"wbMethod", "0"}, // the s2 default white balance shifts the colors
		};

		void dump_lightsets(const std::string& map_name)
		{
			utils::memory::allocator allocator;
			const auto name = "maps/createart/" + map_name + "_lightsets_hdr.csv";

			zonetool::s2::StringTable asset{};
			asset.name = name.data();
			asset.columnCount = 2;
			asset.rowCount = 1 + static_cast<int>(std::size(h1_lightset));
			asset.values = allocator.allocate_array<zonetool::s2::StringTableCell>(asset.rowCount * asset.columnCount);
			asset.values[0].string = "name";
			asset.values[1].string = map_name.data();
			for (auto i = 0u; i < std::size(h1_lightset); i++)
			{
				asset.values[2 + i * 2].string = h1_lightset[i].key;
				asset.values[3 + i * 2].string = h1_lightset[i].value;
			}

			zonetool::s2::string_table::dump(&asset);
			add_csv_row("stringtable", asset.name);
		}

		const std::unordered_map<std::string, std::string> s2_sun_dvars = // s2 sun files name each dvar by its id
		{
			{"r_sunsprite_shader", "4843"},
			{"r_sunsprite_size", "4337"},
			{"r_sunflare_shader", "5654"},
			{"r_sunflare_min_size", "2938"},
			{"r_sunflare_min_angle", "1834"},
			{"r_sunflare_max_size", "89"},
			{"r_sunflare_max_angle", "3627"},
			{"r_sunflare_max_alpha", "2957"},
			{"r_sunflare_fadein", "371"},
			{"r_sunflare_fadeout", "1615"},
			{"r_sunblind_min_angle", "5397"},
			{"r_sunblind_max_angle", "3858"},
			{"r_sunblind_max_darken", "1590"},
			{"r_sunblind_fadein", "4939"},
			{"r_sunblind_fadeout", "407"},
			{"r_sunglare_min_angle", "3682"},
			{"r_sunglare_max_angle", "970"},
			{"r_sunglare_max_lighten", "4772"},
			{"r_sunglare_fadein", "3538"},
			{"r_sunglare_fadeout", "438"},
			{"r_sun_fx_position", "3196"},
		};

		void dump_sun(const std::string& map_name)
		{
			const auto name = "sun/" + map_name + ".sun";
			const auto* sun = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_RAWFILE, name).rawfile;
			if (!sun || !sun->buffer)
			{
				ZONETOOL_WARNING("rawfile \"%s\" not found, the map has no sun sprite", name.data());
				return;
			}

			std::string text;
			for (auto line : utils::string::split(read_rawfile(*sun), '\n'))
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				const auto separator = line.find(' ');
				const auto dvar = line.substr(0, separator);
				if (dvar.empty())
				{
					continue;
				}

				const auto id = s2_sun_dvars.find(dvar);
				if (id == s2_sun_dvars.end() || separator == std::string::npos)
				{
					ZONETOOL_WARNING("sun file \"%s\": S2 does not know dvar \"%s\", dropping it", name.data(), dvar.data());
					continue;
				}

				text += id->second + line.substr(separator) + "\n";
			}

			zonetool::s2::RawFile asset{name.data(), 0, static_cast<int>(text.size()), text.data()};
			zonetool::s2::rawfile::dump(&asset);
			add_csv_row("rawfile", asset.name);
		}

		void dump_donor_map_settings(const std::string& map_name)
		{
			utils::memory::allocator allocator;
			const auto& zone = techset::donor_zone();
			const std::string donor_name{techset::donor_zone_name};
			const auto rename = [&](const char* name)
			{
				return allocator.duplicate_string(utils::string::replace(name, donor_name, map_name));
			};

			for (const auto header : zone.find_assets(zonetool::s2::ASSET_TYPE_RAWFILE))
			{
				auto asset = *static_cast<zonetool::s2::RawFile*>(header);
				if (std::string_view(asset.name).starts_with("vision/" + donor_name))
				{
					asset.name = rename(asset.name);
					zonetool::s2::rawfile::dump(&asset);
					add_csv_row("rawfile", asset.name);
				}
			}

			for (const auto header : zone.find_assets(zonetool::s2::ASSET_TYPE_CLUT))
			{
				auto asset = *static_cast<zonetool::s2::Clut*>(header);
				if (asset.image && asset.image->name)
				{
					const auto is_global = zone.is_global_image(asset.image->name);
					if (!is_global)
					{
						db_scope _(zone.db());
						zonetool::s2::gfx_image::dump(asset.image);
					}
					add_csv_row("image", asset.image->name, is_global);
				}

				asset.name = rename(asset.name);
				zonetool::s2::clut::dump(&asset);
				add_csv_row("clut", asset.name);
			}
		}
	}

	zonetool::s2::MapTriggers convert_triggers(const MapTriggers& trigger, utils::memory::allocator& allocator)
	{
		zonetool::s2::MapTriggers new_trigger{};
		new_trigger.count = trigger.count;
		new_trigger.models = allocator.allocate_array<zonetool::s2::TriggerModel>(trigger.count);
		for (auto i = 0u; i < trigger.count; i++)
		{
			new_trigger.models[i].contents = static_cast<std::uint32_t>(trigger.models[i].contents);
			new_trigger.models[i].hullCount = trigger.models[i].hullCount;
			new_trigger.models[i].firstHull = trigger.models[i].firstHull;
		}

		new_trigger.hullCount = trigger.hullCount;
		new_trigger.hulls = allocator.allocate_array<zonetool::s2::TriggerHull>(trigger.hullCount);
		for (auto i = 0u; i < trigger.hullCount; i++)
		{
			new_trigger.hulls[i].bounds = std::bit_cast<zonetool::s2::Bounds>(trigger.hulls[i].bounds);
			new_trigger.hulls[i].contents = static_cast<std::uint32_t>(trigger.hulls[i].contents);
			new_trigger.hulls[i].slabCount = trigger.hulls[i].slabCount;
			new_trigger.hulls[i].firstSlab = trigger.hulls[i].firstSlab;
		}

		new_trigger.slabCount = trigger.slabCount;
		new_trigger.slabs = reinterpret_cast<zonetool::s2::TriggerSlab*>(trigger.slabs);
		return new_trigger;
	}

	void dump(MapEnts* asset)
	{
		utils::memory::allocator allocator;
		const auto new_asset = convert(asset, allocator);
		zonetool::s2::map_ents::dump(new_asset);
		add_csv_row("map_ents", new_asset->name);
		const auto script_name = map_script_name(new_asset->name);
		dump_map_script(script_name);
		dump_level_net_const_strings();

		// TODO port the real values
		const auto map_name = std::filesystem::path(script_name).filename().string();
		dump_donor_map_settings(map_name);
		dump_lightsets(map_name);
		dump_sun(map_name);
		dump_fog(map_name);
	}
}
