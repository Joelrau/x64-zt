# x64 ZoneTool
A fastfile unlinker and linker for various x64 Call of Duty titles. 

- If you are interested in porting maps or assets from IW3/4/5, check <b>[Aurora's Map Porting IW3/4/5 -> H1](https://docs.auroramod.dev/map-porting-iw5)</b>
- If you are interested in porting maps or assets from newer Call of Duty games like H1, S1, H1, H2, or IW7, between each other, check <b>[Aurora's Map Porting (S1 <-> H1 <-> H2)](https://docs.auroramod.dev/map-porting-s1)</b>

## Supported Games
Two code lines live in this repo:

- **`main` (v1)**: hooks the running game exe. Has the Black Ops III dumper and our IW7 -> H1 map port work.
- **[`v2`](https://github.com/sero7k/ZoneTool/tree/v2)**: Joelrau's rewrite of x64-zt that reads fastfiles directly and never loads the game exe. Adds WWII (S2) and Modern Warfare 2019 (IW8). Black Ops III is not ported to it yet.

| Game | `main` (v1) | `v2` |
|---|:-:|:-:|
| **IW6** (*Call of Duty: Ghosts*) | yes | yes |
| **S1** (*Call of Duty: Advanced Warfare*) | yes | yes |
| **H1** (*Call of Duty: Modern Warfare Remastered*) | yes | yes |
| **H2** (*Call of Duty: Modern Warfare 2 Campaign Remastered*) | yes | yes |
| **IW7** (*Call of Duty: Infinite Warfare*) | yes, with our map-port converter | yes |
| **T7** (*Call of Duty: Black Ops 3*) | dumping only | no |
| **S2** (*Call of Duty: WWII*) | no | yes |
| **IW8** (*Call of Duty: Modern Warfare 2019, 1.20*) | no | yes |
| **T6** (*Call of Duty: Black Ops II*) | map import only (see below) | no |

## How to use
Check out the [Aurora Zonetool Basics](https://docs.auroramod.dev/zonetool-basics) for useful guides & information on how to port maps and use zonetool.

## Commands
* `loadzone <zone>`: Loads a zone
* `unloadzones`: Unloads zones
* `verifyzone <zone>`: Lists assets in a zone
* `dumpzone <zone>`: Dumps a zone
* `dumpzone <target game> <zone> <asset filter>`: Dumps a zone converting assets for a specific game
* `dumpasset <type> <name>`: Dumps a single assset
* `dumpmap <map>`: Dumps all required assets for a map
* `dumpmap <target game> <map> <asset filter> <skip common>`: Dumps and converts all required assets for a map
* `importmap <file.h1map> [name]`: Imports a Black Ops II map (exported by CodLegacy's `bo2_h1_export`) as H1 map assets (H1 module)

  ### Definitions
  * `asset filter`: A filter specifying all the asset types that should be dumped, if not specified or empty it will dump all asset types.
  Asset types are separated by **commas**, **`_`** indicates and empty filter.   
    * Example: `dumpzone h1 mp_clowntown3 sound,material,techset,rawfile`
    * Example: `dumpmap h1 mp_clowntown3 _ true`
  * `skip common`: Skips common zones when dumping a map, can be `true` or `false`.
  * `target game`: The game to convert the assets to.

## Asset Conversion Support

The tables below show which asset conversions are currently supported.

- **Rows** = Source game
- **Columns** = Target game

Legend:
- ✔️ Fully supported
- ⚠️ Partially supported (experimental)
- ❌ Not supported

### `main` (v1)

| **From ↓ / To →** | **IW6** | **S1** | **H1** | **H2** | **T7** | **IW7** |
|-------------------|:-------:|:------:|:------:|:------:|:------:|:-------:|
| **IW6** | ✔️ | ❌ | ✔️ | ✔️ | ❌ | ❌ |
| **S1**  | ❌ | ✔️ | ✔️ | ✔️ | ❌ | ❌ |
| **H1**  | ❌ | ⚠️ | ✔️ | ✔️ | ❌ | ⚠️ |
| **H2**  | ❌ | ❌ | ✔️ | ✔️ | ❌ | ❌ |
| **T7**  | ❌ | ❌ | ⚠️ | ❌ | ✔️ | ⚠️ |
| **IW7** | ❌ | ❌ | ⚠️ | ❌ | ❌ | ✔️ |
| **T6 (BO2)** | ❌ | ❌ | ⚠️ | ❌ | ❌ | ❌ |

- **IW7 -> H1** is our map port: world, collision, map entities, materials, models, reflection probes and effects (used for `mp_flip`). Lighting, sun shadows and mirror-glass reflections are still approximate.
- **T6 (BO2) -> H1** is `importmap <file.h1map> [name]`, a map-only import. It reads the neutral `.h1map` and `.ents` files that the CodLegacy `bo2_h1_export` tool writes from a BO2 zone. Geometry and spawns only, textures are placeholders, and it is not yet verified in game. See [docs/BO2_IMPORT.md](docs/BO2_IMPORT.md).

### `v2`

| **From ↓ / To →** | **IW6** | **S1** | **H1** | **H2** | **S2** | **IW7** | **IW8** |
|-------------------|:-------:|:------:|:------:|:------:|:------:|:-------:|:-------:|
| **IW6** | ✔️ | ❌ | ✔️ | ❌ | ❌ | ❌ | ❌ |
| **S1**  | ❌ | ✔️ | ✔️ | ✔️ | ⚠️ | ❌ | ❌ |
| **H1**  | ❌ | ❌ | ✔️ | ✔️ | ⚠️ | ⚠️ | ❌ |
| **H2**  | ❌ | ❌ | ✔️ | ✔️ | ❌ | ❌ | ❌ |
| **S2**  | ❌ | ❌ | ⚠️ | ❌ | ✔️ | ❌ | ❌ |
| **IW7** | ❌ | ❌ | ⚠️ | ❌ | ❌ | ✔️ | ⚠️ |
| **IW8** | ❌ | ❌ | ❌ | ❌ | ❌ | ⚠️ | ✔️ |

- The v2 rows are taken from the upstream x64-zt `v2` README. The IW7 <-> IW8 cells are our reading of the converters in `src/convert/iw7_iw8` and `iw8_iw7`, which upstream does not list.
- **IW6 -> H2** and **H1 -> S1** are not offered on v2. Use `main` for IW6 -> H2.
- Our IW7 -> H1 map port and the BO2 importer have not been moved to `v2` yet.
