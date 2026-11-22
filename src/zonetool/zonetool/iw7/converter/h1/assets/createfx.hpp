#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace createfx
		{
			// v102 fx: decompiles IW7 fx/createfx scripts (scripts/mp/maps/<map>/[gen/]<map>_fx.gsc) and collects
			//          level._effect[] = loadfx() and createfx entities (origin/angles/fxid/delay)
			void dump(ScriptFile* asset);

			// v102 fx: IW7 particle systems (ASSET_TYPE_VFX) are not converted; remembered so the gsc can say why an effect is missing
			void note_vfx(const char* name);

			// v104 vfx: true when a parsed createfx entity places this effect (path) - ambient map effects loop forever
			bool is_ambient(const std::string& effect_path);

			// v102 fx: writes maps/mp/<map>_fx.gsc, maps/createfx/<map>_fx.gsc and <map>_fx_zone.csv (lines for iw7_port2.ps1)
			void finalize(const std::string& map);
		}
	}
}
