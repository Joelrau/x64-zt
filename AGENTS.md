# AGENTS.md

Guide for humans and agents who work on x64-zt. Read it before you change code.

It applies to every agent and model. "Ground rules" apply to every task. The sections after them are reference. "Agent playbook" has the procedures for long work.

## What x64-zt is

x64-zt is a fastfile unlinker and linker for x64 Call of Duty titles. It reads the `.ff` zones of a game and dumps their assets to disk. It builds new zones from those dumps, also with new assets. It converts assets from one game to another as well.

The `v2` branch is a standalone rewrite of x64-zt with the assistance of AI to help add new games and keep a cleaner codebase. v1 (`main`) loaded the game binary and hooked its zone loader. v2 reads fastfiles directly and never loads a game exe. v2 keeps the v1 asset logic and the v1 dump formats.

| Name | Game | Doc |
|---|---|---|
| `h1` | Modern Warfare Remastered 1.15 | `docs/h1-115-format.md` |
| `h2` | Modern Warfare 2 Campaign Remastered | `docs/H2_sync.md` |
| `iw6` | Ghosts | `docs/IW6_SUPPORT.md` |
| `iw7` | Infinite Warfare | |
| `iw8` | Modern Warfare 2019 1.20 | `docs/IW8_SUPPORT.md`, `docs/IW8_IN_IW7_FOCUS.md` |
| `s1` | Advanced Warfare | `docs/S1_SUPPORT.md` |
| `s2` | WWII | `docs/S2_SUPPORT.md`, `docs/IMPROVE_H1_S2.md` |

Other games are not supported yet. They need to be re-added.

## Ground rules

These rules apply to every task. They apply to agents more than to humans, but they are good practice for humans too. Later sections give the detail.

Code:

1. The asm in `re_reference/` is the source of truth. When the code and the asm disagree, the asm decides. Do not guess a layout, an offset or a game behavior. Prove it with the asm.
2. `read()` and `write()` of an asset match step for step, and both match the game `Load_*` code. Change both in the same edit. See "Zone format rules".
3. Keep working v1 logic and the v1 dump formats. Use zero donor assets in a converter. See "Compatibility rules".
4. Do not write hacky or sloppy shortcut code. Fix the cause, not the symptom.
5. Keep code readable for humans. x64-zt was a human project before AI, and code that only a machine can follow is not accepted. Use named struct fields, not raw offsets and byte casts. Comments do not fix sloppy code.
6. Put engine structs in the game `structs.hpp`. Do not declare structs inline in an asset file or a converter file.
7. Write almost no comments. Most comments in this repository are noise. See "Comments".

Work:

8. Never open a `ship_asm.txt` file with a file tool, `Get-Content` or `ReadAllLines`. The files are 85 MB to 330 MB. Use `tools/asm_slice.py`, or `rg` with a hit limit.
9. h1-mod runs the H1 1.4 binary. For an address from an h1-mod crash, use the `h1_1.4` dump, not `h1` (1.15).
10. Copy the new `zonetool.exe` to the game folder before each dump or build. For H1, this is the MWR 1.15 folder. A dump with an old exe wastes up to one hour.
11. Keep the same `--games` set for `generate.bat`. A different set regenerates the projects and rebuilds everything, which takes more than 10 minutes.
12. Do implementation work in the main session. Use a subagent only for research, and give it rule 8.
13. Put temporary files in a session scratchpad or a temporary folder, not in the repository or `/tmp`.
14. `core.autocrlf` is `true`. Some files are CRLF in the working tree. Keep the line endings of the file you edit.
15. Do not report a result that you did not check. List each check that you did not run as "Not verified".
16. Do not guess. If you do not know how to do a step in this file, read the tool help first. Then ask the user one specific question.

Safety and privacy:

17. Commit only when the user asks. Do not add yourself as author or co-author of a commit. The author is the user.
18. Never close a game or tool process that you did not start. You can close a crashed process that you started, but read its memory first (see "A game crash").
19. Keep machine paths, user names and drive letters out of the repository.
20. Take screenshots of the game window only, not of the desktop or other windows.

## Before you start

- `docs/`, `re_reference/`, `dump/` and `build/` are local folders that git ignores. A fresh clone or a git worktree does not have them. Read `docs/` and `re_reference/` in the main checkout, which `git worktree list` prints first. If they are not there, ask the user.
- Read the doc of a game before you work on that game. A doc can be long. Read its headings first, then the sections for your task and the newest log entries.
- This file does not list the game folders. If you do not know the folder of a game, ask the user. Do not search a drive for it.
- Not every harness has every tool that this file names: file tools, background jobs, subagents, screenshots. If you cannot do a step, tell the user. Do not skip it silently.

## Build

This was only tested for Windows using Visual Studio 2022 via MSBuild and premake.

- `generate.bat --games=<games>` writes the projects to `build/`. `<games>` is `all` (default), one game (`h1`) or a list (`h1,iw7`).
- `tools\build.bat <games>` runs `generate.bat` and then MSBuild (Release, x64). cmd splits a batch argument at `,` and `=`, so use `+` between games: `tools\build.bat h1+iw7`.
- After a source change, MSBuild alone is enough:

```powershell
$msbuild = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe
& $msbuild build\x64-zt.sln -p:Configuration=Release -p:Platform=x64 -m -v:minimal -nologo
```

- The result is `build/bin/x64/Release/zonetool.exe`.
- After you add or remove a source file, run `generate.bat` again with the same `--games` set.
- premake writes `build/generated/enabled_games.hpp` with `ZONETOOL_GAMES(x)` and `ZONETOOL_CONVERTERS(x)`. It shows the current `--games` set. Do not edit it.
- `--no-convert` skips the converter projects if you are not doing any conversions. A converter project `convert-<a>-<b>` builds only when both games are selected. `s1_h2` and `s1_s2` also need `h1`. Each game is a separate static library, so a change in one game does not rebuild the other.
- `--copy-to=<folder>` copies `zonetool.exe` to that folder after each build.

