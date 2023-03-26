#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "pixelshader.hpp"

#include "zonetool/h1/assets/pixelshader.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace pixelshader
		{
			zonetool::h1::MaterialPixelShader* convert(MaterialPixelShader* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::MaterialPixelShader>();

				std::memcpy(new_asset, asset, sizeof(MaterialPixelShader));
				new_asset->name = mem->StrDup(asset->name + TECHSET_PREFIX);

				return new_asset;
			}

			void dump(MaterialPixelShader* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IPixelShader::dump(converted_asset);
			}
		}
	}
}