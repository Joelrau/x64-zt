# x64 ZoneTool

Standalone fastfile unlinker and linker for next-generation Call of Duty titles. You can dump `.ff` fastfile zones to disk, build new zones from those dumps, and convert assets between games.

## Supported games

| Game | Title | Notes |
|---|---|---|
| **IW6** | Ghosts | |
| **S1** | Advanced Warfare | |
| **H1** | Modern Warfare Remastered | |
| **H2** | Modern Warfare 2 Campaign Remastered | Battle.net CASC |
| **S2** | WWII | Signed zones. The key comes from `s2_zone_key.der` or the game executable |
| **IW7** | Infinite Warfare | |

T7 was in v1 (`main`). It is not ported to v2 yet.

## Asset conversion

Rows are the source game. Columns are the target game.

| From ↓ / To → | IW6 | S1 | H1 | H2 | S2 | IW7 |
|---|:-:|:-:|:-:|:-:|:-:|:-:|
| **IW6** | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ |
| **S1**  | ❌ | ✅ | ✅ | ✅ | ⚠️ | ❌ |
| **H1**  | ❌ | ❌ | ✅ | ✅ | ⚠️ | ⚠️ |
| **H2**  | ❌ | ❌ | ✅ | ✅ | ❌ | ❌ |
| **S2**  | ❌ | ❌ | ⚠️ | ❌ | ✅ | ❌ |
| **IW7** | ❌ | ❌ | ⚠️ | ❌ | ❌ | ✅ |

✅ supported &nbsp; ⚠️ partial or experimental &nbsp; ❌ not supported

- **IW6 → H1** uses IW6 techsets instead of remapping
- **S1 → H2** converts through H1
- **H1 → S1** has been purposely left out for now
- **IW7 → H1** converts images, materials and models only
- **H1 → S2** and **S1 → S2** are experiments done for map assets (world, collision, map ents), models, materials and images. effects, sounds, animations, scriptables and weapons are not converted yet

<details>
<summary>v1 legend (<code>main</code>), for comparison</summary>

| From ↓ / To → | IW6 | S1 | H1 | H2 | T7 | IW7 |
|---|:-:|:-:|:-:|:-:|:-:|:-:|
| **IW6** | ✔️ | ❌ | ✔️ | ✔️ | ❌ | ❌ |
| **S1**  | ❌ | ✔️ | ✔️ | ✔️ | ❌ | ❌ |
| **H1**  | ❌ | ⚠️ | ✔️ | ✔️ | ❌ | ⚠️ |
| **H2**  | ❌ | ❌ | ✔️ | ✔️ | ❌ | ❌ |
| **T7**  | ❌ | ❌ | ⚠️ | ❌ | ✔️ | ⚠️ |
| **IW7** | ❌ | ❌ | ❌ | ❌ | ❌ | ✔️ |

</details>

## Quick start

1. Put `zonetool.exe` in the game folder, next to the game exe and the `zone/` folder. x64-zt finds the game from the game exe.
2. Dump a zone:

   ```bash
   zonetool.exe dumpzone mp_shipment
   ```

   The assets and `mp_shipment.csv` go to `dump/mp_shipment/` in the current folder.
3. Build a zone. Put the asset files in `zonetool/<name>/` and the asset list in `zone_source/<name>.csv`. A dump CSV is a good start. Then run:

   ```bash
   zonetool.exe buildzone <name>
   ```

   The result is `zone/<name>.ff` in the game folder. It replaces a zone of the same name, so use a new name if you want to keep the stock zone.

To use x64-zt from another folder, give the game and the game folder:

```bash
zonetool.exe -game h1 -path "C:\Games\MWR" dumpzone mp_shipment
```

Run `zonetool.exe -h` to see all commands and options. Run `zonetool.exe` without a command to open the interactive console.

### Common errors

| Message | Fix |
|---|---|
| `game "x" is not in this build` | The `-game` value is not a game of this build. Use a name from `zonetool.exe -h`, for example `h1` or `s2` |
| `zone "x" not found in ...` | The zone is not in the game folder. Check `-path` and the zone name. The `.ff` extension is optional |
| `set the <game> game folder with -path or zonetool.json` | x64-zt did not find the game folder. Use `-path` or `zonetool.json` |
| Wrong game | The game exe in the folder selects the game. Set `-game` to use a different game |
| An option has no effect in the console | Options work only on the `zonetool.exe` command line. Put them before the command |