## Run

```bash
zonetool.exe -game h1 dumpzone mp_shipment
```

| Command | Result |
|---|---|
| `inspect <zone>`, `inspect --all` | Quick inspect a fastfile container and its data |
| `verifyzone <zone>`, `verifyzone --all` | Reads every asset and checks that the reader consumed the zone exactly |
| `listzone <zone>`, `listzone --all` | Writes the asset CSV of each zone to `asset_listing/<game>/<zone>.zone`. It dumps no assets |
| `dumpzone <zone> [--target <game>]` | Writes `dump/<zone>/` and `dump/<zone>/<zone>.csv`. `--target` converts the assets |
| `dumpmap <map> [--target <game>] [--skip-common]` | Writes the map zones of `<map>` to `dump/<map>/` with one merged `<map>.csv`. It dumps only techsets from common zones (H2 `common`: images, IW6: every common asset that the map zones reference). `--skip-common` loads the common zones but does not dump them. Patch zones load after the map zone. It dumps only the last copy of an asset, because the game uses the patch copy. H1, H2, IW6 and S1 only |
| `dumpasset <type> <name> <zone> [--target <game>]` | Reads `<zone>` and writes one asset to `dump/assets/`. The name `*` writes every asset of the type. `--target` converts the asset |
| `buildzone <zone> [-use_usermaps]` | Reads `zone_source/<zone>.csv` and the files in `zonetool/<zone>/`, then writes `<game folder>/zone/<zone>.ff`. `-use_usermaps` writes the zone to `<game folder>/usermaps/<zone>/` instead |
| `generatecsv <map> [sp] [--path <dir>]` | Writes `zone_source/<map>.csv` from the files in `zonetool/<map>/` |

Without a command, `zonetool.exe` opens an interactive console that waits for input. An agent always passes a command. `zonetool.exe -h` prints every command and flag.

- `dump/`, `zonetool/`, `zonetool_paths/`, `zone_source/` and `asset_listing/` are relative to the working directory. The usual working directory is the game folder.
- A `-` flag can go anywhere on the command line. Put a `--` option where the table shows it.
- Game selection: `-game <name>`, then the game exe in the `-path` folder, else in the exe folder or the current folder (`game::binaries`), then `zonetool.json` `game`.
- Game folder: `-path <folder>`, then `zonetool.json` `paths.<game>`, then the exe folder or the current folder if it has the game exe or `zone/`, then the Steam default folder if it exists.
- `-target_path <folder>` sets the game folder of the `--target` game. Without it: `zonetool.json` `paths.<game>`, then the Steam default folder.
- IW8 has no `zone/` folder. x64-zt reads its fastfiles from the CASC storage in `Data/`, applies the `.fp` patch of each zone, and loads `oo2core_*_win64.dll` from the game folder for Oodle. Streamed images come from the `pak_*.xpak` files in CASC. If the game folder has no `Data/` but has `zone/`, x64-zt reads the loose fastfiles in `zone/`, for example a 1.41 install.
- `zonetool.json` is next to `zonetool.exe`. Example: `{"game": "h1", "paths": {"h1": "path/to/mwr"}}`.
- `dumpasset` finds only assets that the zone holds, not referenced ones (`type,,name` rows in the CSV). Type names are the CSV type names. IW7 also searches the converter required zones, so `dumpasset weapon <name> <view zone> --target iw8` converts the weapon with its view model loaded.
- With `--target`, a converter loads its `required_zones` (h1_s2: `common_core_mp`, `common_mp` and others) and can also dump common-zone assets that a converted asset uses (h1_s2 `dump_used`: images, xanims, sounds with their loaded sounds). The S2 `buildzone` skips the ones S2 already ships through the `ignore` rows.
- H1 to S2 output can need S2x: a map zone cannot replace a rawfile that S2 `common_mp` already holds, so h1_s2 writes such files as `override/<name>` and S2x loads `override/<name>` first. This covers `vision/<map>.vision` when `common_mp` has the same name, and `override/animtrees/destructibles.atr` (the stock tree plus the H1 scriptable anims it lacks, since S2 looks scriptable anims up in the hardcoded `destructibles` tree). h1_s2 reads `<S2 folder>/zone/common_mp.ff` to find them. A hand-filtered `zone_source/<map>.csv` needs the `rawfile,override/...` rows.
- Flags for `dumpzone`: `-dds` writes DDS images. `-dump_streamed_image` reads `imagefile*.pak` and writes streamed images. `-castdump` writes a `.cast` file next to each xmodel and xanim; IW6, S1, H1, H2 and IW7 share `src/core/assets/cast_dump.hpp`. See `docs/S2_SUPPORT.md`.
- Flag for `dumpzone --target iw7` of IW8: `-lambert` converts materials to the stock lambert techsets. Without it, the converter uses the stock PBR techsets.
- Flags for `dumpzone --target iw8` of IW7: `-max_image_size <n>` limits the longest side of a converted image to `n` pixels. `-no_gpu` compresses BC6H and BC7 images on the CPU. Without it, the converter uses a D3D11 GPU and falls back to the CPU.
- Flags for `buildzone`: `-more_memory` doubles the zone memory limit. `-keep_mp_traverse_nodes` keeps invalid mp traverse nodes in aipaths (H2). `-uncompressed` writes a stored IW8 zone (`\x01IWC` body), which `mw120rproxy` needs. `-copy_to <folder>` copies the built `.ff` and `.pak` to that folder, for example a mod package folder.
- The `require` row in a build CSV loads another zone into the asset database. There is no `loadzone` command.
- `buildzone` writes `<zone>.ff` and `<zone>.pak` to the `zone/` folder of the game folder. It replaces a zone of the same name.
- `-no_cache` writes every output again and ignores the build cache.
- `-threads <n>` limits the worker threads of the zone readers, the image converters and the zlib compressor to `n`. The default is all cores.

