#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "vertexdecl.hpp"

#include "zonetool/h1/assets/vertexdecl.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace vertexdecl
		{
			zonetool::h1::MaterialVertexDeclaration* convert(MaterialVertexDeclaration* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::MaterialVertexDeclaration>();

				new_asset->streamCount = asset->streamCount;
				new_asset->hasOptionalSource = asset->hasOptionalSource;

				for (auto i = 0; i < 32; i++)
				{
					new_asset->routing.data[i].source = asset->routing.data[i].source;
					new_asset->routing.data[i].dest = asset->routing.data[i].dest;
					new_asset->routing.data[i].mask = asset->routing.data[i].mask;

					if (asset->routing.data[i].source > 7)
					{
						new_asset->routing.data[i].source += 2;
					}
				}

				new_asset->name = allocator.duplicate_string(game::add_source_postfix(asset->name, game::s1));

				return new_asset;
			}

			void dump(MaterialVertexDeclaration* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::vertex_decl::dump(converted_asset);
			}
		}
	}
}
