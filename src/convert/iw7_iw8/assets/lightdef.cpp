#include <std_include.hpp>

#include "../include.hpp"
#include "lightdef.hpp"

#include <iw8/assets/lightdef.hpp>

namespace zonetool::iw7::converter::iw8::lightdef
{
	void dump(GfxLightDef* asset)
	{
		if (!mark_dumped(ASSET_TYPE_LIGHT_DEF, asset->name))
		{
			return;
		}

		zonetool::iw8::GfxLightDef converted{};
		converted.name = asset->name;
		converted.iesProfile = asset->iesProfile;
		converted.firstPixel = asset->unk0; // TODO: unk0 is not proven to be firstPixel
		converted.coordOffset = asset->coordOffset;
		converted.coordScale = asset->coordScale;

		zonetool::iw8::light_def::dump(&converted);
		add_csv_row("lightdef", asset->name);
	}
}
