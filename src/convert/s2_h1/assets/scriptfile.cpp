#include <std_include.hpp>

#include "../include.hpp"
#include "scriptfile.hpp"

#include <utils/compression.hpp>
#include <utils/string.hpp>

namespace zonetool::s2::converter::h1::scriptfile
{
	namespace
	{
		std::unordered_set<std::string> default_map_scripts;
		std::unordered_set<std::string> dumped_scripts;

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

		std::string make_map_script(const std::string& map, const std::string& source) // TODO: convert the precache, fx, lighting and aud scripts, the minimap and the ambient alias
		{
			static const std::regex game_line(R"re(\s*game\["(attackers|defenders)"\] = .*;\s*)re");

			auto script = std::format("main()\n{{\n    maps\\createart\\{}_art::main();\n    maps\\mp\\_load::main();\n", map);
			for (auto line : utils::string::split(source, '\n'))
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				if (std::regex_match(line, game_line))
				{
					script += line + "\n";
				}
			}

			return script + "}\n";
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

		if (name != std::format("maps/mp/{}.gsc", map))
		{
			return;
		}

		try
		{
			dump_script(name, make_map_script(map, decompile(asset)));
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
