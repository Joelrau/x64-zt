#include <std_include.hpp>
#include "zonetool/h1/converter/s1/include.hpp"
#include "scriptfile.hpp"

//#include "zonetool/s1/assets/scriptfile.hpp"

#include "zonetool/utils/gsc.hpp"

#include <utils/string.hpp>
#include <utils/compression.hpp>

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace scriptfile
		{
			zonetool::s1::ScriptFile* convert(ScriptFile* asset, utils::memory::allocator& allocator)
			{
				return nullptr;
			}

			std::string convert_name(std::string gsc_name)
			{
				if (utils::string::is_numeric(gsc_name))
				{
					const auto name = gsc::h1::gsc_ctx->token_name(std::atoi(gsc_name.data()));
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
				auto& decompiler = gsc::h1::gsc_ctx->decompiler();
				auto& disassembler = gsc::h1::gsc_ctx->disassembler();

				const std::string stack_compressed{asset->buffer, static_cast<std::uint32_t>(asset->compressedLen)};
				const auto decompressed_stack = utils::compression::zlib::decompress(stack_compressed);

				const std::vector<std::uint8_t> stack{decompressed_stack.begin(), decompressed_stack.end()};
				const std::vector<std::uint8_t> bytecode{asset->bytecode, asset->bytecode + asset->bytecodeLen};

				const auto disasm = disassembler.disassemble(bytecode, stack);
				const auto decomp = decompiler.decompile(*disasm);

				const auto decomp_data = gsc::h1::gsc_ctx->source().dump(*decomp);
				return {decomp_data.begin(), decomp_data.end()};
			}

			void dump_as_gsc(ScriptFile* asset)
			{
				const auto decompiled_data = decompile_script(asset);

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
