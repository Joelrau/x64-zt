#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "hullshader.hpp"

#include "zonetool/h1/assets/hullshader.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace hullshader
		{
			zonetool::h1::MaterialHullShader* convert(MaterialHullShader* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::MaterialHullShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				new_asset->name = allocator.duplicate_string(game::add_source_postfix(asset->name, game::iw6));

				return new_asset;
			}

			void dump(MaterialHullShader* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::hull_shader::dump(converted_asset);
			}
		}
	}
}