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

## Usage

```bash
zonetool.exe -game h1 dumpzone mp_shipment
```

Run `zonetool.exe` without a command to open the interactive console.

| Command | Result |
|---|---|
| `inspect <zone>` / `inspect --all` | Show fastfile header & container data |
| `verifyzone <zone>` / `verifyzone --all` | Reads every asset and checks that the zone is consumed exactly |
| `dumpzone <zone> [--target <game>]` | Writes a fastfile dump to `dump/<zone>/`. `--target` converts assets |
| `dumpasset <type> <name> <zone> [--target <game>]` | Writes one asset of `<zone>` to `dump/assets/`. `--target` converts it |
| `buildzone <zone>` | Reads `zone_source/<zone>.csv` and `zonetool/<zone>/`, then writes `<zone>.ff` to the game `zone/` folder |
| `generatecsv <map> [sp] [--path <dir>]` | Generates a build CSV for a map |

### Flags

| Flag | Effect |
|---|---|
| `-game <name>` | Selects the game |
| `-path <folder>` | Game folder to use |
| `-target_path <folder>` | Sets the folder of the `--target` game |
| `-dds` | Writes images as DDS as well |
| `-dump_streamed_image` | Reads `imagefile*.pak` and writes streamed images |
| `-no_cache` | Ignores the build cache and writes every output again |

If a flag is not set, x64-zt can read `zonetool.json` next to the executable:

```json
{ "game": "h1", "paths": { "h1": "D:/Games/MWR", "s2": "D:/Games/WWII" } }
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

