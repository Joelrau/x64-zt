#include <std_include.hpp>
#include "zonetool/h1/converter/s1/include.hpp"
#include "weapondef.hpp"

#include "zonetool/s1/assets/weapondef.hpp"

#define COPY_VALUE(value) new_asset->value = asset->value;
#define COPY_VALUE_(value, type) new_asset->value = reinterpret_cast<type>(asset->value);
#define COPY_VALUE_MEM(value, type) memcpy(new_asset->value, asset->value, sizeof(type));

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace weapondef
		{
			zonetool::s1::WeaponDef* convert(WeaponDef* asset, utils::memory::allocator& allocator)
			{
				auto new_asset = allocator.allocate<zonetool::s1::WeaponDef>();

				// copy first 40 bytes
				memcpy(new_asset, asset, 40);
				// reticleViewModels
				COPY_VALUE(szModeName)
				new_asset->camoWorldModelMaterialOverride = nullptr;
				new_asset->camoViewModelMaterialOverride = nullptr;
				COPY_VALUE_MEM(szXAnimsRightHanded, zonetool::s1::XAnimParts**)
				COPY_VALUE_MEM(szXAnimsLeftHanded, zonetool::s1::XAnimParts**)
				COPY_VALUE_(hideTags, zonetool::s1::scr_string_t*)
				COPY_VALUE_MEM(szXAnimsLeftHanded, zonetool::s1::XAnimParts**)

				return new_asset;
			}

			void dump(WeaponDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::s1::weapon_def::dump(converted_asset);
			}
		}
	}
}
