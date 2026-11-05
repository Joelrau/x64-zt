#include <std_include.hpp>

#include "../include.hpp"
#include "scriptfile.hpp"
#include "material.hpp"
#include "scriptfile/fog.hpp"

#include <utils/compression.hpp>
#include <utils/string.hpp>

namespace zonetool::h1::converter::s2::scriptfile
{
	namespace
	{
		std::string convert_name(std::string name)
		{
			if (utils::string::is_numeric(name))
			{
				const auto token = h1_context().token_name(std::atoi(name.data()));
				if (!token.starts_with("_id_"))
				{
					name = token;
				}
			}

			return name + ".gsc";
		}

		std::string decompile(const ScriptFile* asset)
		{
			auto& context = h1_context();
			const auto stack = utils::compression::zlib::decompress({asset->buffer, static_cast<std::size_t>(asset->compressedLen)});
			const auto assembly = context.disassembler().disassemble(
				std::vector<std::uint8_t>{asset->bytecode, asset->bytecode + asset->bytecodeLen},
				std::vector<std::uint8_t>{stack.begin(), stack.end()});

			const auto tree = context.decompiler().decompile(*assembly);
			const auto source = context.printer().print(*tree);
			return {source.begin(), source.end()};
		}

		std::string replace_removed_builtins(const std::string& source)
		{
			static const std::regex is_using_hdr(R"(\bisusinghdr\s*\(\s*\))");
			return std::regex_replace(source, is_using_hdr, "1");
		}

		std::optional<std::string> make_s2_art_script(const std::string& name)
		{
			// the s2 client loads the vision and fog itself, visionsetnaked would clear the fog
			constexpr std::string_view prefix = "maps/createart/";
			constexpr std::string_view suffix = "_art.gsc";
			if (!name.starts_with(prefix) || !name.ends_with(suffix))
			{
				return {};
			}

			const auto map = name.substr(prefix.size(), name.size() - prefix.size() - suffix.size());
			return std::format("main()\n{{\n    level.tweakfile = 1;\n    maps\\createart\\{}_fog_hdr::setupfog();\n}}\n", map);
		}

		std::optional<std::string> make_s2_map_script(const std::string& name, const std::string& source)
		{
			const auto map = filesystem::get_fastfile();
			if (name != std::format("maps/mp/{}.gsc", map))
			{
				return {};
			}

			// s2 ambientplay is builtin 0x11A (sub_140514B40), gsc-tool has no name for it
			return make_map_setup_script(map, source, {.ambient_play = "_func_11A", .keep_dvars = false});
		}

		void dump_minimap(const std::string& source)
		{
			static const std::regex setup_minimap(R"re(\bsetupminimap\s*\(\s*"([^"]+)")re");
			std::smatch match;
			if (std::regex_search(source, match, setup_minimap))
			{
				material::dump_minimap(match[1].str());
			}
		}
	}

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

	std::string patch(const std::string& name, const std::string& source)
	{
		if (const auto art = make_s2_art_script(name))
		{
			return *art;
		}

		dump_minimap(source);
		if (const auto map_script = make_s2_map_script(name, source))
		{
			return *map_script;
		}

		const auto result = replace_removed_builtins(source);
		return name.starts_with("maps/createart/") && name.ends_with("_fog_hdr.gsc") ? convert_fog_file(result) : result;
	}

	void dump(ScriptFile* asset)
	{
		if (!asset->buffer || !asset->bytecode)
		{
			return;
		}

		// the rawfile of the same name has the original source, rawfile dumps it
		const auto name = convert_name(asset->name);
		if (zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_RAWFILE, name).rawfile)
		{
			return;
		}

		std::string source;
		if (!make_s2_art_script(name))
		{
			try
			{
				source = decompile(asset);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_WARNING("scriptfile \"%s\" does not decompile: %s", asset->name, e.what());
				return;
			}
		}

		source = patch(name, source);

		write_file(name, source);

		// s2x compiles a "<name>.gsc" rawfile in place of the stock script, so only the map scripts get a row
		if (std::filesystem::path(name).stem().string().starts_with(filesystem::get_fastfile()))
		{
			add_csv_row("rawfile", name);
		}
	}
}
