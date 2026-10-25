#include <std_include.hpp>

#include "../include.hpp"
#include "scriptfile.hpp"
#include "material.hpp"

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
	}

	namespace
	{
		// atmos fog keys h1 does not write, values of the stock s2 neutral fog blocks; the parser (2.5 sub_1403D9200) keeps the
		// previous or zero value of a missing key, entity scale 0 leaves models unfogged
		constexpr std::array<std::pair<std::string_view, std::string_view>, 24> s2_atmos_fog_keys
		{{
			{"atmosFogExtinctionDesaturation", "0"},
			{"atmosFogHeightFogMaximumOffset", "0"},
			{"atmosFogHeightFogStrengthOfDepthFog", "0"},
			{"atmosFogEntityScale", "1"},
			{"atmosFogAirLightCapacity", "0"}, // stock main map blocks use 2, but h1 has no air lights: they haze the room around every bright primary light
			{"atmosFogAirLightDensityScalar", "1"},
			{"atmosFogAirLightVertexInnerMutedRadius", "0"},
			{"atmosFogSplineEnabled", "0"},
			{"atmosFogSplineCustomDistance1", "0"},
			{"atmosFogSplineCustomDistance2", "0"},
			{"atmosFogSplineCustomDistance3", "0"},
			{"atmosFogSplineMaxDistance", "0"},
			{"atmosFogSplineCustomVisibility1", "0"},
			{"atmosFogSplineCustomVisibility2", "0"},
			{"atmosFogSplineCustomVisibility3", "0"},
			{"atmosFogSplineMaxDistanceVisibility", "0"},
			{"atmosFogSplineUseMaxDistanceVisibilityOverride", "0"},
			{"atmosFogSplineMaxDistanceVisibilityOverride", "(0, 0, 0)"},
			{"atmosFogSplineCustomTangentsEnabled", "0"},
			{"atmosFogSplineStartingTangentAngle", "0"},
			{"atmosFogSplineCustomTangentAngle1", "0"},
			{"atmosFogSplineCustomTangentAngle2", "0"},
			{"atmosFogSplineCustomTangentAngle3", "0"},
			{"atmosFogSplineMaxDistanceTangentAngle", "0"},
		}};

		std::string fog_key(const std::string_view line)
		{
			if (!line.starts_with("ent."))
			{
				return {};
			}

			const auto end = line.find_first_of(" =", 4);
			return utils::string::to_lower(std::string(line.substr(4, end == std::string_view::npos ? end : end - 4)));
		}

		// every create_vision_set_fog block gets the s2 atmos fog keys it lacks after its last "ent." line
		std::string convert_fog_file(const std::string& source)
		{
			std::vector<std::string> lines;
			std::unordered_set<std::string> keys;
			std::optional<std::size_t> block_end;
			const auto finish_block = [&]
			{
				if (!block_end)
				{
					return;
				}

				std::vector<std::string> missing;
				for (const auto& [key, value] : s2_atmos_fog_keys)
				{
					if (!keys.contains(utils::string::to_lower(std::string(key))))
					{
						missing.emplace_back(std::format("\tent.{} = {};", key, value));
					}
				}

				lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(*block_end), missing.begin(), missing.end());
				keys.clear();
				block_end.reset();
			};

			for (auto line : utils::string::split(source, '\n'))
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				const auto first = line.find_first_not_of(" \t");
				const auto trimmed = first == std::string::npos ? std::string{} : line.substr(first);
				if (trimmed == "/*" || trimmed == "*/") // h1 comments out setupfog(), the s2 art script calls it
				{
					continue;
				}

				if (trimmed == "/$" || trimmed == "$/") // gsc compilers only know "/#" dev blocks, the client fog parser skips both
				{
					line.replace(first, 2, trimmed == "/$" ? "/#" : "#/");
				}
				if (trimmed.find("create_vision_set_fog(") != std::string::npos || trimmed == "}")
				{
					finish_block();
				}

				lines.emplace_back(line);
				if (trimmed.find("create_vision_set_fog(") != std::string::npos)
				{
					block_end = lines.size();
				}
				else if (const auto key = fog_key(trimmed); block_end && !key.empty())
				{
					keys.insert(key);
					block_end = lines.size();
				}
			}

			finish_block();

			std::string text;
			for (const auto& line : lines)
			{
				text += line + "\n";
			}

			static const std::regex setup_fog(R"((^|\n)\s*setupfog\s*\()", std::regex::icase);
			if (!std::regex_search(text, setup_fog)) // custom maps ship no setupfog(), the s2 art script still links it
			{
				text += "\nsetupfog()\n{\n}\n";
			}

			return text;
		}

		// s2 dropped isusinghdr. always renders hdr
		std::string replace_removed_builtins(const std::string& source)
		{
			static const std::regex is_using_hdr(R"(\bisusinghdr\s*\(\s*\))");
			return std::regex_replace(source, is_using_hdr, "1");
		}

		// s2 art scripts only run the fog file's setupfog(), the client loads vision and fog itself and visionsetnaked clears the fog
		std::optional<std::string> make_s2_art_script(const std::string& name)
		{
			constexpr std::string_view prefix = "maps/createart/";
			constexpr std::string_view suffix = "_art.gsc";
			if (!name.starts_with(prefix) || !name.ends_with(suffix))
			{
				return {};
			}

			const auto map = name.substr(prefix.size(), name.size() - prefix.size() - suffix.size());
			return std::format("main()\n{{\n    level.tweakfile = 1;\n    maps\\createart\\{}_fog_hdr::setupfog();\n}}\n", map);
		}

		// the map script names its compass material, built from the donor zone's minimap materials
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
		const auto result = replace_removed_builtins(source);
		return name.starts_with("maps/createart/") && name.ends_with("_fog_hdr.gsc") ? convert_fog_file(result) : result;
	}

	void dump(ScriptFile* asset)
	{
		if (!asset->buffer || !asset->bytecode)
		{
			return;
		}

		// the zone's rawfile version has the original source (createfx text format etc) and is patched and dumped by rawfile
		const auto name = convert_name(asset->name);
		if (zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_RAWFILE, name).rawfile)
		{
			return;
		}

		std::string source;
		if (!make_s2_art_script(name)) // patch replaces art scripts whole
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

		// s2x compiles a "<name>.gsc" rawfile in place of the stock scriptfile, so shared h1 scripts must not replace the s2 ones
		if (std::filesystem::path(name).stem().string().starts_with(filesystem::get_fastfile()))
		{
			add_csv_row("rawfile", name);
		}
	}
}