## Build cache

The cache makes a second `dumpzone` or `buildzone` of the same zone faster. It does not change the asset files, so v1 and tools without the cache read the output.

- `dump/<zone>/.zonetool_cache` records a fingerprint and a file stamp (size and write time) for each cached dump file. If both match, the dumper skips the file.
- `%LOCALAPPDATA%/zonetool/<game>/<zone>.pak.streams` records the stream offsets of the last `.pak`. If the streamed image files and the `.pak` did not change, `buildzone` reuses the `.pak`.
- `%LOCALAPPDATA%/zonetool/s2/h1_techsets.txt` holds the H1 techset catalog of the S2 to H1 converter.
- Zlib zones of 8 MB or more compress in 4 MB chunks on all threads. The result is one valid zlib stream.
- The revision values are in `cache_revision` in `src/core/io/output_cache.hpp`. If you change the output of a cached asset, increment its revision.

## Source layout

```
src/common/            utils: io, string, compression, crypto, flags
src/core/xfile/        container: header, stream file table, signed blocks, zlib, LZ4, Oodle
src/core/zone/         zone_reader, zone_buffer, asset database, script strings
src/core/formats/      csv, iwi, imagefile, mapents, ddl, cast, havok tagfile, vcdiff
src/core/assets/       asset templates that more than one game uses
src/core/io/           filesystem, game files (folder or CASC), assetmanager (binary dump format), output cache
src/games/<game>/      game.cpp, structs.hpp, <game>.hpp, assets/, zone/, dump.cpp, build.cpp
src/convert/<a>_<b>/   converter from game <a> to game <b>
src/cli/               main, commands, settings, game registry, crash handler
deps/                  submodules; deps/premake/*.lua defines each dependency project
tools/                 build.bat, premake5.exe, asm_slice.py, asm_pack.py, livemem.py, game_window.ps1, s2_hub_to_h1.ps1
re_reference/<game>/   IDA text dumps (local folder)
docs/                  research notes and work logs (local folder)
```

Per game:

- `<game>.hpp` is the game header. It holds the `<GAME>_ASSETS(x)` x-macro (type, asset class, struct), includes every asset header, and declares the database lookups.
- `game.cpp` returns the `game` definition: name, container format, and the dump, verify and build entry points.
- `zone/loader.cpp` builds the reader table from `<GAME>_ASSETS`. `dump.cpp` builds the dump switch from the same macro.
- `zone/zone.cpp` holds the `ADD_ASSET` list that `buildzone` uses.

Dependency rules:

- No include from one game into another inside `src/games/`. Code that knows two games goes in `src/convert/<a>_<b>/`. Code for all games goes in `src/core/`.
- A dependency that only one converter uses belongs to that converter. Example: the h1 gsc context lives in `convert/h1_iw7/assets/clipmap.cpp`.

## Asset anatomy

Each asset type is one class in `src/games/<game>/assets/<type>.hpp` and `.cpp`, derived from `asset_interface`:

| Member | Job |
|---|---|
| `static read(zone_reader&, T*)` | Reads the asset from a zone. Mirrors the game `Load_*` function |
| `write(zone_base*, zone_buffer*)` | Writes the asset into a zone. Mirrors `read()` |
| `prepare(zone_buffer*, zone_memory*)` | Registers script strings before any asset data is written |
| `static dump(T*)` | Writes the asset to disk in the v1 format |
| `parse(name, zone_memory*)` | Reads the dump from disk |
| `init(name, zone_memory*)` | Calls `parse()`, or makes a name stub for a referenced asset (name starts with `,`) |
| `load_depending(zone_base*)` | Adds the assets that this asset needs to the build |

Simple types use `REGISTER_TEMPLATED_ASSET(class, Struct, ASSET_TYPE_X)` with a template from `src/core/assets/`.

To add an asset type:

1. Add the class files in `assets/` and include the header in `<game>.hpp`.
2. Add one row to `<GAME>_ASSETS`. A type that has only `read()` goes in `<GAME>_READ_ONLY_ASSETS`.
3. Add the `ADD_ASSET` row in `zone/zone.cpp` if `buildzone` must write it.
4. Run `verifyzone --all` and a round trip.

## Zone format rules

These rules are the core of the project. Most bugs are a break of one of them.

**`read()` and `write()` must match step for step.** Same field order, same `align`, same null checks and conditions, same counts, same `push_stream` and `pop_stream`. Both must match the game `Load_*` code. If you change one, change the other in the same edit. When the two disagree, the asm decides. Prove each layout change with the asm and a short pseudocode.

The one accepted asymmetry: `reader.read_script_string*` reads no stream bytes. It converts a zone-local id to a global id. It pairs with `prepare()`, which calls `buf->write_scriptstring`. Keep `prepare()` for script strings.

Writer and reader calls pair up like this:

| `zone_buffer` (write) | `zone_reader` (read) |
|---|---|
| `align(n)` + `write(data, count)` + `clear_pointer` | `read_array(field, n, count)` |
| `write_str` | `read_string(field)` |
| `write_stream` | `read_stream(field, n, size, count)` |
| asset pointer | `read_asset(type, field)`, `read_asset_array` |
| `align(7)` + `write(&data_following)` + `write_str(name)` | `read_name_reference(field)` |
| `prepare()` + `write_scriptstring` | `read_script_string`, `read_script_strings` |
| `find_sub_buffer` offset | `read_aliased_array` (game uses `DB_ConvertOffsetToAlias`) |

Stream facts:

