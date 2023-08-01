#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "computeshader.hpp"

#include "zonetool/h1/assets/computeshader.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace computeshader
		{
			zonetool::h1::ComputeShader* convert(ComputeShader* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::ComputeShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				new_asset->name = allocator.duplicate_string(game::add_source_postfix(asset->name, game::iw6));

				return new_asset;
			}

			void dump(ComputeShader* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::compute_shader::dump(converted_asset);
			}
		}
	}
}