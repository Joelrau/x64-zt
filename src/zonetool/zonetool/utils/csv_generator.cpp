#include <std_include.hpp>

#include "utils.hpp"
#include "csv_generator.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>

namespace csv_generator
{
	namespace
	{
		std::unordered_set<std::string> parse_create_fx_gsc(const std::string& data)
		{
			std::unordered_set<std::string> sounds;

			const auto lines = utils::string::split(data, '\n');
			for (const auto& line : lines)
			{
				std::regex expr(R"~(^(?:(?:\s|)+).+\.v\[(?:(?:\s|)+)"soundalias"(?:(?:\s|)+)\](?:(?:\s|)+)=(?:(?:\s|)+)"(.+)"(?:(?:\s|)+);.*(?:\r|)+$)~");
				std::smatch match{};
				if (!std::regex_search(line, match, expr))
				{
					continue;
				}

				const auto sound = match[1].str();
				sounds.insert(sound);
			}

			return sounds;
		}

		std::unordered_set<std::string> parse_fx_gsc(const std::string& data)
		{
			std::unordered_set<std::string> effects;

			const auto lines = utils::string::split(data, '\n');
			for (const auto& line : lines)
			{
				std::regex expr(R"~(^(?:(?:\s|)+)level\..+\[.+\](?:(?:\s|)+)=(?:(?:\s|)+)loadfx\((?:(?:\s|)+)"(.+)"(?:(?:\s|)+)\);.*(?:\r|)+$)~");
				std::smatch match{};
				if (!std::regex_search(line, match, expr))
				{
					continue;
				}

				const auto fx = match[1].str();
				effects.insert(fx);
			}

			return effects;
		}
	}