- `align(n)` takes a mask. `align(3)` is 4-byte alignment, `align(7)` is 8-byte alignment. Alignment moves the block position only and writes no file bytes.
- H1 blocks: TEMP 0, PHYSICAL 1, RUNTIME 2, VIRTUAL 3, LARGE 4, CALLBACK 5, SCRIPT 6.
- IW7 blocks: TEMP 0, TEMP_PRELOAD 1 (aligns to 16 on pop), RUNTIME 6, VIRTUAL 8. IW7 has 10 blocks.
- IW8 blocks: TEMP 1, TEMP_PRELOAD 2 (aligns to 32 on pop), RUNTIME 4, VIRTUAL 5, SCRIPT 6, PHYSICAL 7. IW8 has 11 blocks.
- RUNTIME data holds no file bytes. The game zero-fills it.
- Data read in TEMP is lost after `pop_stream`. Copy it with `duplicate` if you keep it.
- H1 pointers: `0xFDFDFDFFFFFFFFFF` is inline, `...FE` is inline with an insert slot. Other values are offsets: block `(v >> 32) & 0xF`, offset `(u32)v - 1`.
- IW7 pointers: -1 is shared (treat as inline), -2 is inline, -3 is insert. Other values are offsets: block is the high dword.
- IW8 pointers: the same as IW7. The block is `(v >> 32) & 0xF`, and bit 36 marks an asset-relative offset.
- IW8 structs: the OpenIW8 Xbox header is a newer build than PC 1.20. Prove each struct size and offset with the PC loader. `docs/IW8_SUPPORT.md` lists the known differences.
- The game reads most arrays inline when the pointer is not null, and ignores the value. Third-party zones store raw heap pointers there. `zone_reader` follows only a valid offset.
- Stock zones point some fields into arrays that are already written. The writers reuse these with `insert_sub_buffer` and `find_sub_buffer`, so built zones keep the stock size.

## Compatibility rules

- Keep working v1 logic. x64-zt asset code is studied and tested since 2021. If it works, do not change it.
- Use zero donor assets. A converter builds each target asset from the source asset data only. Do not copy, load or fall back to a donor asset of the target game. This includes techsets and shaders: convert them natively. Existing donor paths, for example the S2 to H1 material remap, are debt: remove them, do not extend them. One exception: the IW8 to IW7 converter references stock IW7 techsets by name where IW7 has an equivalent. It converts a techset natively only when IW7 has no equivalent. See `docs/IW8_IN_IW7_FOCUS.md`.
- Keep the v1 dump formats. A dump from v1 must parse in v2. Change a format only when it is necessary, and tell the user.
- v2 has no game runtime. A database lookup can return null where v1 got a game default asset. Add a null check, not a fake default.
- Known v2 differences from v1: `dumpzone <zone> --target <game>` replaces `dumpzone <target> <zone>`, IW7 `proceduralbones` has a dump format, built zones share arrays the way stock zones do, and output goes to the `zone/` folder of the game folder.

## Converter structure

```
src/convert/<a>_<b>/converter.cpp    required zones and one dump call for each asset type
src/convert/<a>_<b>/include.hpp      the headers of both games
src/convert/<a>_<b>/assets/<type>.*  one file pair for each asset type
```

- The namespace is `zonetool::<a>::converter::<b>::<type>`, for example `zonetool::iw5::converter::iw7::xmodel`.
- Each type has `dump(<a>::T*)`. If another type needs the converted asset, the type also has `convert()`. `dump()` calls `convert()`, then the `dump()` of game `<b>`:

```cpp
void dump(zonetool::h1::XModel* asset)
{
	utils::memory::allocator allocator;
	const auto converted_asset = convert(asset, allocator);
	zonetool::iw7::xmodel::dump(converted_asset);
}
```

- Put helpers in an anonymous namespace in the `.cpp` file. Declare in the `.hpp` file only what other files call.
- Allocate all target data from the allocator that `convert()` gets. Do not keep asset data in globals or `static` variables.
- Convert a shared asset one time. In the function that converts the array, keep a map from source pointer to target pointer, for example `std::unordered_map<zonetool::iw4::XModel*, zonetool::iw5::XModel*>`.
- Convert each field by name. Map each enum value with a `switch` or a table. The same enum name can have a different value in each game:

| Value | IW3 | IW4, IW5 | IW7 |
|---|---|---|---|
| `CAMERA_REGION_EMISSIVE` | 2 | 2 | 4 |
| `CAMERA_REGION_NONE` | 3 | 4 | `CAMERA_REGION_NONE_2` 11 |

- Copy a struct with `memcpy` only if both structs have the same fields in the same order. A `static_assert(sizeof(...))` checks the size only, so also compare the fields in `structs.hpp`.
- Never copy a pointer value from a source struct into a target struct. In the x86 chain (`iw3_iw4`, `iw4_iw5`), an `x86::pointer<T>` is a 32-bit offset into the x86 address space. A copied pointer still points to data of the old game, and the next stage reads it with the wrong layout. Convert the asset with the converter of its type, or make a name reference.
- An `x86::pointer` can hold only an address in the x86 region. Allocate x86 data and strings with the `x86::allocator`, for example `allocator.duplicate_string("default")`. A string literal or memory from `new`, `std::vector` or `utils::memory::allocator` is not in the region.
- A name reference is a target asset that holds only the name, for example `material_reference` in `src/convert/iw3_iw4/assets/fx.cpp`. Use it when the target dump writes only the name.
- The IW8 dump walk dumps RUNTIME arrays too. For each RUNTIME array, allocate a zero-filled array with the count that the walk uses. Do not share one allocation between two arrays, because the walk deduplicates arrays by address range.
- If the target game has a field that the source game does not have, compute it from the source data. Example: the IW5 to IW7 converter sets `numLightingValues` from the vertex count of the model. Do not read the value from a stock target asset.
- Log a lossy conversion with `ZONETOOL_WARNING` and the asset name.
- A chain converter (`iw3_iw7`) runs each stage in order. A field that one stage drops is lost for all later stages. Check that a new value reaches the final dump.

