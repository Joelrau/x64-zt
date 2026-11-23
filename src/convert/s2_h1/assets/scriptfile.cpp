#include <std_include.hpp>

#include "../include.hpp"
#include "fxeffectdef.hpp"
#include "sound.hpp"
#include "xanim.hpp"
#include "scriptfile.hpp"

#include <utils/compression.hpp>
#include <utils/string.hpp>

namespace zonetool::s2::converter::h1::scriptfile
{
	namespace
	{
		struct sound_emitter
		{
			std::string alias;
			std::string origin;
		};

		std::unordered_set<std::string> default_map_scripts;
		std::unordered_set<std::string> dumped_scripts;
		std::string createfx_source;
		std::vector<sound_emitter> sound_emitters;

		std::string convert_name(std::string name)
		{
			if (utils::string::is_numeric(name))
			{
				const auto token = s2_context().token_name(std::atoi(name.data()));
				if (!token.starts_with("_id_"))
				{
					name = token;
				}
			}

			return name + ".gsc";
		}

		std::string decompile(const ScriptFile* asset)
		{
			auto& context = s2_context();
			const auto stack = utils::compression::zlib::decompress({asset->buffer, static_cast<std::size_t>(asset->compressedLen)});
			const auto assembly = context.disassembler().disassemble(
				std::vector<std::uint8_t>{asset->bytecode, asset->bytecode + asset->bytecodeLen},
				std::vector<std::uint8_t>{stack.begin(), stack.end()});

			const auto tree = context.decompiler().decompile(*assembly);
			const auto source = context.printer().print(*tree);
			return {source.begin(), source.end()};
		}

		std::string make_art_script(const std::string& map)
		{
			return std::format("main()\n{{\n    level.tweakfile = 1;\n    maps\\createart\\{}_fog_hdr::setupfog();\n}}\n", map);
		}

		std::unordered_map<std::string, std::vector<std::string>> split_functions(const std::string& source)
		{
			static const std::regex header(R"re((\w+)\(.*\))re");
			std::unordered_map<std::string, std::vector<std::string>> result;
			std::vector<std::string>* current = nullptr;
			for (auto line : utils::string::split(source, '\n'))
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				std::smatch match;
				if (!current && std::regex_match(line, match, header))
				{
					current = &result[match[1].str()];
				}

				if (current)
				{
					current->emplace_back(line);
					if (line == "}")
					{
						current = nullptr;
					}
				}
			}

			return result;
		}

