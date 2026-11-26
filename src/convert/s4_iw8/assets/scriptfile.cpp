#include <std_include.hpp>

#include "../include.hpp"
#include "scriptfile.hpp"

#include <iw8/assets/scriptfile.hpp>

#include <utils/compression.hpp>

#include <xsk/gsc/engine/s4.hpp>

namespace zonetool::s4::converter::iw8::scriptfile
{
	namespace
	{
		script_context& target_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<script_context>();
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
		}

		xsk::gsc::s4::context& source_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::s4::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
		}

		std::string decompile(const ScriptFile* asset)
		{
			const std::string compressed_stack{asset->buffer, static_cast<std::uint32_t>(asset->compressedLen)};
			const auto decompressed_stack = utils::compression::zlib::decompress(compressed_stack);

			const std::vector<std::uint8_t> stack{decompressed_stack.begin(), decompressed_stack.end()};
			const std::vector<std::uint8_t> bytecode{asset->bytecode, asset->bytecode + asset->bytecodeLen};

			auto& context = source_context();
			const auto assembly = context.disassembler().disassemble(bytecode, stack);
			const auto program = context.decompiler().decompile(*assembly);
			const auto source = context.printer().print(*program);
			return {source.begin(), source.end()};
		}

		std::size_t count_occurrences(const std::string_view source, const std::string_view text)
		{
			auto count = 0ull;
			for (auto pos = source.find(text); pos != std::string_view::npos; pos = source.find(text, pos + text.size()))
			{
				count++;
			}

			return count;
		}

		std::string get_map_script_name(const std::string& map)
		{
			return std::format("scripts/{}/maps/{}/{}.gsc", map.starts_with("cp_") ? "cp" : "mp", map, map); // GScr_MainMP_LoadScripts_shared (0xE94369)
		}

		std::string make_map_script(const std::string& map, const std::string& source)
		{
			static const std::regex game_value(R"re(game\[\s*"(attackers|defenders|allies_outfit|axis_outfit)"\s*\]\s*=\s*"([^"]*)"\s*;)re");

			std::string game_values;
			for (auto it = std::sregex_iterator(source.begin(), source.end(), game_value); it != std::sregex_iterator(); ++it)
			{
				game_values += std::format("    game[\"{}\"] = \"{}\";\n", (*it)[1].str(), (*it)[2].str());
			}

			return std::format(R"(main()
{{
    scripts\mp\load::main();
    level.outofboundstriggers = getentarray( "OutOfBounds", "targetname" );
    scripts\mp\compass::setupminimap( "compass_map_{}" );
{}}}
)", map, game_values);
		}

		void dump_map_script(const std::string& map, const std::string& source)
		{
			const auto name = get_map_script_name(map);
			const auto script_source = make_map_script(map, source);
			std::vector<std::uint8_t> data(script_source.begin(), script_source.end());

			auto& context = target_context();
			const auto assembly = context.compiler().compile(name, data);
			const auto [bytecode, stack, devmap] = context.assembler().assemble(*assembly);
			const auto buffer = utils::compression::zlib::compress(std::string(reinterpret_cast<const char*>(stack.data), stack.size));

			zonetool::iw8::ScriptFile script{};
			script.name = name.data();
			script.compressedLen = static_cast<int>(buffer.size());
			script.len = static_cast<int>(stack.size);
			script.bytecodeLen = static_cast<int>(bytecode.size);
			script.buffer = buffer.data();
			script.bytecode = reinterpret_cast<char*>(const_cast<std::uint8_t*>(bytecode.data));
			zonetool::iw8::scriptfile::dump(&script);
			add_csv_row("scriptfile", name);
		}
	}

	void dump(ScriptFile* asset)
	{
		try
		{
			const auto source = decompile(asset);

			filesystem::file file(asset->name);
			file.open("wb");
			file.write(source.data(), source.size());
			file.close();

			const auto& map = filesystem::get_fastfile();
			if (asset->name == get_map_script_name(map))
			{
				dump_map_script(map, source);
				return;
			}

			const auto unnamed_ids = count_occurrences(source, "_id_") + count_occurrences(source, "_func_") + count_occurrences(source, "_meth_");
			const auto hashed_strings = count_occurrences(source, "\"#x");
			if (unnamed_ids || hashed_strings)
			{
				ZONETOOL_WARNING("scriptfile \"%s\" is dumped as S4 source only: %zu ids have no name and %zu strings are hashes", asset->name, unnamed_ids, hashed_strings);
			}
		}
		catch (const std::exception& e)
		{
			ZONETOOL_WARNING("scriptfile \"%s\" is not converted: %s", asset->name, e.what());
		}
	}
}