## Research with re_reference

`re_reference/<game>/<build>/` holds the IDA text export of one game exe. `DUMPS` in `tools/asm_slice.py` maps each dump name to its folder.

| File | Content |
|---|---|
| `ship_asm.txt.xz` | The packed disassembly that git holds. `asm_slice.py` unpacks it |
| `ship_asm.txt` | The full disassembly with IDA names. Git ignores it. Never open it with a file tool |
| `func_index.txt` | The line in `ship_asm.txt`, the start, the end and the name of each function |
| `funcs.txt` | The start, end, size, xref count and name of each function |
| `names.txt` | Each named address and its kind |
| `strings.txt` | Each string with its address, length and xref count |
| `types.h` | The local types of the IDA database |
| `imports.txt`, `segments.txt`, `ptr_tables.txt` | Imports, segments and pointer tables |

- `asm_slice.py` needs `ship_asm.txt` and `func_index.txt`. Search the other files with `grep`.
- On the first use of a dump, `asm_slice.py` unpacks `ship_asm.txt.xz` to `ship_asm.txt`. It unpacks again when the `.xz` file changes. Before you use `rg`, run `python tools/asm_pack.py unpack`.
- Do not commit other export files, for example `func_index.pkl`. `asm_slice.py` does not read them.
- To add a dump, put it in `re_reference/<game>/<build>/` and add its name to `DUMPS`. Then run `python tools/asm_pack.py pack <dump>` and commit `ship_asm.txt.xz`. The pack removes the IDA instruction descriptions and keeps the line count. IW3, IW4 and IW5 have no dump yet.

Research steps:

1. Start from the `Load_*` function of the asset. It gives the stream order, the struct sizes and the array strides.
2. To find the code that uses a field, search the field offset with `-r`. Keep the hits in functions that also use the struct. If a field has only one reader, that reader shows what the field does.
3. To find an unnamed function, search a dvar name or an assert text in `strings.txt`. Then search the IDA label of the string with `-r`. Example: `-r aRSmodelinstanc` finds `R_RegisterDvars`, which registers `r_smodelInstancedRenderer`.
4. Map a call stack with `-l`. List the callers of a function with `-c`.
5. Read every caller before you give a meaning to a field. One function can use the same register for a different struct.
6. Check a field meaning on many stock assets. One example is not proof.
7. Trust the code, not the name. Many IDA names in the H1 1.15 dump are wrong, because the compiler folds identical functions into one.
8. If a dump has few names, find the same function in a game of the same engine through a shared string or constant. Example: for S2, search S1 or H1 1.4.
9. Write the proof in the doc of the game before you write the code: the function, its address, the condition and the fields.
10. Keep a table of proven functions in the doc of the game: address, name and what it proves. Start the next session from that table.

Read the asm:

- A line starts with the address in lowercase hex without `0x`, for example `140dcdba0`. A function name has uppercase hex: `sub_140DCDBA0`.
- IDA writes operand values in uppercase hex with an `h` suffix, and adds a `0` before a letter: `[rax+74h]`, `0B8h`, `[rdx+0A2h]`. Search `\+0A2h\]`, not `0xa2`.
- A comment such as `; Logical Compare` or `; Load Effective Address` is the IDA description of the instruction. Ignore it. A comment such as `; flags` or `; value` names a call argument.
- A string label starts with the first characters of the string, for example `aRSmodelinstanc`. Two strings with the same start get `_0`, `_1`. To get the label of a string, find its address in `strings.txt`, then find that address in `names.txt`.
- A multiply by a constant before an array access shows the struct size. `imul rdx, rsi, 0B8h` indexes records of 0xB8 bytes. Search the stride to find code that walks an array of that struct.
- The dvar register code stores the dvar pointer after the register call. Example: `call Dvar_RegisterBool`, then `mov cs:off_1483E9228, rax` for `r_smodelInstancedRenderer`. `-r off_1483E9228` lists every reader of that dvar.
- In a live process, ASLR moves the exe. The live address is the module base plus (IDA address - 0x140000000). For an H1 1.15 RVA, add the RVA to the module base.

Fast searches (times for the IW7 dump, 326 MB):

| Job | Command | Time |
|---|---|---|
| Map addresses to functions | `asm_slice.py iw7 -l <address> <address> ...` | 0.2 s for all |
| Print a function | `asm_slice.py iw7 sub_140DCDBA0` | 0.7 s at line 3.4M |
| Search text, line numbers only | `rg -n "\+74h\]" re_reference/iw7/ship/ship_asm.txt` | 0.4 s |
| Search text, with function names | `asm_slice.py iw7 -r "\+74h\]"` | 1.5 s |

- Give `-l` all addresses in one call.
- `-r` and `-c` read the whole file one time for each target. Join regexes with `|` in one target.
- `-r` stops after `-m` hits (default 400) and prints `; more references`. Make the regex narrow with the mnemonic or the register, for example `"cmp .*\[r..\+74h\]"`.
- `-c` finds only direct `call <name>` lines. It does not find a `jmp` tail call or a call through a table. For those, search the name with `-r` and the address in `ptr_tables.txt`.
- A name target is a Python regex, and `asm_slice.py` prints the first 20 matches. Use `^` and `$` for one exact name.
- For a large function, print only the lines around an address with `-a -m 80`.
- `rg` with a hit limit does not load the file. Use it for a fast text search, then map the hit addresses with one `-l` call.

## Asset hunts

Start from the symptom. Then check the asset that it points to.

Material and techset:

- Check that the techset has each technique that the draw path selects. A draw path can add an offset to the technique index. In IW7, the instanced static model path adds 0x26. A techset without that technique draws nothing for that path.
- Read the technique list in `dump/<zone>/techsets/<techset>.techset.txt`. Compare it with a stock techset of the same kind.
- Check the camera region and the sort key after the conversion. A wrong camera region puts the surface in a different draw list.