	void generate_map_csv(const std::string& map, const mapents::token_name_callback& get_token_name, bool is_sp)
	{
		const auto root_dir = "zonetool/" + map;
		if (!utils::io::directory_exists(root_dir))
		{
			ZONETOOL_ERROR("Zonetool map directory \"%s\" does not exist", map.data());
			return;
		}

		const auto map_prefix = map.starts_with("mp_")
			? "maps/mp"s
			: "maps"s;
		const auto map_prefix_full = root_dir + "/" + map_prefix + "/";
		const auto mapents_path = map_prefix_full + map + ".d3dbsp.ents";

		std::string mapents_data;
		if (!utils::io::read_file(mapents_path, &mapents_data))
		{
			ZONETOOL_ERROR("Missing mapents");
			return;
		}

		ZONETOOL_INFO("Generating CSV for map \"%s\"", map.data());
		ZONETOOL_INFO("Parsing mapents...");

		const auto list = mapents::parse(mapents_data, get_token_name);

		std::string csv;

		const auto add_line = [&](const std::string& line)
		{
			csv.append(line);
			csv.append("\n");
		};

		add_line("// Generated by x64-ZoneTool\n");

		if (!is_sp)
		{
			add_line("// netconststrings");
			add_line("netconststrings,ncs_lsr_level");
			add_line("netconststrings,ncs_lui_level");
			add_line("netconststrings,ncs_acl_level");
			add_line("netconststrings,ncs_fxt_level");
			add_line("netconststrings,ncs_anm_level");
			add_line("netconststrings,ncs_hnt_level");
			add_line("netconststrings,ncs_att_level");
			add_line("netconststrings,ncs_wep_level");
			add_line("netconststrings,ncs_sel_level");
			add_line("netconststrings,ncs_mic_level");
			add_line("netconststrings,ncs_nps_level");
			add_line("netconststrings,ncs_hic_level");
			add_line("netconststrings,ncs_tag_level");
			add_line("netconststrings,ncs_mnu_level");
			add_line("netconststrings,ncs_shk_level");
			add_line("netconststrings,ncs_snl_level");
			add_line("netconststrings,ncs_sbx_level");
			add_line("netconststrings,ncs_snd_level");
			add_line("netconststrings,ncs_loc_level");
			add_line("netconststrings,ncs_vfx_level");
			add_line("netconststrings,ncs_veh_level");
			add_line("netconststrings,ncs_rmb_level");
			add_line("netconststrings,ncs_mat_level");
			add_line("netconststrings,ncs_mdl_level");
			add_line("");
		}

		std::unordered_set<std::string> models;

		auto added_models_comment = false;
		for (const auto& ent : list.entities)
		{
			if (ent.get("classname") != "script_model")
			{
				continue;
			}

			const auto model = ent.get("model");
			if (model == "")
			{
				continue;
			}

			if (models.contains(model))
			{
				continue;
			}

			if (!added_models_comment)
			{
				added_models_comment = true;
				add_line("// models");
			}

			models.insert(model);
			add_line(utils::string::va("xmodel,%s", model.data()));
		}

		if (models.size() > 0)
		{
			add_line("");
		}

		const auto add_sounds = [&]
		{
			const std::string create_fx_name = utils::string::va("maps/createfx/%s_fx.gsc", map.data());
			const std::string create_fx_path = utils::string::va("%s/%s", root_dir.data(), create_fx_name.data());

			std::string data;
			if (!utils::io::read_file(create_fx_path, &data))
			{
				return;
			}

			ZONETOOL_INFO("Parsing createfx gsc...");

			add_line("// sounds");
			add_line(utils::string::va("rawfile,%s", create_fx_name.data()));
			const auto sounds = parse_create_fx_gsc(data);
			for (const auto& sound : sounds)
			{
				add_line(utils::string::va("sound,%s", sound.data()));
			}

			add_line("");
		};

		const auto add_effects = [&]
		{
			const std::string fx_name = utils::string::va("%s/%s_fx.gsc", map_prefix.data(), map.data());
			const std::string fx_path = utils::string::va("%s/%s", root_dir.data(), fx_name.data());

			std::string data;
			if (!utils::io::read_file(fx_path, &data))
			{
				return;
			}

			ZONETOOL_INFO("Parsing fx gsc...");

			add_line("// effects");
			add_line(utils::string::va("rawfile,%s", fx_name.data()));

			const auto effects = parse_fx_gsc(data);
			for (const auto& effect : effects)
			{
				add_line(utils::string::va("fx,%s", effect.data()));
			}

			add_line("");
		};

		add_sounds();
		add_effects();

		const auto add_map_asset = [&](const std::string& type)
		{
			add_line(utils::string::va("%s,%s/%s.d3dbsp", type.data(), map_prefix.data(), map.data()));
		};

		const auto add_iterator = [&](const std::string& type, const std::string& folder,
			const std::string& extension, const std::string& comment)
		{
			auto added_comment = false;
			const auto folder_ = root_dir + "/" + folder;
			if (!utils::io::directory_exists(folder_))
			{
				return;
			}

			const auto files = utils::io::list_files(folder_);
			for (const auto& file : files)
			{
				if (!file.ends_with(extension))
				{
					continue;
				}

				if (!added_comment)
				{
					added_comment = true;
					add_line(comment);
				}

				const std::string name = folder + file.substr(folder_.size());
				ZONETOOL_INFO("Adding %s \"%s\"", type.data(), name.data());
				add_line(utils::string::va("%s,%s", type.data(), name.data()));
			}

			if (added_comment)
			{
				add_line("");
			}
		};

		const auto add_if_exists = [&](const std::string& text, const std::string& path)
		{
			if (!utils::io::file_exists(root_dir + "/" + path))
			{
				return false;
			}

			add_line(text);
			return true;
		};

		{
			const std::string compass_name = utils::string::va("compass_map_%s", map.data());
			const std::string compass_path = utils::string::va("materials/%s.json", compass_name.data());
			add_if_exists(utils::string::va("// compass\nmaterial,compass_map_%s\n", map.data()), compass_path);
		}

		add_iterator("stringtable", "maps/createart/", ".csv", "// lightsets");
		add_iterator("clut", "clut/", ".clut", "// color lookup tables");
		add_iterator("rawfile", "vision/", ".vision", "// visions");
		add_iterator("rawfile", "sun/", ".sun", "// sun");

		ZONETOOL_INFO("Adding map assets...");

		add_line("// map assets");
		add_map_asset("com_map");
		add_map_asset("fx_map");
		add_map_asset("gfx_map");
		add_map_asset("map_ents");
		add_map_asset("glass_map");
		add_map_asset("phys_worldmap");
		add_map_asset("aipaths");
		add_map_asset("col_map_mp");
		add_line("");

		const auto csv_path = "zone_source/" + map + ".csv";

		ZONETOOL_INFO("CSV saved to %s", csv_path.data());
		utils::io::write_file(csv_path, csv, false);
	}
}