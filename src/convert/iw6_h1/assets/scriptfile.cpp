#include <std_include.hpp>
#include "../include.hpp"
#include "scriptfile.hpp"

//#include <h1/assets/scriptfile.hpp>

#include <xsk/gsc/engine/iw6_pc.hpp>

#include <utils/string.hpp>
#include <utils/compression.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		xsk::gsc::iw6_pc::context& gsc_context()
		{
			static xsk::gsc::iw6_pc::context context{xsk::gsc::instance::server};
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

			std::string decompile_script(ScriptFile* asset)
			{
				auto& decompiler = gsc_context().decompiler();
				auto& disassembler = gsc_context().disassembler();

				const std::string stack_compressed{ asset->buffer, static_cast<std::uint32_t>(asset->compressedLen) };
				const auto decompressed_stack = utils::compression::zlib::decompress(stack_compressed);

				const std::vector<std::uint8_t> stack{ decompressed_stack.begin(), decompressed_stack.end() };
				const std::vector<std::uint8_t> bytecode{ asset->bytecode, asset->bytecode + asset->bytecodeLen };

				const auto disasm = disassembler.disassemble(bytecode, stack);
				const auto decomp = decompiler.decompile(*disasm);

				const auto decomp_data = gsc_context().printer().print(*decomp);
				return {decomp_data.begin(), decomp_data.end()};
			}

			void fixup_script(std::string& data)
			{
				data = std::regex_replace(data, std::regex("common_scripts\\\\utility::waitframe"), "waitframe");
				data = std::regex_replace(data, std::regex("common_scripts\\\\utility::exploder"), "common_scripts\\_exploder::exploder");
			}

			void dump_as_gsc(ScriptFile* asset)
			{
				auto decompiled_data = decompile_script(asset);

				fixup_script(decompiled_data);

				filesystem::file file(convert_name(asset->name));
				file.open("wb");
				file.write(decompiled_data.data(), decompiled_data.size());
				file.close();
			}

			void dump(ScriptFile* asset)
			{
				dump_as_gsc(asset);
			}
		}
	}
}