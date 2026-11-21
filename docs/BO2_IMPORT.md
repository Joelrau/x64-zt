# BO2 -> H1 map import (first pass, untested in game)

1. In the CodLegacy repo: `cargo run --release -p render_t7 --example bo2_h1_export -- <mp_map.ff> <out dir>` writes `<map>.h1map` and `<map>.ents`.
2. In ZoneTool (H1 module): `importmap <out dir>/<map>.h1map` dumps `maps/mp/<map>.d3dbsp.*` and placeholder `materials/wc/bo2_*.json` plus `maps/mp/<map>.materials.csv` (lines for zone_source).
3. Assemble like `iw7_port2.ps1`: borrow mp_dig placeholder lightmap/reflection images and techsets, write zone_source/<map>.csv (netconststrings, map_ents, rawfiles, materials list), `buildzone`, clone the load zone.

Stubbed: lightmaps, reflection probes, light grid, vis, sky. Textures are `$white` placeholders. Sun direction convention is unverified.