		std::string make_map_script(const std::string& map, const std::string& source) // TODO: convert the precache, lighting and aud scripts, the minimap and the ambient alias
		{
			static const std::regex game_line(R"re(\s*game\["(attackers|defenders)"\] = .*;\s*)re");
			static const std::regex thread_line(R"re(\s*thread (\w+)\(\s*\);\s*)re");
			static const std::regex anim_call(R"re(scriptmodelplayanim\w*\( "([^"]+)")re");
			static const std::regex model_call(R"re(setmodel\( "([^"]+)")re");

			const auto functions = split_functions(source);
			const auto main = functions.find("main");
			std::string body, extra, precache;
			std::set<std::string> anims, models;
			for (const auto& line : main != functions.end() ? main->second : std::vector<std::string>{})
			{
				std::smatch match;
				if (std::regex_match(line, game_line))
				{
					body += line + "\n";
				}
				else if (std::regex_match(line, match, thread_line) && functions.contains(match[1].str()) && match[1].str() != "main")
				{
					body += line + "\n";
					extra += "\n";
					for (const auto& function_line : functions.at(match[1].str()))
					{
						if (function_line.contains("_id_"))
						{
							ZONETOOL_WARNING("map script: dropping s2 call \"%s\"", function_line.data());
							continue;
						}

						for (auto it = std::sregex_iterator(function_line.begin(), function_line.end(), anim_call); it != std::sregex_iterator(); ++it)
						{
							anims.insert((*it)[1].str());
						}

						for (auto it = std::sregex_iterator(function_line.begin(), function_line.end(), model_call); it != std::sregex_iterator(); ++it)
						{
							models.insert((*it)[1].str());
						}

						extra += function_line + "\n";
					}
				}
			}

			for (const auto& anim : anims)
			{
				if (!xanim::dump_used(anim))
				{
					ZONETOOL_WARNING("map script: xanim \"%s\" is not converted", anim.data());
				}

				precache += std::format("    precachempanim( \"{}\" );\n", anim);
			}

			for (const auto& model : models)
			{
				precache += std::format("    precachemodel( \"{}\" );\n", model);
			}

			auto script = std::format("main()\n{{\n{}    maps\\createart\\{}_art::main();\n", precache, map);
			if (source.contains(std::format("maps\\mp\\{}_fx::main()", map)))
			{
				script += std::format("    maps\\mp\\{}_fx::main();\n", map);
			}

			script += "    maps\\mp\\_load::main();\n";
			return script + body + "}\n" + extra;
		}

		std::string make_fx_script(const std::string& map, const std::string& source)
		{
			static const std::regex loadfx_line(R"re(\s*level\._effect\["([^"]+)"\] = loadfx\( "([^"]+)" \);\s*)re");

			std::string script = "main()\n{\n";
			for (const auto& line : utils::string::split(source, '\n'))
			{
				std::smatch match;
				if (!std::regex_match(line, match, loadfx_line))
				{
					continue;
				}

				const auto effect = db_find_x_asset_header_safe(ASSET_TYPE_FX, match[2].str()).fx;
				if (!effect)
				{
					ZONETOOL_WARNING("fx script: effect \"%s\" is not loaded, dropping it", match[2].str().data());
					continue;
				}

				fxeffectdef::dump(effect);
				script += std::format("\tlevel._effect[ \"{}\" ] = loadfx( \"{}\" );\n", match[1].str(), match[2].str());
			}

			return script + std::format("\n/#\n    if ( getdvar( \"clientSideEffects\" ) != \"1\" )\n        maps\\createfx\\{}_fx::main();\n#/\n\n}}\n", map);
		}

		std::vector<sound_emitter> read_sound_emitters(const std::string& source)
		{
			static const std::regex emitter_line(R"re(\s*\w+::_id_6842\( "([^"]+)", undefined, \( (.*) \) \);\s*)re");

			std::vector<sound_emitter> emitters;
			for (const auto& line : utils::string::split(source, '\n'))
			{
				std::smatch match;
				if (std::regex_match(line, match, emitter_line))
				{
					emitters.push_back({match[1].str(), match[2].str()});
				}
			}

			return emitters;
		}

		std::string make_sound_emitters(const std::vector<sound_emitter>& emitters, int& count)
		{
			std::string body;
			for (const auto& emitter : emitters)
			{
				if (!sound::dump_used(emitter.alias))
				{
					continue;
				}

				count++;
				body += std::format("\n\tent = createLoopSound();\n\tent set_origin_and_angles( ({}), (270, 0, 0) );\n\tent.v[ \"soundalias\" ] = \"{}\";\n",
					emitter.origin, emitter.alias);
			}

			return body;
		}

		std::string make_createfx_script(const std::string& source, const std::vector<sound_emitter>& emitters)
		{
			static const std::regex create_line(R"re(\s*var_\d+ = common_scripts\\utility::(create\w+)\( (.*) \);\s*)re");
			static const std::regex origin_line(R"re(\s*var_\d+ common_scripts\\_createfx::set_origin_and_angles\( (.*) \);\s*)re");
			static const std::regex key_line(R"re(\s*var_\d+\.v\["([^"]+)"\] = (.*);\s*)re");
			static const std::unordered_map<std::string, std::string> create_names
			{
				{"createoneshoteffect", "createOneshotEffect"},
				{"createloopeffect", "createLoopEffect"},
				{"createloopsound", "createLoopSound"},
				{"createexploder", "createExploder"},
				{"createintervalsound", "createIntervalSound"},
				{"createreactiveent", "createReactiveEnt"},
			};

			std::string body;
			auto count = 0;
			for (const auto& line : utils::string::split(source, '\n'))
			{
				std::smatch match;
				if (std::regex_match(line, match, create_line))
				{
					const auto name = create_names.find(match[1].str());
					body += std::format("\n\tent = {}( {} );\n", name != create_names.end() ? name->second : match[1].str(), match[2].str());
					count++;
				}
				else if (std::regex_match(line, match, origin_line))
				{
					body += std::format("\tent set_origin_and_angles( {} );\n", match[1].str());
				}
				else if (std::regex_match(line, match, key_line))
				{
					body += std::format("\tent.v[ \"{}\" ] = {};\n", match[1].str(), match[2].str());
				}
			}

			body += make_sound_emitters(emitters, count);
			return std::format("//_createfx generated. Do not touch!!\n#include common_scripts\\utility;\n#include common_scripts\\_createfx;\n\n\nmain()\n{{\n\t// CreateFX fx entities placed: {}{}}}\n", count, body);
		}

		std::string make_default_map_script()
		{
			return "main()\n{\n    maps\\mp\\_load::main();\n    game[\"attackers\"] = \"allies\";\n    game[\"defenders\"] = \"axis\";\n}\n";
		}

		void dump_script(const std::string& name, const std::string& source)
		{
			write_file(name, source);
			add_csv_row("rawfile", name);
			dumped_scripts.insert(name);
		}
	}

	xsk::gsc::s2::context& s2_context()
	{
		static auto context = []
		{
			auto result = std::make_unique<xsk::gsc::s2::context>(xsk::gsc::instance::server);
			result->init(xsk::gsc::build::prod, {});
			return result;
		}();
		return *context;
	}

	void dump(ScriptFile* asset)
	{
		if (!asset->buffer || !asset->bytecode)
		{
			return;
		}

		const auto name = convert_name(asset->name);
		const auto map = filesystem::get_fastfile();
		if (name == std::format("maps/createart/{}_art.gsc", map))
		{
			dump_script(name, make_art_script(map));
			return;
		}

		const auto is_map = name == std::format("maps/mp/{}.gsc", map);
		const auto is_fx = name == std::format("maps/mp/{}_fx.gsc", map);
		const auto is_createfx = name == std::format("maps/createfx/{}_fx.gsc", map);
		const auto is_aud = name == std::format("maps/mp/{}_aud.gsc", map);
		if (!is_map && !is_fx && !is_createfx && !is_aud)
		{
			return;
		}

		try
		{
			const auto source = decompile(asset);
			if (is_aud)
			{
				sound_emitters = read_sound_emitters(source);
			}
			else if (is_createfx)
			{
				createfx_source = source;
			}
			else
			{
				dump_script(name, is_map ? make_map_script(map, source) : make_fx_script(map, source));
			}
		}
		catch (const std::exception& e)
		{
			ZONETOOL_WARNING("scriptfile \"%s\" does not decompile: %s", asset->name, e.what());
		}
	}

	void add_default_map_script(const std::string& name)
	{
		default_map_scripts.insert(name);
	}

	void finish()
	{
		if (!createfx_source.empty() || !sound_emitters.empty())
		{
			dump_script(std::format("maps/createfx/{}_fx.gsc", filesystem::get_fastfile()), make_createfx_script(createfx_source, sound_emitters));
		}

		createfx_source.clear();
		sound_emitters.clear();

		for (const auto& name : default_map_scripts)
		{
			if (!dumped_scripts.contains(name))
			{
				dump_script(name, make_default_map_script());
			}
		}

		default_map_scripts.clear();
		dumped_scripts.clear();
	}
}
