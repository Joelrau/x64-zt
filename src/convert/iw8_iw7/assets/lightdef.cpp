#include <std_include.hpp>

#include "../include.hpp"
#include "lightdef.hpp"

#include <iw7/assets/gfxlightdef.hpp>

namespace zonetool::iw8::converter::iw7::lightdef
{
	void dump(GfxLightDef* asset)
	{
		zonetool::iw7::GfxLightDef light{};
		light.name = asset->name; // TODO: IES profile not converted, stock IW7 lightdefs have none
		zonetool::iw7::gfx_light_def::dump(&light);
		add_csv_row("lightdef", asset->name);
	}
}