## Usage

| Command | Result |
|---|---|
| `inspect <zone>` / `inspect --all` | Shows the fastfile header and container data |
| `verifyzone <zone>` / `verifyzone --all` | Reads every asset and checks that the zone is consumed exactly |
| `dumpzone <zone> [--target <game>]` | Writes a fastfile dump to `dump/<zone>/`. `--target` converts assets |
| `dumpmap <map> [--target <game>] [--skip-common]` | Writes the map zones to `dump/<map>/` with one merged CSV. H1, H2 and S1 only |
| `dumpasset <type> <name\|*> <zone> [--target <game>]` | Writes one asset of `<zone>` to `dump/assets/`. `*` writes all assets of the type. `--target` converts it |
| `listzone <zone>` / `listzone --all` | Writes the asset CSV of each zone to `asset_listing/<game>/`. It dumps no assets |
| `buildzone <zone>` | Reads `zone_source/<zone>.csv` and `zonetool/<zone>/`, then writes `<zone>.ff` to the game `zone/` folder |
| `generatecsv <map> [sp] [--path <dir>]` | Writes `zone_source/<map>.csv` from the files in `zonetool/<map>/` |

### Options

Put options before the command, on the `zonetool.exe` command line.

| Option | Effect |
|---|---|
| `-game <name>` | Selects the game: `h1`, `h2`, `iw6`, `iw7`, `s1` or `s2`, if the build includes it |
| `-path <folder>` | Game folder to use, the folder that holds `zone/` |
| `-target_path <folder>` | Sets the folder of the `--target` game |
| `-no_cache` | Ignores the build cache and writes every output again |
| `-dds` | Dump: writes images as DDS as well |
| `-dump_streamed_image` | Dump: reads `imagefile*.pak` and writes streamed images |
| `-castdump` | Dump: writes a `.cast` file next to each xmodel and xanim. IW7 model geometry is in the `*_tr` zones |
| `-more_memory` | Build: doubles the zone memory limit |
| `-keep_mp_traverse_nodes` | Build: keeps invalid MP traverse nodes in aipaths. H2 only |

### Game selection

Without `-game`, x64-zt selects the game in this order:

1. The game exe in the `-path` folder, or in the folder of `zonetool.exe`.
2. `game` in `zonetool.json`.
3. The first game of the build.

| Game exe | Game |
|---|---|
| `h1_mp64_ship.exe`, `h1_sp64_ship.exe` | `h1` |
| `MW2CR.exe`, `h2_sp64_bnet_ship.exe` | `h2` |
| `s1_mp64_ship.exe`, `s1_sp64_ship.exe` | `s1` |
| `s2_mp64_ship.exe` | `s2` |
| `iw6mp64_ship.exe`, `iw6sp64_ship.exe` | `iw6` |
| `iw7_ship.exe` | `iw7` |

x64-zt reads `zonetool.json` next to the executable for the values that are not set on the command line:

```json
{ "game": "h1", "paths": { "h1": "path/to/mwr", "s2": "path/to/wwii" } }
```

## Building

Requires Windows and Visual Studio 2022.

```bash
generate.bat --games=h1,iw7
```

| Option | Effect |
|---|---|
| `--games=<list>` | Games to build: `all` (default), or a list such as `h1,s2` |
| `--no-convert` | Skips the converter projects |
| `--copy-to=<folder>` | Copies `zonetool.exe` to this folder after each build |

The converters for games only builds only when its source and target games are selected.

## Guides

x64-zt is a work in progress software that aims to document the modding experience as best as possible. Some things may not be answered well, but checking out the Zonetool Basics will help you get started.

- [Zonetool Basics](https://docs.auroramod.dev/zonetool-basics)
- [Map porting IW3/4/5 → H1](https://docs.auroramod.dev/map-porting-iw5)
- [Map porting S1 ↔ H1 ↔ H2](https://docs.auroramod.dev/map-porting-s1)