XModel and XSurface:

- Take each struct size from `structs.hpp` and the `Load_*` code. A script with a wrong size reads the first surface correctly and the rest as garbage.
- IW7 `XSurface.vertexLightingIndex` is a running sum of the vertex counts of all surfaces in all LODs of the model. Do not reset it for each LOD.
- In a chain, check that each surface has its converted material in the final stage.

GfxWorld and ComWorld:

- If a model appears and disappears when the camera moves, the bug is in the culling or in the draw path, not in the material.
- Ask the user to test one dvar at a time. In IW7, `r_umbra 0` turns off umbra culling. `r_smodelInstancedRenderer 0` turns off the instanced static model path.
- A dvar test finds the path. It is not the fix. A map must work with the default dvars of the game.
- IW7 draws a static model rigid, not instanced, if one of these is true (`sub_140DCDBA0`): flag 0x400, a SUBDIV LOD, `numLightingValues` is not 0, or `lightmapIndex` is not -1.
- Count the assets that a change affects before you build. Example: `>= CAMERA_REGION_EMISSIVE` made 661 static models rigid. `== CAMERA_REGION_EMISSIVE` made only the 18 lamp models rigid.
- GfxWorld refers to the ComWorld primary lights by index. Keep the count and the order of the primary lights the same in both assets.

GfxImage:

- The warning `does not account for <n> bytes` means that the pixel data size does not match the width, height, format and mip count. Check the format map and the mip count.

Render bugs:

- Use a stock map of the target game as the control for each question. Dump the stock value before you guess a value.
- Fix lost data first: missing models, layers, masks or techniques. Do not adjust the tonemap, the tint or the exposure before that.
- A dvar test is valid only if the dvar exists. Some clients print nothing for an unknown dvar, and a string in `strings.txt` does not prove that the game registers it.
- A control build with one changed value gives a clearer result than a dvar test.
- To test an input, replace it with a constant. Do not scale it: `0 * NaN` is NaN, and a clamp can hide a scale.
- A finite value is not a correct value. A check that the data is valid does not prove the scale.
- Do not edit render data in live memory. The renderer copies world data at map load. Read live memory only to examine values.
- In a RenderDoc capture, find the material of a draw by the DXBC checksum of its shader (bytes 4 to 20 of the bytecode). Search the `.cso` files of the dump for it.
- A live process uses ASLR, so IDA addresses of globals do not apply. Find a world asset through a pointer to its name string.

Other assets:

- Find the `Load_*` function, then the code that reads the asset at run time. Compare a converted asset with a stock asset of the same kind, field by field.

Scripts for dump files:

- To count or compare dumped binary files, write a small Python script in the scratchpad. Take each struct size and offset from `structs.hpp`.
- Print counts and a short list, not every record.
- Keep the script out of the repository, unless the user wants a new tool in `tools/`.

## Verification

Run these after a change to a reader, writer or the container code:

| Check | Command | Pass condition |
|---|---|---|
| Container | `inspect --all` | Every zone passes |
| Reader | `verifyzone --all` | The count of the game in the table below |
| Writer (round trip) | `dumpzone X`, `buildzone X`, `verifyzone X`, then `dumpzone` the built zone | The two dumps are equal, except pointer values and the known differences |

| Game | `verifyzone --all` | Round-trip zones |
|---|---|---|
| H1 | 6991 of 6991 | `mp_shipment`, `mp_crash` |
| H2 | No full count. `patch_gulag`, `code_post_gfx`, `common` and `gulag` pass | `patch_gulag` |
| IW6 | 878 of 878 | `mp_battery3` |
| IW7 | 696 of 697 (`mp_vlobby_room` on disk is a modified file) | `mp_frontend`, `mp_afghan` |
| IW8 | 9471 of 9494 (`code_post_gfx` and old frontend zones fail, see `docs/IW8_SUPPORT.md`) | `mp_rust`, `srv_mp_rust`, `techsets_mp_rust` |
| S1 | 3058 of 3058 | `mp_prison` |
| S2 | 268 of 268 | `common`, `mp_aachen_v2` |

The total changes when the zone folder changes. A zone that passed before and fails now is a regression.

## Code style

- lowercase snake_case for functions, variables, classes and files. Engine structs keep their engine names (`XModel`, `GfxImage`).
- Tabs, braces on their own line, `this->` for members, `const auto` where the type is clear. Match the file you edit.
- Keep the structure flat. Do not make a file for one declaration. Put a small declaration in the header that already owns the topic. A list macro belongs in the game header, and a value that one file uses belongs in that file.
- No umbrella headers outside `<game>.hpp`. Include what you use.
- Name things so that the code reads without comments. Prefer a small well-named function to a comment over a block.
- Do not make named constants by default. Write the value where the code uses it. Keep a named constant only for a shared value that is unknown at the use site, for example a format version or a file magic.
- An engine flag, id or type value goes in the game `structs.hpp` as an enum value, not as a `constexpr` in a `.cpp` file.
- Use `constexpr` only where the language needs a compile-time value: array sizes, template arguments, `static_assert`, `if constexpr` and field tables.
- Do not define structs in `.cpp` files. Engine and file-format structs go in the game `structs.hpp`. Helper structs go in the header of the `.cpp` file.

### Comments

Default: no comment. About 99% of the lines in x64-zt have no comment, and new code keeps that ratio. Before you add a comment, try to delete it. Less is better, and nothing is best.

Write a comment only for one of these cases:

- a short label for one line whose effect the code cannot show, for example a value that only an engine check uses;
- a deviation from the asm or from v1 that a reader would "fix" back;
- a hack or workaround that can break later;
- a TODO;
- an address or a name that the code cannot show, for example an unnamed builtin.

A label is 2 to 5 words, with the address when there is one.

Form:

- Put the comment at the end of the line that it describes. Do not put it on its own line above the code.
- Use the fewest words possible. A name and an address is enough.
- One line only. No comment blocks, no comments above functions, no section banners.

Do not write:

- an explanation of how the game or the client uses a value. A label is enough;
- where the data comes from, or which function reads or writes it;
- research notes, debug findings or the reason behind a fix. Put these in the commit message or in `docs/`;
- restatements, edit narration or commented-out code.

Examples:

| Bad | Good |
|---|---|
| `// s2 ambientplay is builtin 0x11A (sub_140514B40), gsc-tool has no name for it` on the line above | `// ambientplay (0x140514B40)` at the end of the line |
| `// the s2 client reads "fx_<fx>;<tag>" and crashes on a missing tag (MS 0x14039CEC0)` | no comment |
| `// the client parses the createart fog source for "ent = maps\_utility::create_vision_set_fog(" (CG_ParseClientVisionsetTriggers)` | no comment |
| `// the rawfile of the same name has the original source, dump_source writes it` | no comment |
| `// IW7 draws a static model rigid when numLightingValues is not 0 (sub_140DCDBA0), so emissive models get values` on the line above | `// forces rigid draw (sub_140DCDBA0)` at the end of the line |
| `// convert each material once` above a loop with a cache | no comment. A function name such as `convert_materials` shows it |

If you are unsure whether a comment qualifies, it does not.

### Technical writing

Docs, commit messages and user-facing text use ASD-STE100 style:

- Max 20 words per sentence in instructions, 25 in descriptions.
- Imperative for steps. One instruction per sentence. Condition before command.
- Simple tenses only. No present perfect, no -ing verbs, no should/would/may/might.
- Active voice. One word per meaning, no synonym rotation.
- No contractions. Keep articles and "that".
- Delete filler: simply, robust, seamlessly, leverage.
- Code and identifiers stay exact.

## Pull requests

- Keep one topic in each pull request: one game, one converter or one asset type. Do not mix a refactor with a fix.
- Change only the lines that the fix needs. Do not reformat other lines. Keep the line endings of each file, and run `git diff --check`.
- For each layout change, give the proof in the description: the `Load_*` function, its address and the game build.
- Give each check that you ran and its result: build, `verifyzone` count, round trip, test in the game. Report the build result and the game result separately.
- List each check that you did not run as "Not verified".
- Do not commit machine paths, user names, drive letters, dumps, built zones or game files.
- Put research notes in the pull request description or in `docs/`, not in code comments.
- Update this file when you add a command, a flag, a game, a dump or a verification count.

## Agent playbook

This section is for agents that work alone for a long time, for example during a map port. It comes from real sessions and their mistakes.

### Work loop

1. Find the code with the "Source layout" table and one search. Do not list the repository or search a whole drive.
2. Read the function, 100 to 200 lines from the search hit. Never open a large file from line 1.
3. Write the cause in one sentence before you edit. If you cannot, get more evidence first.
4. Make the smallest edit that fixes the cause. Copy the style of the code around it.
5. Build once. Fix the first compile error first.
6. Run the checks in "Verification" that apply to the change.
7. Reply with the build state, 3 to 6 bullets, and what you did not verify.

### Speed

Times from a 72-minute session that ported the S2 hub to H1:

| Step | Time | What to do |
|---|---|---|
| One `Get-Content` of `ship_asm.txt` (1.5M lines) | about 30 s, many times | `python tools/asm_slice.py <dump> <address or name regex>` takes less than 1 s |
| S2 zone reads from a slow disk | about 20 min of the dump | Start the dump early, in the background |
| Streamed images | about 10 min of the dump | Keep `dump/<zone>/.zonetool_cache`. The next dump skips the files |
| Material conversion | about 30 min of the dump | Run the dump once. Fix build-side bugs without a new dump |
| `buildzone` of the hub | about 6 min | Run it in the background |
| Full rebuild after a new `--games` set | more than 10 min | Keep the same set |

- Run long commands in the background and redirect output with `*> file.log`. `Tee-Object` holds the output, so you see no progress.
- A harness can stop a foreground shell command after about 2 minutes. Run a longer job in the background and wait for it.
- Compare hex addresses as integers, not as strings.
- Before a long dump, check that the exe in the game folder is the one that you just built (size and time).
- Batch reads. List every function that you need, then read them all in one step.
- Do not run a dump and a build of the same zone at the same time.

### Search recipes

```bash
python tools/asm_slice.py h1_1.4 -l 14060B312 1406060E9 140605A69
python tools/asm_slice.py h1_1.4 14060B312 -a -m 80
python tools/asm_slice.py h1 "Load_Material$"
python tools/asm_slice.py s2 -c Load_MaterialTechniqueSet
python tools/asm_slice.py iw8 Load_GfxWorld -m 400
python tools/asm_slice.py iw7 -r "aRSmodelinstanc"
python tools/asm_slice.py iw7 -r "\+74h\]"
grep -n "struct GfxStateBits" -A12 src/games/h1/structs.hpp
grep -n "r_smodelInstancedRenderer" re_reference/iw7/ship/strings.txt
```

- Dump names: `h1` (1.15, RVAs), `h1_1.4` (full addresses), `h2`, `iw6`, `iw7`, `iw8`, `s1`, `s2`, `s4`.
- `-l` maps a call stack to function names. `-a` prints the lines around an address. `-c` lists every call site of a function.
- `-r` searches the text of each line with a regex, not an address. Use it for a string label, a global or a field offset.
- A dumped techset has a readable technique list in `dump/<zone>/techsets/<techset>.techset.txt`.
- The state data of each material is in `techsets/state/<techset>/<material>.statebits`, `.statebitsmap` and `.stateinfo`.
- The build log names each asset that it parses. Search the log for the asset name.
- A zonetool log from a Windows PowerShell 5.1 redirect is UTF-16, and `grep` finds nothing in it. Check with `file <log>`, then convert with `iconv -f UTF-16LE -t UTF-8 <log>`.

