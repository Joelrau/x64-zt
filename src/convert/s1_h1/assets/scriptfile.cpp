#include <std_include.hpp>
#include "../include.hpp"
#include "scriptfile.hpp"

//#include <h1/assets/scriptfile.hpp>

#include <xsk/gsc/engine/s1_pc.hpp>

#include <utils/string.hpp>
#include <utils/compression.hpp>

namespace zonetool::s1
{
	namespace converter::h1
	{
		xsk::gsc::s1_pc::context& gsc_context()
		{
			static xsk::gsc::s1_pc::context context{xsk::gsc::instance::server};
			return context;
		}

		namespace scriptfile
		{
			zonetool::h1::ScriptFile* convert(ScriptFile* asset, utils::memory::allocator& allocator)
			{
				return nullptr;
			}

			std::string convert_name(std::string gsc_name)
			{
				if (utils::string::is_numeric(gsc_name))
				{
					const auto name = gsc_context().token_name(std::atoi(gsc_name.data()));
					if (!name.starts_with("_id_"))
					{
						gsc_name = name;
					}
				}
				gsc_name.append(".gsc");

				return gsc_name;
			}

			std::string remove_s1_dev_blocks(std::string source)
			{
				for (auto start = source.find("/$"); start != std::string::npos; start = source.find("/$", start))
				{
					const auto end = source.find("$/", start + 2);
					source.erase(start, end == std::string::npos ? std::string::npos : end + 2 - start);
				}

				return source;
			}

			void dump_source(RawFile* asset)
			{
				// the client parses the createart fog source for "ent = maps\_utility::create_vision_set_fog(" (CG_ParseClientVisionsetTriggers)
				std::string source{asset->buffer, static_cast<std::uint32_t>(asset->compressedLen > 0 ? asset->compressedLen : asset->len)};
				if (asset->compressedLen > 0)
				{
					source = utils::compression::zlib::decompress(source);
				}

				source = remove_s1_dev_blocks(source);

				filesystem::file file(asset->name);
				file.open("wb");
				file.write(source.data(), source.size());
				file.close();
			}

			std::string decompile_script(ScriptFile* asset)
			{
				auto& decompiler = gsc_context().decompiler();
				auto& disassembler = gsc_context().disassembler();

				const std::string stack_compressed{asset->buffer, static_cast<std::uint32_t>(asset->compressedLen)};
				const auto decompressed_stack = utils::compression::zlib::decompress(stack_compressed);

				const std::vector<std::uint8_t> stack{decompressed_stack.begin(), decompressed_stack.end()};
				const std::vector<std::uint8_t> bytecode{asset->bytecode, asset->bytecode + asset->bytecodeLen};

				const auto disasm = disassembler.disassemble(bytecode, stack);
				const auto decomp = decompiler.decompile(*disasm);

				const auto decomp_data = gsc_context().printer().print(*decomp);
				return {decomp_data.begin(), decomp_data.end()};
			}

			void dump_as_gsc(ScriptFile* asset)
			{
				const auto name = convert_name(asset->name);
				const auto map = filesystem::get_fastfile();
				auto decompiled_data = decompile_script(asset);
				if (name == std::format("maps/mp/{}.gsc", map))
				{
					decompiled_data = make_map_setup_script(map, decompiled_data, {.ambient_play = "ambientplay", .keep_dvars = true});
				}

				filesystem::file file(name);
				file.open("wb");
				file.write(decompiled_data.data(), decompiled_data.size());
				file.close();
			}

			void dump(ScriptFile* asset)
			{
				// the rawfile of the same name has the original source, dump_source writes it
				if (db_find_x_asset_header_safe(ASSET_TYPE_RAWFILE, convert_name(asset->name)).data)
				{
					return;
				}

				dump_as_gsc(asset);
			}
		}
	}
}
