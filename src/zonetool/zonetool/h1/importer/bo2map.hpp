#pragma once

// Black Ops II -> H1 map importer (FIRST PASS, untested in game).
//
// Reads the neutral map file the CodLegacy Rust tool writes (`bo2_h1_export`:
// `<map>.h1map` = the drawn world and placed models in one vertex stream, the
// materials with their kind, the sun; `<map>.ents` = the spawn and script
// entities H1 can run) and dumps the H1 assets ZoneTool's `buildzone` reads:
// gfx_map, col_map, com_map, map_ents, phys_worldmap, fx_map, glass_map and
// one placeholder material per Black Ops II material.
//
// Command: `importmap <file.h1map> [name]` (the name defaults to the file's).

#include <string>

namespace zonetool::h1::importer
{
	bool import_bo2_map(const std::string& h1map_path, const std::string& name_override);
}