### Theories

1. Write 3 candidate causes, not 1. For each candidate, write what it predicts that the others do not.
2. Do the cheapest check that can remove a candidate first.
3. Write the cause as a chain. Give each link its proof: a register, a memory value, a log line or a code line (`file:line`). A link without proof is a guess.
4. A theory must explain every observation. Example: two runs crash at the same address with the same registers, so a timing theory is wrong.
5. Write the result that you expect before you test in the game. If the result is different, the theory is wrong. Remove the first fix before you try a second fix.
6. After three null results in one line of tests, stop that line. Go back to the list of inputs and bisect.
7. Keep a "Ruled out" list in the doc of the game. Do not test an item on it again without a new reason.

### A game crash

1. Read `<game folder>/minidumps/<newest>/info.txt`: exception address, registers, call stack and last zone. The `.dmp` file is in the same folder. If the game folder has no minidump, look for a Windows process dump in `%LOCALAPPDATA%\CrashDumps`.
2. Map the call stack with `python tools/asm_slice.py h1_1.4 -l <addresses>`.
3. Read the faulting instruction with `-a`. Write which register holds which struct and field.
4. The h1-mod minidump holds the stack, but not the heap or the zone memory. While the crash dialog is open, the process is alive. Read its memory with `tools/livemem.py`:
   - `python tools/livemem.py <pid> read <address> 40` prints 0x40 bytes.
   - `python tools/livemem.py <pid> refs <address> --stride 28 --field 140` finds the struct that owns an array that reaches `<address>`. This example finds the H1 `Material` whose `stateBitsTable` holds a `GfxStateBits` (0x28 bytes).
   - `python tools/livemem.py <pid> h1-material <address>` lists the techniques that have no valid state bits.
5. Find the asset in `dump/<zone>/` and in the build log. Then find the code that made the bad value.
6. Close the crashed process only after you have the evidence.

Known crash: `sub_14060B200` in 1.4 reads `GfxStateBits.zone` from `stateBitsTable[stateBitsEntry[techType] + pass]`. If a technique has `stateBitsEntry` 0xFF, the game reads entry 255 and crashes. `buildzone` logs this case as `has no state bits for technique`.

### Test in the game

Ask the user before you start the game or send input to its window. Many users test by hand. Then give the user a short list for each test: a spot (a `setviewpos` line or a landmark) and the thing to look for.

A visual feature works only after a game screenshot or a report from the user shows it. `buildzone` and `verifyzone` prove the file, not the picture. A debug output in a shader is allowed for one test build. Remove it before the next build.

1. Build the zone with `buildzone <map> -use_usermaps`.
2. Start the game from the game folder: `h1-mod.exe -noupdate -multiplayer +devmap <map>`. `devmap` enables cheats. Most Call of Duty clients have `map` and `devmap`.
3. Control the game window with `tools/game_window.ps1`. `-process_name <name>` selects a process other than `h1-mod`.
   - `shot <file.jpg> [scale]` saves the client area of the game window only.
   - `click <x> <y>` clicks at client coordinates. Divide the coordinates of a scaled shot by the scale.
   - `key tilde` opens the console, `type "<text>"` types text and `key enter` runs it.
4. Select "Auto-assign" and a class. Then move with `setviewpos <x> <y> <z> <yaw> <pitch>`.
5. Get coordinates from the map entities: the `origin` of named entities in `dump/<map>/maps/mp/<map>.d3dbsp.ents`.
6. After each move, take one shot and look for a new folder in `minidumps`.

### Known mistakes

- A search of a whole drive or a recursive listing of the game folder. The output fills your context, and the search takes minutes.
- `cmd /c` with `tools\build.bat` and a `--copy-to` path that has spaces or parentheses. The quotes break, and premake reports `no such action`. Run MSBuild directly, then copy the exe.
- An edit of a file that you did not read in this session. Some edit tools refuse it.
- A fix in the converter for a bug in the build side (`material::parse`, `techset::parse`, the `zonetool_paths` merge). Check the dumped files first. If the dump is correct, the bug is in the build.
- Trust in the output of `DEEP_LOOK_TECHNIQUES`. It merges technique slots from every techset of the same name in `zonetool/` and `zonetool_paths/`. The merged techset can have techniques that the material dump does not have.
- `&&` in Windows PowerShell 5.1. Use `;` and `if ($?) { ... }`.
- A raw copy of a pointer between two games. See "Converter structure".
- A `<` or `>=` compare of an engine enum value. The order of the values changes between games. Compare with `==` and the named value.
- A fix in the last stage of a chain when an earlier stage drops the data. Check that the value reaches the final dump.
- A dvar change as the fix. A map must work with the default dvars of the game.
- A change that you did not count. Count the assets that the change affects before you build.

### When the build fails

1. Find the first `error C` or `error LNK` line, and fix that error first.
2. If the same error comes back, read 30 lines around it again. Do not try the same edit again.
3. After three failed tries, stop. Report the exact error line and what you tried.

### When you are stuck

Say what you searched, what you found and what is missing. Ask one specific question. Do not fill the gap with a guess.

### Checklist before you reply

1. The build of this run succeeded, and the game folder has the new exe.
2. `verifyzone` passes for each zone that you built.
3. `git status --short` lists only the files that you meant to change.
4. `git diff` has no unrelated edits, no reformatting and no debug code.
5. The docs of the work (`docs/<GAME>_SUPPORT.md` or a handoff file) have a short dated entry with the result.
6. Nothing is committed, unless the user asked for a commit.

### Reply format

```
Built and copied. Zone built to usermaps. Tested in the game: no crash at 6 spots.

- techset.cpp parse_statebits: opens the merged file from its own search path.
- material.cpp write: logs techniques that have no state bits.

Not verified: IW6 build (IW6 is not in this --games set).
```
