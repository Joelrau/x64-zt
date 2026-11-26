# AGENTS.md

Guide for humans and agents who work on x64-zt. Read it before you change code.

It applies to every agent and model. A smaller model can do good work here if it follows this file step by step. "Ground rules" apply to every task. "Working formula" is the loop that each task follows. The sections after them are reference. Read the section for your task when you start it. `CONTRIBUTING.md` tells new contributors how to set up and how to send a pull request.

| Task | Read |
|---|---|
| Fix a reader or a writer | "Zone format rules", "Verification" |
| Add a game | "Add a new game", "Research with re_reference" |
| Port a map | "Port a map", "Map events and scripts", "Render bugs" |
| Port a weapon | "Port a weapon", "Materials and techsets" |
| Fix a render bug | "Render bugs", "Materials and techsets", "Images" |
| Fix a bad conversion | "Debug a bad conversion", "Converter structure" |
| Fix a crash | "A game crash" |
| Keep a game working | "Maintain a game", "Verification" |
| Send a pull request | "Pull requests", `CONTRIBUTING.md` |

## What x64-zt is

x64-zt is a fastfile unlinker and linker for Call of Duty titles. It reads the `.ff` zones of a game and dumps their assets to disk. It builds new zones from those dumps, also with new assets. It converts assets and full maps from one game to another as well.

The `v2` branch is a standalone rewrite of x64-zt with the assistance of AI to help add new games and keep a cleaner codebase. v1 (`main`) loaded the game binary and hooked its zone loader. v2 reads fastfiles directly and never loads a game exe. v2 keeps the v1 asset logic and the v1 dump formats. `develop` is the active branch.

| Name | Game | Doc |
|---|---|---|
| `h1` | Modern Warfare Remastered 1.15 | `docs/h1-115-format.md` |
| `h2` | Modern Warfare 2 Campaign Remastered | `docs/H2_sync.md` |
| `iw3` | Call of Duty 4: Modern Warfare (read only, x86) | `docs/X86_GAMES_AS_X64_X64ZT_2.md` |
| `iw5` | Modern Warfare 3 (read only, x86) | `docs/X86_GAMES_AS_X64_X64ZT_2.md` |
| `iw6` | Ghosts | `docs/IW6_SUPPORT.md` |
| `iw7` | Infinite Warfare | `docs/IW7_IW8_MAP_PORT.md` |
| `iw8` | Modern Warfare 2019 1.20 | `docs/IW8_SUPPORT.md`, `docs/IW8_IN_IW7_FOCUS.md` |
| `s1` | Advanced Warfare | `docs/S1_SUPPORT.md` |
| `s2` | WWII | `docs/S2_SUPPORT.md`, `docs/IMPROVE_H1_S2.md` |
| `t9` | Black Ops Cold War | `docs/T9_SUPPORT_AND_T9_IN_IW8.md` |

IW4 exists only as a stage of the x86 converter chain (`iw3_iw4`, `iw4_iw5`). S4 (Vanguard) work is on its own branch and is not on `develop` yet. Other games are not supported yet. They need to be added or re-added. "Add a new game" tells how.

## Ground rules

These rules apply to every task. They apply to agents more than to humans, but they are good practice for humans too. Later sections give the detail.

Code:

1. The asm in `re_reference/` is the source of truth. When the code and the asm disagree, the asm decides. Do not guess a layout, an offset or a game behavior. Prove it with the asm.
2. `read()` and `write()` of an asset match step for step, and both match the game `Load_*` code. Change both in the same edit. See "Zone format rules".
3. Keep working v1 logic and the v1 dump formats. Build converted assets from source data. See "Compatibility rules".
4. Do not write hacky or sloppy shortcut code. Fix the cause, not the symptom.
5. Keep code readable for humans. x64-zt was a human project before AI, and code that only a machine can follow is not accepted. Use named struct fields, not raw offsets and byte casts. Comments do not fix sloppy code.
6. Put engine structs in the game `structs.hpp`. Do not declare structs inline in an asset file or a converter file.
7. Write almost no comments. Most comments in this repository are noise. See "Comments".

Work:

8. Never open a `ship_asm.txt` file with a file tool, `Get-Content` or `ReadAllLines`. The files are 85 MB to 600 MB. Use `tools/asm_slice.py`, or `rg` with a hit limit.
9. h1-mod runs the H1 1.4 binary. For an address from an h1-mod crash, use the `h1_1.4` dump, not `h1` (1.15).
10. Before each dump or build, check that the `zonetool.exe` that you run is the one that you just built (size and time). A dump with an old exe wastes up to one hour. Run it from a scratch work folder with `-path <game folder>`, or copy it to the game folder. Never overwrite a `zonetool.exe` that you did not build.
11. Keep the same `--games` set for `generate.bat`. A different set regenerates the projects and rebuilds everything, which takes more than 10 minutes.
12. Do the work in the main session. Start a subagent only when the user asks for one. Give each subagent rule 8.
13. Put temporary files in a session scratchpad or a temporary folder, not in the repository or `/tmp`.
14. `core.autocrlf` is `true`. Some files are CRLF in the working tree. Keep the line endings of the file you edit.
15. Do not report a result that you did not check. List each check that you did not run as "Not verified".
16. Do not guess. If you do not know how to do a step in this file, read the tool help first. Then ask the user one specific question.
17. Make only proven fixes. If a test build changes nothing in the game, the theory is wrong. Revert the fix and add the theory to "Ruled out".

Safety and privacy:

18. Commit only when the user asks. Do not add yourself as author or co-author of a commit. The author is the user.
19. Never close a game or tool process that you did not start. You can close a crashed process that you started, but read its memory first (see "A game crash").
20. Do not start a game or send input to a game window unless the user asks. Most users test by hand.
21. Look at a target before you write, move or delete it. Test a destructive command only in a scratch folder, never in a dump folder or a game folder.
22. Keep machine paths, user names and drive letters out of the repository.
23. Take screenshots of the game window only, not of the desktop or other windows.

## Working formula

Each task follows the same loop. The loop works for a new game, a map port, a render bug and a crash.

1. Write a task file. Make `docs/<TASK>.md` (git ignores `docs/`). Put in it the request of the user word for word, the rules of the task, an image index, the open items, a "Proven facts" table, a "Ruled out" list and a dated log. The conversation is not the memory. After a context compaction, read the task file again before the next tool call.
2. Find the control. For each question, find a stock asset or a stock map of the target game that does the same thing. Dump it. A stock control answers most questions faster than the asm.
3. Survey all assets, not one. Write a small Python script in the scratchpad that reads every dumped asset of the kind and prints counts. One example is not proof.
4. Prove the cause. Give each fact a proof level (table below). Write the cause as a chain, and give each link its proof. Write the proof into the task file before you write the code.
5. Make the smallest fix that the proof supports. Count the assets that the fix changes before you build.
6. Build, run `verifyzone` and back up the last package. Then give the user a test list: a spot and the thing to look for at each spot.
7. Read the result of the user. Mark each item as fixed in the game, fixed in code only, or open. If nothing changed, go back to step 4.
8. Add a dated entry to the task file.

Proof levels:

| Level | Meaning |
|---|---|
| `asm` | A function and its address in a `re_reference` dump |
| `verify` | The reader consumes N stock zones exactly. Give N. One zone is not proof |
| `stock` | The same pattern in stock assets of the target game. Give the count |
| `capture` | A value read from a RenderDoc capture |
| `survey` | Bytes read from dump files without a code proof |
| `hint` | Source of another tool (v1 zonetool, OpenAssetTools, atian-cod-tools, OpenIW8 headers). A hint is not proof |

Code that depends on a `hint` fact goes into a "Re-verify" list in the task file.

What the user expects:

- Report each item that the user listed. Track each item in the task file, and do not skip one.
- Do not tune a value by eye. A fix without proof often makes the result worse in other places.
- Before each report, compare the new output with the last output that worked. A material match that is too wide can break assets that worked before. Keep a copy of the last good dump folder for this compare.
- Do not remove a proven step because of an inferred cause.
- A visual feature works only after a game screenshot or a report from the user shows it.
- If a test build has a debug output, tell the user exactly what it looks like.
- If a feature does not exist, say so plainly. Do not report a part of it as the feature.

## Work with the user

The user knows the game and how it must look. The agent knows the data. Ask the user for what you cannot read from the files.

- Use the question tool of your harness when you need a decision. Ask one specific question. Do not be afraid to ask.
- If the user does not know the asset names or what to fix, ask for screenshots. Ask for 5 to 8 positions. At each position, ask for a pair: one picture from the source game and one from the target game, at the same `setviewpos`.
- Give the user the `setviewpos` lines. Take them from the `origin` of named entities in the map entity dump, for example `dump/<map>/maps/mp/<map>.d3dbsp.ents`.
- Ask for reference photos of the source game first. They show how the map must look.
- For a render bug, ask for a RenderDoc capture of the same spot in both games. See "RenderDoc".
- You can take game screenshots yourself with `tools/game_window.ps1` when the user allows it. Take one shot after each move.
- Write each image into the image index of the task file: file name, game, build, position and what it shows.
- Copy the words of the user into the task file when they report a test. Their words are the bug list.

## Before you start

- `re_reference/` is in git. It holds the packed asm (`ship_asm.txt.xz`) and the index files of each dump. `tools/asm_slice.py` unpacks the asm on the first use.
- `docs/`, `dump/` and `build/` are local folders that git ignores. A fresh clone or a git worktree does not have them. Read `docs/` in the main checkout, which `git worktree list` prints first. If there is no `docs/`, make it and start your task file there.
- Read the doc of a game before you work on that game. A doc can be long. Read its headings first, then the sections for your task and the newest log entries.
- This file does not list the game folders. If you do not know the folder of a game, ask the user. Do not search a drive for it.
- Not every harness has every tool that this file names: file tools, background jobs, subagents, screenshots. If you cannot do a step, tell the user. Do not skip it silently.
- Other sessions can work in the same checkout. Files that you did not change can show as modified. Do not revert, stage or reformat them.

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
- The CI builds Release with warnings as errors. A warning that Debug accepts can fail the pull request. Build Release before you push.
- If a build fails with `C1083 ... Permission denied`, `LNK1104 cannot open zonetool.exe` or `MSB6003 ... file in use`, a different process holds the file. Do not stop that process. Link into a scratch folder instead:

```powershell
& $msbuild build\zonetool.vcxproj -p:Configuration=Release -p:Platform=x64 -p:BuildProjectReferences=false "-p:OutDir=<scratch>\zt_out\" "-p:IntDir=<scratch>\zt_obj\"
```

## Run

```bash
zonetool.exe -game h1 dumpzone mp_shipment
```

| Command | Result |
|---|---|
| `inspect <zone>`, `inspect --all` | Quick inspect a fastfile container and its data |
| `verifyzone <zone>`, `verifyzone --all` | Reads every asset and checks that the reader consumed the zone exactly |
| `listzone <zone>`, `listzone --all` | Writes the asset CSV of each zone to `asset_listing/<game>/<zone>.zone`. It dumps no assets |
| `listzones` | Prints the name of each zone of the game, one name on each line |
| `dumpzone <zone> [--target <game>]` | Writes `dump/<zone>/` and `dump/<zone>/<zone>.csv`. `--target` converts the assets |
| `dumpmap <map> [--target <game>] [--skip-common]` | Writes the map zones of `<map>` to `dump/<map>/` with one merged `<map>.csv`. It dumps only techsets from common zones (H2 `common`: images, IW6: every common asset that the map zones reference). `--skip-common` loads the common zones but does not dump them. Patch zones load after the map zone. It dumps only the last copy of an asset, because the game uses the patch copy. H1, H2, IW6 and S1 only |
| `dumpasset <type> <name> <zone> [--target <game>]` | Reads `<zone>` and writes one asset to `dump/assets/`. The name `*` writes every asset of the type. `--target` converts the asset |
| `buildzone <zone> [-use_usermaps]` | Reads `zone_source/<zone>.csv` and the files in `zonetool/<zone>/`, then writes `<game folder>/zone/<zone>.ff`. `-use_usermaps` writes the zone to `<game folder>/usermaps/<zone>/` instead |
| `generatecsv <map> [sp] [--path <dir>]` | Writes `zone_source/<map>.csv` from the files in `zonetool/<map>/` |

Without a command, `zonetool.exe` opens an interactive console that waits for input. An agent always passes a command. `zonetool.exe -h` prints every command and flag.

- `dump/`, `zonetool/`, `zonetool_paths/`, `zone_source/` and `asset_listing/` are relative to the working directory. The usual working directory is the game folder or a scratch work folder.
- A `-` flag can go anywhere on the command line. Put a `--` option where the table shows it.
- Game selection: `-game <name>`, then the game exe in the `-path` folder, else in the exe folder or the current folder (`game::binaries`), then `zonetool.json` `game`.
- Game folder: `-path <folder>`, then `zonetool.json` `paths.<game>`, then the exe folder or the current folder if it has the game exe or `zone/`, then the Steam default folder if it exists.
- `-target_path <folder>` sets the game folder of the `--target` game. Without it: `zonetool.json` `paths.<game>`, then the Steam default folder.
- IW8 has no `zone/` folder. x64-zt reads its fastfiles from the CASC storage in `Data/`, applies the `.fp` patch of each zone, and loads `oo2core_*_win64.dll` from the game folder for Oodle. Streamed images come from the `pak_*.xpak` files in CASC. If the game folder has no `Data/` but has `zone/`, x64-zt reads the loose fastfiles in `zone/`, for example a 1.41 install.
- IW8 `verifyzone` of a zone that does not exist prints `not found in CASC storage` and returns exit code 0. Read the log line, not the exit code.
- `zonetool.json` is next to `zonetool.exe`. Example: `{"game": "h1", "paths": {"h1": "path/to/mwr"}}`.
- `dumpasset` finds only assets that the zone holds, not referenced ones (`type,,name` rows in the CSV). Type names are the CSV type names. IW7 also searches the converter required zones, so `dumpasset weapon <name> <view zone> --target iw8` converts the weapon with its view model loaded.
- With `--target`, a converter loads its `required_zones` (h1_s2: `common_core_mp`, `common_mp` and others) and can also dump common-zone assets that a converted asset uses (h1_s2 `dump_used`: images, xanims, sounds with their loaded sounds). The S2 `buildzone` skips the ones S2 already ships through the `ignore` rows.
- H1 to S2 output can need S2x: a map zone cannot replace a rawfile that S2 `common_mp` already holds, so h1_s2 writes such files as `override/<name>` and S2x loads `override/<name>` first. This covers `vision/<map>.vision` when `common_mp` has the same name, and `override/animtrees/destructibles.atr` (the stock tree plus the H1 scriptable anims it lacks, since S2 looks scriptable anims up in the hardcoded `destructibles` tree). h1_s2 reads `<S2 folder>/zone/common_mp.ff` to find them. A hand-filtered `zone_source/<map>.csv` needs the `rawfile,override/...` rows.
- Flags for `dumpzone`: `-dds` writes DDS images. `-dump_streamed_image` reads `imagefile*.pak` and writes streamed images. `-castdump` writes a `.cast` file next to each xmodel and xanim; IW6, S1, H1, H2, IW7 and T9 share `src/core/assets/cast_dump.hpp`. See `docs/S2_SUPPORT.md`.
- Flag for `dumpzone --target iw7` of IW8: `-lambert` converts materials to the stock lambert techsets. Without it, the converter uses the stock PBR techsets.
- Flags for `dumpzone --target iw8` of IW7: `-max_image_size <n>` limits the longest side of a converted image to `n` pixels. `-no_gpu` compresses BC6H and BC7 images on the CPU. Without it, the converter uses a D3D11 GPU and falls back to the CPU. The GPU path is about 12 times faster.
- Flags for `buildzone`: `-more_memory` doubles the zone memory limit. `-keep_mp_traverse_nodes` keeps invalid mp traverse nodes in aipaths (H2). `-uncompressed` writes a stored IW8 zone (`\x01IWC` body), which `mw120rproxy` needs. `-copy_to <folder>` copies the built `.ff` and `.pak` to that folder, for example a mod package folder.
- The `require` row in a build CSV loads another zone into the asset database. There is no `loadzone` command.
- `buildzone` writes `<zone>.ff` and `<zone>.pak` to the `zone/` folder of the game folder. It replaces a zone of the same name. Build to a scratch folder, and ask before you write over a stock zone name in a game folder.
- `-no_cache` writes every output again and ignores the build cache.
- `-threads <n>` limits the worker threads of the zone readers, the image converters and the zlib compressor to `n`. The default is all cores. If the user needs the PC during a long dump, use a low `-threads` value.

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
src/core/formats/      csv, iwi, imagefile, mapents, ddl, cast, havok tagfile, vcdiff, dxil
src/core/assets/       asset templates that more than one game uses
src/core/io/           filesystem, game files (folder or CASC), assetmanager (binary dump format), output cache
src/games/<game>/      game.cpp, structs.hpp, <game>.hpp, assets/, zone/, dump.cpp, build.cpp
src/convert/<a>_<b>/   converter from game <a> to game <b>
src/cli/               main, commands, settings, game registry, crash handler
src/umbra_tomegen/     umbra-tomegen: generates, checks, queries and inspects Umbra tomes
deps/                  submodules; deps/premake/*.lua defines each dependency project
tools/                 build.bat, premake5.exe, asm_slice.py, asm_pack.py, livemem.py, game_window.ps1, s2_hub_to_h1.ps1
re_reference/<game>/   IDA text dumps of each game exe
docs/                  research notes and task files (local folder)
```

Per game:

- `<game>.hpp` is the game header. It holds the `<GAME>_ASSETS(x)` x-macro (type, asset class, struct), includes every asset header, and declares the database lookups.
- `game.cpp` returns the `game` definition: name, container format, and the dump, verify and build entry points.
- `zone/loader.cpp` builds the reader table from `<GAME>_ASSETS`. `dump.cpp` builds the dump switch from the same macro.
- `zone/zone.cpp` holds the `ADD_ASSET` list that `buildzone` uses.

Dependency rules:

- No include from one game into another inside `src/games/`. Code that knows two games goes in `src/convert/<a>_<b>/`. Code for all games goes in `src/core/`.
- A dependency that only one converter uses belongs to that converter. Example: the h1 gsc context lives in `convert/h1_iw7/assets/clipmap.cpp`.
- One converter cannot include another converter. If two converters need the same target builder (for example two `<a>_iw8` map converters), ask the user where the shared code goes.

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
- A `verifyzone` error such as `pointer 0x2C00000000000000 is not a valid zone offset` at asset N means that the reader of asset N, or of an asset before it, read the wrong size. Check the struct size of that type first, then the inline sub-assets that it reads.

## Compatibility rules

- Keep working v1 logic. x64-zt asset code is studied and tested since 2021. If it works, do not change it.
- Build each target asset from the source asset data. Do not copy, load or fall back to a whole donor asset of the target game. Convert techsets and shaders natively where the target engine can draw the source data.
- Donor techsets are allowed only where the user approved them, and only by name: the IW8 to IW7 converter uses a stock IW7 techset where IW7 has an equivalent (`docs/IW8_IN_IW7_FOCUS.md`). The `<a>_iw8` map converters (IW7, T9, S4) map materials to stock IW8 techsets by their features. Keep each mapping in one place in the converter, mark it `TODO`, and log it as debt. A native techset port replaces it later. Existing donor paths outside these, for example the S2 to H1 material remap, are debt: remove them, do not extend them.
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

- The same flag bit can mean a different thing in each game. Example: S4 scriptable def flag 0x2000 and IW8 flag 0x2000 do not mean the same thing. Map flags bit by bit, and prove each bit with the target asm or stock data.
- Copy a struct with `memcpy` only if both structs have the same fields in the same order. A `static_assert(sizeof(...))` checks the size only, so also compare the fields in `structs.hpp`.
- Never copy a pointer value from a source struct into a target struct. In the x86 chain (`iw3_iw4`, `iw4_iw5`), an `x86::pointer<T>` is a 32-bit offset into the x86 address space. A copied pointer still points to data of the old game, and the next stage reads it with the wrong layout. Convert the asset with the converter of its type, or make a name reference.
- An `x86::pointer` can hold only an address in the x86 region. Allocate x86 data and strings with the `x86::allocator`, for example `allocator.duplicate_string("default")`. A string literal or memory from `new`, `std::vector` or `utils::memory::allocator` is not in the region.
- A name reference is a target asset that holds only the name, for example `material_reference` in `src/convert/iw3_iw4/assets/fx.cpp`. Use it when the target dump writes only the name.
- The IW8 dump walk dumps RUNTIME arrays too. For each RUNTIME array, allocate a zero-filled array with the count that the walk uses. Do not share one allocation between two arrays, because the walk deduplicates arrays by address range.
- If the target game has a field that the source game does not have, compute it from the source data. Example: the IW5 to IW7 converter sets `numLightingValues` from the vertex count of the model. Do not read the value from a stock target asset.
- Log a lossy conversion with `ZONETOOL_WARNING` and the asset name.
- A chain converter (`iw3_iw7`) runs each stage in order. A field that one stage drops is lost for all later stages. Check that a new value reaches the final dump.
- One asset can be dumped in more than one variant (a material as a lit model material or as a sprite for a vfx). The dumper writes a name one time. If the zone lists the generic user first, the later variant is skipped. Dump shared materials after the assets that need a special variant.

Good examples to copy: `src/convert/s1_h1/` was written by humans before AI and is the reference for H1 techset work. The legacy IW5 to H1 converter on the v1 `main` branch is also human work. `src/convert/h1_s2/` and `src/convert/s2_h1/` were written with AI help and also work well. Read one of them before you start a new converter.

## Research with re_reference

`re_reference/<game>/<build>/` holds the IDA text export of one game exe. `DUMPS` in `tools/asm_slice.py` maps each dump name to its folder. `re_reference/README.md` lists the dumps.

| File | Content |
|---|---|
| `ship_asm.txt.xz` | The packed disassembly that git holds. `asm_slice.py` unpacks it |
| `ship_asm.txt` | The full disassembly with IDA names. Git ignores it. Never open it with a file tool |
| `func_index.txt` | The line in `ship_asm.txt`, the start, the end and the name of each function |
| `funcs.txt` | The start, end, size, xref count and name of each function |
| `names.txt` | Each named address and its kind |
| `strings.txt` | Each string with its address, length and xref count |
| `types.h` | The local types of the IDA database |
| `imports.txt`, `exports.txt`, `segments.txt`, `ptr_tables.txt` | Imports, exports, segments and pointer tables |

Address forms of the dumps:

| Dumps | Addresses |
|---|---|
| `h1` (1.15), `iw8`, `s4` | RVAs (image base 0). `asm_slice.py` accepts an RVA or a `0x140...` address |
| `h1_1.4`, `h2`, `iw6`, `iw7`, `s1`, `s2` | Full addresses with image base `0x140000000` |
| `t9` | Full addresses with image base `0x7ff6d7c90000`. Always give a full address, never an RVA |

- `asm_slice.py` needs `ship_asm.txt` and `func_index.txt`. Search the other files with `grep`.
- On the first use of a dump, `asm_slice.py` unpacks `ship_asm.txt.xz` to `ship_asm.txt`. It unpacks again when the `.xz` file changes. Before you use `rg`, run `python tools/asm_pack.py unpack`.
- Do not commit other export files, for example `func_index.pkl`. `asm_slice.py` does not read them.
- To add a dump, put it in `re_reference/<game>/<build>/` and add its name to `DUMPS`. Then run `python tools/asm_pack.py pack <dump>` and commit `ship_asm.txt.xz`. The pack removes the IDA instruction descriptions and keeps the line count. IW3, IW4 and IW5 have no dump yet.
- Never test `asm_pack.py` or a write script in a live dump folder. Test in the scratchpad.

Order of trust: the asm, then bytes of the exe, then stock zone data on many zones, then the hint sources. For the target side of a converter, use the target asm and the `structs.hpp` of the target game.

Research steps:

1. Start from the `Load_*` function of the asset. It gives the stream order, the struct sizes and the array strides.
2. To find the code that uses a field, search the field offset with `-r`. Keep the hits in functions that also use the struct. If a field has only one reader, that reader shows what the field does.
3. To find an unnamed function, search a dvar name or an assert text in `strings.txt`. Then search the IDA label of the string with `-r`. Example: `-r aRSmodelinstanc` finds `R_RegisterDvars`, which registers `r_smodelInstancedRenderer`.
4. Map a call stack with `-l`. List the callers of a function with `-c`.
5. Read every caller before you give a meaning to a field. One function can use the same register for a different struct.
6. Check a field meaning on many stock assets. One example is not proof.
7. Trust the code, not the name. Many IDA names are wrong, because the compiler folds identical functions into one. Names that a user added in IDA can be guesses: rely on the asm, not on the name.
8. If a dump has few names, find the same function in a game of the same engine through a shared string or constant. Example: for S2, search S1 or H1 1.4. For S4, search IW8. For T9, the BO3 and BO4 loaders have the same style.
9. Write the proof in the doc of the game before you write the code: the function, its address, the condition and the fields.
10. Keep a table of proven functions in the doc of the game: address, name and what it proves. Start the next session from that table.

Find a loader without names:

1. `Load_XAsset` (or `Load_XAssetHeader`) is a switch on the asset type. IDA names the jump table `jpt_<address>`. The case index is the asset type id. Search `"jumptable <address> case 27$"` with `-r`.
2. Each case calls the pointer loader of the type. The pointer loader tests the special pointer values, pushes a stream block and calls the stream load with the struct size as a constant. That constant is `sizeof` of the asset. Compare it with the `DB_GetXAssetTypeSize` table.
3. The body loads the sub-data in field order. `cmp qword ptr [rbx+NNh], 0` followed by a call is a pointer field at offset `NN`. A count comes from a second field. A stride comes from `imul`, `shl` or `lea [rax+rax*2]`.
4. The loader often stores the field address in a global, then calls the sub-loader. Keep a table "global, meaning" in your notes.
5. A byte pattern from a hint tool can find an anchor function in an exe without names. Read how the tool applies its patterns before you use one.

Read the asm:

- A line starts with the address in lowercase hex without `0x`, for example `140dcdba0`. A function name has uppercase hex: `sub_140DCDBA0`.
- IDA writes operand values in uppercase hex with an `h` suffix, and adds a `0` before a letter: `[rax+74h]`, `0B8h`, `[rdx+0A2h]`. Search `\+0A2h\]`, not `0xa2`.
- A comment such as `; Logical Compare` or `; Load Effective Address` is the IDA description of the instruction. Ignore it. A comment such as `; flags` or `; value` names a call argument.
- A string label starts with the first characters of the string, for example `aRSmodelinstanc`. Two strings with the same start get `_0`, `_1`. To get the label of a string, find its address in `strings.txt`, then find that address in `names.txt`.
- A multiply by a constant before an array access shows the struct size. `imul rdx, rsi, 0B8h` indexes records of 0xB8 bytes. Search the stride to find code that walks an array of that struct.
- The dvar register code stores the dvar pointer after the register call. Example: `call Dvar_RegisterBool`, then `mov cs:off_1483E9228, rax` for `r_smodelInstancedRenderer`. `-r off_1483E9228` lists every reader of that dvar.
- In a live process, ASLR moves the exe. The live address is the module base plus (IDA address - image base). For an RVA dump, add the RVA to the module base.
- T9 and S4 strings are encrypted in the exe, and `strings.txt` has few useful strings. Use structure, constants and anchor functions instead of dvar names.

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
- `-c` finds only direct `call <name>` lines. It does not find a `jmp` tail call, a call through a table or a virtual listener. For those, search the name with `-r`, the address in `ptr_tables.txt`, or the call stack of a crash log.
- A name target is a Python regex, and `asm_slice.py` prints the first 20 matches. Use `^` and `$` for one exact name.
- For a large function, print only the lines around an address with `-a -m 80`.
- `rg` with a hit limit does not load the file. Use it for a fast text search, then map the hit addresses with one `-l` call.
- The T9 asm is the largest dump. Expect several seconds for one function and 30 seconds or more for one `-r` pass.

## Add a new game

A new game is possible when its exe dump is in `re_reference/` and its zones are on disk. IW8 (Modern Warfare 2019) went from no support to a full map round trip in v2. S4 is built on the IW8 engine and uses the IW8 code as its base. T9 is a Treyarch engine, so each T9 fact needed a new proof.

Do the milestones in this order. Do not start converter work for an asset type before the reader of that type passes `verifyzone`.

| # | Milestone | Done when |
|---|---|---|
| M0 | Setup | The exe dump is in `re_reference/<game>/<build>/` and in `DUMPS`. The game folder and the zone format are known. The current build succeeds before the first edit |
| M1 | Container | `inspect` passes for the zones of one map, then `inspect --all` |
| M2 | Asset list | A histogram of the asset types of each map zone is in the doc of the game |
| M3 | Names | Hashed names resolve through a hash list. An unknown hash prints as `hash_<16 hex digits>` |
| M4 | Readers | `verifyzone` passes for the smallest zones first, then for the reference map, then `verifyzone --all` |
| M5 | Dumps | `dumpzone` of the reference map writes images, models (`-castdump`), the world, the entity data and the rawfiles. A human can open them |
| M6 | Writers | `buildzone` of the dumped reference map, then `verifyzone` of the built zone, then a second dump that is equal to the first |
| M7 | Converter | `src/convert/<game>_<target>/` builds. `dumpzone <map> --target <target>` writes the first converted types |
| M8 | Map package | The converted map builds, passes `verifyzone` in the target game and loads in the game |

Rules:

- Pick one reference map and prove each reader on it first. Then use at least 3 zones of different kinds. A reader that consumes one zone exactly by chance is not proof.
- A zone parses as one stream. `verifyzone` cannot skip an asset type that has no reader. Start M4 with the zones that hold the fewest asset types.
- Container code goes in `src/core/xfile/`. Game code goes in `src/games/<game>/`. Use `<GAME>_READ_ONLY_ASSETS` until a type has a writer.
- Do not put AES keys, RSA keys or large hash lists into tracked source files.
- Read a hint tool for facts. Write x64-zt code in x64-zt style. Do not paste files of another tool into x64-zt.
- If a milestone has a blocker, log it and do the next item that does not need it. Do not wait idle.

Games of the same engine family:

- A game that is built on a supported engine (S4 on IW8) still needs a proof for each struct. The structs are often not the same, even when most of the code is. Diff each struct against the base game with the asm.
- Script strings, bone names and other names can be encrypted in the zone. S4 encrypts all zone script strings. Spawn class names did not match, so the players spawned out of bounds. Decrypt the strings in the reader.
- Havok, shader models and image formats can change between games of one family. S4 uses Havok 2020.1 and IW8 uses Havok 2018.2. S4 has two pixel formats that IW8 does not have.

### IW9 jumpstart

IW9 (Modern Warfare II, 2022) is the next large iteration of the IW engine. The route to IW8 to IW9 conversion is:

1. Add the IW9 exe dump to `re_reference/iw9/<build>/` and to `DUMPS`.
2. Do M1 to M6 for IW9 with one multiplayer map. Get the map to dump and rebuild completely, with every asset type, the same as IW8 `mp_rust`.
3. Only then start `src/convert/iw8_iw9/`. Diff each IW9 struct against `src/games/iw8/structs.hpp` with the asm.
4. Expect what S4 and T9 showed: hashed names, encrypted strings, DXIL shaders, a newer Havok and new pixel formats.

Do not start the converter before IW9 round trips. A converter on an unproven reader converts wrong data.

## Maintain a game

- After each change to a reader, a writer or the container, run the checks in "Verification".
- A zone that passed before and fails now is a regression. Fix it before new work.
- When a game updates its exe, make a new dump in `re_reference/<game>/<build>/`. Keep the old dump while users still play the old build.
- When `verifyzone --all` gives a new count, update the table in "Verification" and say why the count changed.
- Fix a bug of one game in that game. Do not change a shared template in `src/core/assets/` without a `verifyzone --all` of each game that uses it.

## Port a map

A map port is `dumpzone <map> --target <game>` (or `dumpmap`), then `buildzone`, then a package for the target client. Port one map from start to end before you start a second map.

1. Dump the source map with `--target`. Read the log. Each `ZONETOOL_WARNING` is a lossy conversion.
2. Check the CSV. Each row must use a type name of the target game. A source type name (for example `col_map_mp` in an IW7 build) leaves the zone without that asset, and the game crashes at map load.
3. Keep the `rename` rows when the map id changes, for example `mp_killhouse` to `mp_killhouse_iw`. The game looks up `maps/mp/<id>.d3dbsp`. Without the rows, the map entities are not found and the game crashes in `Scr_ResetEntityParsePoint`.
4. Build. Run `verifyzone` of each built zone.
5. Package the zones for the client. Back up the old package before you copy a new one.
6. Give the user a test list.

Clients and packages:

| Target | Package |
|---|---|
| H1 | `buildzone <map> -use_usermaps`, then `h1-mod.exe -noupdate -multiplayer +devmap <map>` |
| IW7 | iw7-mod does not load maps from `usermaps/`. Copy the zone to the `zone/` folder of the IW7 folder |
| IW8 1.20 | The replay client (`mw120rproxy`) loads `mods/mw120r/maps/<id>/`: the five zones `<map>`, `techsets_<map>`, `eng_<map>`, `ww_<map>`, `srv_<map>` and `map.json`. Build the zones with `-uncompressed` |
| S2 | S2x. Press PLAY first, then `map <name>`. Do not use `devmap` |

Map pieces. Check each one against a stock map of the target game:

| Piece | Asset | Check |
|---|---|---|
| World geometry | `gfx_map` | Surfaces, cells, static models, brush models, Umbra tome, light grid, lightmaps, reflection probes |
| Lights | `com_map`, `gfx_map` | Light count and order must be the same in both assets. GfxWorld refers to the ComWorld primary lights by index |
| Collision | `col_map` | IW8 makes the world body only from shape slot 0. Data in collision tiles loads only through transient streaming. Merge it into slot 0 |
| Entities | `map_ents` | Entity string, spawn list, triggers, client triggers, splines, submodels, edge lists |
| Ladders, mantle | `map_ents` edge lists | Without edge lists, ladders do not work and the mantle prompt shows at the wrong places |
| Scriptables, dynents | `map_ents`, scriptable defs | See "Map events and scripts" |
| Effects | `fx_map`, vfx, map GSC | See "Map events and scripts" |
| Net const strings | `netconststrings` | IW7 and IW8 need the `ncs_*_level` tables, or the map does not load |
| Vision, grading | `rawfile` vision, grading CLUT | Exposure, fog and tonemap keys. See "Render bugs" |
| Minimap | compass material and image | Map script calls `setupminimap` |
| Bots | `aipaths`, navmesh | Bot warfare waypoints can give path data where the source map has none |
| Sound | sound bank and its `.sabs` | The bank does not load without the `.sabs` file |
| Height limits | world bounds | Killstreak height comes from the top of the world bounds. Sky geometry in the bounds puts killstreaks too high |

The target renderer can read some world images without a null check, for example the reflection probe array and the IES lookup image. A missing image there crashes the game. Write each world image that a stock map of the target game has.

## Map events and scripts

Map events are scriptables, dynents, createfx, the map GSC and the other scripts that the map starts. Each engine stores them in a different way. Find each one in the source dump, then find where a stock map of the target game keeps the same thing.

Map GSC:

- The map script is `scripts/mp/maps/<map>/<map>.gsc` (IW7, IW8) or `maps/mp/<map>.gsc` (older games). It calls the load script of the game and `setupminimap`.
- A converter writes the target map script from the source map data. Do not convert script bytecode between engines. For T9 to IW8, the user writes the map script by hand.
- Source scripts that set unknown dvar hashes or call source-only functions (lighting, art and precache scripts) have no proven target mapping. Document them in the task file. Do not convert them blindly.
- Decompile GSC with gsc-tool when you need the source text. Its token tables also map entity keys.
- A weapon that only a converted map uses must be registered at level init. In IW7, a `/give` of a weapon that was not precached is silent. Register it with `precacheitem` in an init script.

Createfx:

- Older maps place effects with `loadfx` lines and `createoneshoteffect` entities in the map script or in `<map>_fx.gsc` and `gen/<map>_fx.gsc`. Each entity has `ent.v["origin"]`, `ent.v["angles"]`, `ent.v["fxid"]` and `ent.v["delay"]`.
- IW8 keeps placed effects in `MapEnts.clientSideEffects`. A converter that reads only the stock rawfile form finds no effects in a map that writes them in `main()`. Read every form that the source map uses.
- After the effect plays, check its material. A vfx needs the sprite variant of its material. A lit model variant draws nothing or the wrong thing. A particle that the source lights on the GPU needs a lit sprite techset, or it draws at full brightness.

Scriptables and dynents:

- Scriptable defs have flags at the def, part, state and event levels. Compare each level with stock defs of the target game before you copy a flag.
- In IW8, a scriptable instance without an entity draws only through a dynent. S4 marks such defs with 0x2000 and draws them with its own model event. A converted def that keeps 0x2000 and has no dynent draws nothing, but its collision works.
- Map scriptables need their anims in the animtree that the game uses. Some games look up scriptable anims in a hardcoded tree (`destructibles`).
- A moving brush (`script_brushmodel`) needs its shape in the entity shape list and a physics asset, or it has no collision. If only its shadow shows, look at the culling of the brush model.
- Inline weapons in scriptable explosion events use the weapon layout of the source game. The reader needs the full source weapon tree.

Entity string:

- IW7 and IW8 entity strings use token ids as keys. Map each source id to a name with the gsc-tool token table of the source game, then to an id with the table of the target game.
- A key that has no id in the target table is dropped. Log the count for each map.
- A sparse token table can drop keys that the game needs, for example `radius` of `trigger_radius`. Check the warnings for keys that stock maps use.

## Port a weapon

A weapon port is the weapon def, the view and world models, the anim package and its xanims, the sounds, the effects, the sight and its reticles. Port one weapon from start to end before you start a second weapon. Start with a pistol: it has no sight and few anims. Then port a rifle, then a weapon with a sight.

The public guides "Weapon porting IW7 → IW8" and "Weapon porting H1 → S2" are linked in `README.md`. The task logs are `docs/IW8_SUPPORT.md` (IW7 to IW8), `docs/IW8_IN_IW7_WEAPONS.md` (IW8 to IW7), `docs/S2_WEAPONS.md` and `docs/H1_WEAPON_IN_S2.md` (H1 to S2).

Routes:

| Pair | Source zones | Command |
|---|---|---|
| IW7 to IW8 | Def in `global_core_mp`, anims in `common_core_mp`, models in `mp_weapon_view_<family>_tr` and `mp_weapon_world_<family>_tr` | `dumpasset weapon <weapon> mp_weapon_view_<family>_tr --target iw8`. The converter also loads the world zone and finds the def in `global_core_mp` |
| IW8 to IW7 | Def in `global_stream_mp`, parts in transient attachment zones | `dumpasset weapon <weapon> global_stream_mp --target iw7` |
| H1 to S2 | Def in `common_mp`, models in `mp_vm_<name>_tr` and `mp_wm_<name>_tr` | `dumpasset weapon <weapon> common_mp --target s2`, then `dumpzone` of the two model zones with `--target s2`. Merge the three dumps |

`<family>` is the weapon name without `_mp`. If IW7 has no view zone of that name, remove 1 or 2 characters from the end: `iw7_cheytacc` uses `mp_weapon_view_iw7_cheytac_tr`.

### Quick port weapon

The x64-zt-gui repository has `Tools > Quick port weapon`. It ports one IW7 multiplayer weapon to an IW8 package for the replay client in one job: dump, convert, package, `buildzone`, `verifyzone` and install. `docs/quick-port-weapon.md` of that repository gives each step. Use it for IW7 to IW8 when the user has the GUI. For other pairs, do the same steps by hand.

The IW8 package for the replay client:

- `<base>` is `iw8_cw_<name>`, and the weapon asset is `<base>_mp`. `iw7_cheytacc_mp` gives `iw8_cw_cheytacc`.
- The package is 8 stored zones (`-uncompressed`): `<base>` and `<base>_common`, each with the prefixes none, `techsets_`, `ww_` and `eng_`. The 6 prefixed zones are empty.
- `<base>.csv` has every row except `xanim`, `animpkg` and `netconststrings`, plus `rename,<weapon>,<base>_mp`.
- `<base>_common.csv` has the `xanim` and `animpkg` rows, plus `addpath,zonetool\<base>`. Without the `addpath` row, the zone holds references only.
- Drop the `netconststrings` rows. A weapon zone must not replace the `ncs_*_level` tables of a map.
- `<base>.weapon.json` names a stock `reference` weapon of the same class and a free loadout slot (61 to 254).
- Copy the `.sabs` file of the sound bank to `mods/mw120r/weapons/`.
- Remove an empty `zone/` folder from the IW8 folder after the build. A `zone/` folder in a CASC install stops the game boot.

### Weapon rules

- Start from the def field list. Copy each field that has the same name and type in both games. Map each enum with a table.
- A target field that the source does not have gets a value from a survey of all stock target weapons of the same class, not from one weapon. Examples: the ADS DOF values came from 167 stock IW8 weapons, and `fViewMaxPitch` and `fViewMaxYaw` are 10 in all 52 stock IW8 weapons with the legacy view kick.
- The game registers weapon names only during the level init. A `give` of a weapon that no script uses does nothing. In IW7, register it with `precacheitem` in an init script (`custom_scripts/mp/<name>.gsc` in iw7-mod).
- The target game can copy anim package arrays without a null check, for example the IW8 `blendSpaces` and `footstep` arrays. Write zero-filled arrays when the source has no data.
- A sound bank needs its `.sabs` file. Without it the whole bank fails (`sound bank failed to load`), and the weapon plays no sound. A bank with no streamed assets is an empty `.sabs` file of 0x1000 bytes.
- Name the sound bank after the weapon (`<weapon>_weapons.all`), not after the zone. Two weapons with the same bank name replace each other.
- Keep the dump folder path short. Windows MAX_PATH (260 characters) stops the write of streamed image files, and the gun is invisible in the game.
- Count the materials that a view model material rule matches before you build. Almost every IW7 view model techset has the `svp` token, so an `svp` rule matches every camo body.

### Weapon symptoms

| Symptom | Proven cause |
|---|---|
| `give` does nothing | The weapon is not registered during the level init |
| Gun too low, stretched geometry, flat anims | The view anims use the bind pose of the source rig. IW8 view anim translations add to the `viewhands_base_iw8` bind pose. IW7 view anims hold the full local translation on a rig with zero bind translations |
| Gun not attached to the hands | IW7 attaches the gun to `tag_weapon_right`, IW8 to `j_gun`. Copy the tracks to the bone of the target |
| Invisible gun in IW7 | The IW8 gun model has 1 bone and no surfaces. IW8 builds the view gun from the attachment parts. Merge the base parts into one IW7 gun model |
| View model flickers | The tri cluster bounds were written as mins and maxs. IW8 stores a center and a half size. Or `XModelLodInfo.partBits` is 0: copy it from `XModelSurfs.partBits` |
| Parts stretch to a rod, a bullet floats | IW7 does not compute client bones and computes only the bones that the surface `partBits` request. Write the merged bones as regular bones. Build the surface `partBits` again from the vertex bones |
| Fire anim cut, snaps to idle | `fireAnimTimesMs` was 0. Use the length of each fire anim |
| No reload, raise or melee sound | The sound notetracks have a different form. IW8 plays `ps_<alias>` notetracks and `melee_attack_sfx`. IW7 needs `notetrackSoundMap` rows |
| Sounds too loud | The source base volmods (0.5 for IW7 weapon sounds) were not applied to the alias volumes |
| No bullet impacts | `vfxImpactType` was 0, which is `none` |
| Muzzle light too bright | The light units differ. IW7 omni light intensity is about 56 times the IW8 intensity (survey of 529 IW7 and 224 IW8 light states) |
| Muzzle flash too bright | An exposure compensated (`evc`) effect material got a techset without exposure compensation |
| Tracer is a thick line that stops abruptly | A blend tracer material with a fade got an additive techset. Additive ignores the alpha |
| Black weapon body | A material rule for one special case (a scope lens) matched every camo material |
| Map lights show through walls with one weapon | A view model blend rule was too wide and gave glass an effect techset |
| No reticle | The reticle is a LUI widget on `WeaponAttachment.reticles`. `WeaponDef` has no reticle. The weapon needs a sight attachment, target widget names and `lockReticleToTag` 1 (all 322 stock IW8 reticles) |
| Gun or scope hidden in ADS | The dual FOV flags come from the sight of the attachment first, then from `WeaponDef` |
| Scope ring missing in ADS | The ring is closer to the eye than `r_nearPlaneFadeDistance` (3.0). The ADS align offset is not set |

Open in IW7 to IW8, see the newest entries of `docs/IW8_SUPPORT.md`:

- The replay client holds the converted weapons without attachments, so the default sight is not equipped and no reticle shows.
- The left hand eases back to the grip after an anim. Candidate causes: the anims do not animate `tag_ik_loc_le` and `tag_ik_loc_ri`, or `ikType` differs from stock.

H1 to S2:

- The H1 and S2 arm rigs differ. The converter retargets the H1 view anims to the S2 arm rig, and the weapon keeps the S2 hands.
- H1 has one damage step between `midDamage` and `minDamage`. S2 has two steps. Map them so that the S2 falloff gives the H1 values.
- H1 attachments change anims through their fields. S2 attachments do not. Write each H1 attachment anim as a weapon `animOverrides` entry.
- H1 type 9 fields are seconds, and S2 type 9 fields are milliseconds.

## Render bugs

Start each render bug with a control. Use a stock map of the target game, and dump the stock value before you guess a value.

Order of work:

1. Fix lost data first: missing models, layers, masks, images or techniques.
2. Then fix wrong data: formats, constants, flags, units.
3. Change the light level, the tonemap, the tint or the exposure only after steps 1 and 2.

What to expect for the direction of the port:

- A forward port (an older game to a newer engine) has the data, but the newer renderer reads it in a different way. Expect broken lights, effect techsets that do not draw, light intensity that is too low or too high, and exposure differences. Light units and tonemap curves differ between engines.
- A backport or a cross-engine port (a newer or a different engine to an older one) can lack data that the target techset reads. Expect white or black textures, lost layers, textures cut off, and missing normal or specular maps.

Rules:

- A dvar test finds the render path. It is not the fix. A map must work with the default dvars of the game.
- Ask the user to test one dvar at a time. A dvar test is valid only if the dvar exists. Some clients print nothing for an unknown dvar.
- A control build with one changed value gives a clearer result than a dvar test.
- To test an input, replace it with a constant. Do not scale it: `0 * NaN` is NaN, and a clamp can hide a scale.
- A finite value is not a correct value. A check that the data is valid does not prove the scale.
- Prove which draw is wrong with the capture or the geometry, not by inference. A guess at the wrong material wastes a test round.
- Do not edit render data in live memory. The renderer copies world data at map load. Read live memory only to examine values.
- A live process uses ASLR, so IDA addresses of globals do not apply. Find a world asset through a pointer to its name string.
- Measure frame rate without RenderDoc. The capture layer changes the frame time.

Culling and draw paths:

- If a model appears and disappears when the camera moves, the bug is in the culling or in the draw path, not in the material. Check the Umbra tome, the cell and portal data, `sortedSurfIndex` and the LOD distances.
- In IW7, `r_umbra 0` turns off umbra culling and `r_smodelInstancedRenderer 0` turns off the instanced static model path.
- IW7 draws a static model rigid, not instanced, if one of these is true (`sub_140DCDBA0`): flag 0x400, a SUBDIV LOD, `numLightingValues` is not 0, or `lightmapIndex` is not -1.
- Count the assets that a change affects before you build. Example: `>= CAMERA_REGION_EMISSIVE` made 661 static models rigid. `== CAMERA_REGION_EMISSIVE` made only the 18 lamp models rigid.

Proven causes from past ports. Check these first when you see the symptom:

| Symptom | Proven cause |
|---|---|
| White surfaces | The target techset reads a slot that the source material does not fill, and the slot gets `$white` |
| Green or black background on tree cards, leaves cut off | An alpha-test material got an opaque techset, and its opacity image was dropped |
| Sign text, decals and wall strips missing | A layered world material lost its layers 1 and up |
| Decal volumes missing | The image decal atlas index was 0, and the image format was not one that the atlas accepts |
| Black sky | A sky material was dumped under the name of a different variant, or a shader constant of the sky was not mapped |
| Black weapon | A material match for one special case (a scope lens) matched every camo material |
| All lights visible through walls with one weapon | A view model blend match was too wide |
| Effect too bright | A GPU-lit particle got an unlit sprite techset |
| Effect missing | The effect material was dumped as a lit model variant, not as a sprite |
| White minimap | The 2D material lookup used the wrong texture semantic |
| Map too bright or too dark | Vision keys (EV compensation, fog intensity, tonemap mode, exposure adjust) differ in unit or default between engines |
| Lightmap too bright | The lightmap was not converted to linear |
| Blocky sun shadows | The baked sun channel was used where the target uses a screen-space sun shadow mask |
| Reflections too strong | A shader constant suppressed F0 and metalness in the source, and the target constant had a different meaning |
| Static models invisible | The converted model surfaces were empty |
| Static models black | The target techset needs probe or light grid data that the map does not have |
| Model looks like loose shards | A per-model position scale field of the source was ignored |
| Fatal error in `Create2DTexture` | The target has no mip size case for the converted DXGI format |

## Materials and techsets

The techset decides most of how a surface draws. Study the material and its techset before you change light values.

A material has: a techset, a texture table (slot, semantic, name), a constant table, state bits for each technique, a camera region, a sort key, game flags and a material type. Each of them can be wrong after a conversion.

Study a material:

1. Dump it: `dumpasset material <name> <zone>`. The techset has a readable technique list in `dump/<zone>/techsets/<techset>.techset.txt`. The state data of each material is in `techsets/state/<techset>/<material>.statebits`, `.statebitsmap` and `.stateinfo`.
2. Find the technique that the draw path selects. A draw path can add an offset to the technique index. In IW7, the instanced static model path adds 0x26. A techset without that technique draws nothing for that path.
3. Dump the pixel shader of that technique. Find which textures (`dcl_resource t<n>`) and which constants it reads. Check that the material fills each texture slot. An empty slot gets a default image (`$white`, `$black`, `$identitynormalmap`).
4. Map each source constant to a target constant by a proof in both shaders. Do not map a constant by its index.
5. Check the state bits: blend, alpha test, depth write and cull.
6. Check the camera region and the sort key. A wrong camera region puts the surface in a different draw list. A wrong sort key draws a decal under its surface or a transparent surface in the wrong order.
7. Check the game flags and the material type. Compare them with a stock material of the same kind.
8. Compare the result with a stock material of the same kind, field by field.

Shaders:

- IW7 and IW8 1.20 shaders are DXBC (shader model 5). Disassemble them with `fxc /dumpbin`. `dxc` cannot read them.
- T9 shaders can be DXIL. Disassemble them with `dxc -dumpbin`. `src/core/formats/dxil.cpp` reads the constant buffer reflection.
- The IW7 shader files are in `dump/assets/techsets/ps/<hash>.cso`. Shaders that a map techset references can be in a common zone. Dump them from that zone, for example `dumpasset pixelshader <hash> code_post_gfx`.
- Search disassembly output with `grep -a`, because the files can hold binary bytes.

## RenderDoc

RenderDoc captures one frame with every draw, texture, constant and shader. It lets an agent check the renderer from data, not from a picture. Recommend it to the user for each render bug.

The user:

1. Installs RenderDoc.
2. Starts the game through RenderDoc, or through a client that loads the RenderDoc in-app API. The IW8 replay client can do this, and F12 or PrintScreen saves a capture.
3. Goes to the spot, captures one frame, and gives you the `.rdc` path and the `setviewpos`.
4. Captures the same spot in the source game when the source game runs on D3D11 or D3D12.

The agent:

- Writes a Python script for the RenderDoc API and runs it in `qrenderdoc`. Write the script with a file tool, not with a shell heredoc. Backslashes in a heredoc break the script.
- Lists the draws. Finds the draw of a material by the DXBC checksum of its shader (bytes 4 to 20 of the bytecode). Searches the `.cso` files of the dump for it.
- Reads the bound textures and the constant buffers of the draw.
- Uses pixel history at a chosen pixel. Decode the frame to an image first, and check that the pixel is the surface that you study.
- Compares the same draw in both games.
- Closes each `qrenderdoc` process that the script started.

Limits:

- `GetUsage` misses bindless texture use in D3D12 captures. Read the root signature and the descriptors instead.
- A D3D12 capture can have no GPU timings.
- Shaders in a capture can have no constant buffer names. Use the shader disassembly of the dump with the capture.

## Images

Check these for each game and each converter:

- The format map: each source format to a target format that the target engine can create. A format without a mip size case in the target crashes the game at load.
- The mip count and the size. The warning `does not account for <n> bytes` means that the pixel data size does not match the width, height, format and mip count.
- sRGB or linear: color images are sRGB, normal maps, masks and lightmaps are linear. A wrong flag gives a surface that is too bright or too dark.
- Normal, gloss, occlusion and specular maps. Engines pack them in a different way. A merged or split map with the wrong channels gives flat or shiny surfaces.
- Decal images: some engines need an atlas index and an atlas format in the image header.
- The image category and the header bytes after it. Copy them from the source when the stock data proves the same meaning.
- Very small images: a 1x1 BC7 image can crash a renderer, and a decal image with a side less than 8 pixels can be rejected.
- Streamed images: the dump needs `-dump_streamed_image`, and the build needs the `.pak` or the target stream files.
- Reflection probes: the probe array image is often generated. A probe array copied from an older game can look pixelated. iw7-mod Debug can render new probes in the game (see "Clients and tools").

## Debug a bad conversion

Use these methods in this order:

1. Dump the converted asset and the stock asset of the same kind. Compare them field by field with a scratch script.
2. Survey the source data. Count the assets that have the value that you change.
3. Decode the converted data with a scratch script, and compare it with the source decode. Example: decode the packed vertex positions of a converted surface and compare their bounds with the source bounds.
4. Build a control: a package with one changed value. The user tests it.
5. Bisect: build packages that each leave out one half of the suspects, for example half of the materials.
6. Run the converter on one asset with `dumpasset ... --target <game>`. It is faster than a full map dump.
7. Compare with the last good output before each report.

The fault can be in the build, not in the converter. Check the dumped files first. If the dump is correct, the bug is in `parse()`, in a CSV row or in the `zonetool_paths` merge.

## Asset hunts

Start from the symptom. Then check the asset that it points to.

XModel and XSurface:

- Take each struct size from `structs.hpp` and the `Load_*` code. A script with a wrong size reads the first surface correctly and the rest as garbage.
- IW7 `XSurface.vertexLightingIndex` is a running sum of the vertex counts of all surfaces in all LODs of the model. Do not reset it for each LOD.
- In a chain, check that each surface has its converted material in the final stage.
- Skinned models that a map uses as static models need their static copies in the world dump.

GfxWorld and ComWorld:

- GfxWorld refers to the ComWorld primary lights by index. Keep the count and the order of the primary lights the same in both assets.
- Compare the light count, the light grid and the probes with a stock map of the same size.

Weapons: see "Port a weapon".

Sound:

- Map ambient sound comes from the map sound bank. A converted bank needs its `.sabs` file next to it.
- Footsteps come from the surface types of the materials and the footstep tables of the game.

Other assets:

- Find the `Load_*` function, then the code that reads the asset at run time. Compare a converted asset with a stock asset of the same kind, field by field.

Scripts for dump files:

- To count or compare dumped binary files, write a small Python script in the scratchpad. Take each struct size and offset from `structs.hpp`.
- Print counts and a short list, not every record.
- Keep the script out of the repository, unless the user wants a new tool in `tools/`.
- Write the script to a file and run the file. On Windows, give Python paths in the `C:/...` form, not `/c/...`. `python -I` ignores `PYTHONIOENCODING`, so call `sys.stdout.reconfigure(encoding="utf-8")` in the script.
- Do not name a script after a standard library module.

## Clients and tools

| Tool | Use |
|---|---|
| h1-mod | H1 client (H1 1.4 binary). The Debug build adds in-game asset viewers for gfxworld, comworld, mapents, material, techset, shader, xmodel, weapon and more |
| iw7-mod | IW7 client. The Debug build adds the asset list GUI, a navmesh debug view, asset dumps and a reflection probe generator (`r_reflectionProbeGenerate 1`). Crashes write a minidump with `info.txt` |
| IW8 1.20 replay client (`mw120rproxy`) | Loads `mw120r` map packages. Writes crash logs and engine logs. Loads the RenderDoc in-app API |
| S2x | S2 client. Loads `override/<name>` rawfiles first |
| RenderDoc | Frame captures. See "RenderDoc" |
| `fxc`, `dxc` | Shader disassembly (DXBC and DXIL) |
| gsc-tool | GSC decompile and token tables |
| OpenAssetTools, atian-cod-tools, OpenIW8 headers, v1 zonetool | Hint sources |
| `tools/livemem.py` | Reads the memory of a live process |
| `tools/game_window.ps1` | Screenshots and input for the game window |
| x64-zt-gui | Desktop front end for zonetool. `Tools > Quick port weapon` ports an IW7 weapon to an IW8 replay client package |

Ask the user to build a Debug client when you need its tools. Ask the users of the client projects for help when a client feature is missing. The community is in the Aurora Discord server that `README.md` links.

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
| IW3 | No full count yet (read only) | none |
| IW5 | 69 of 228 (read only, see `docs/X86_GAMES_AS_X64_X64ZT_2.md`) | none |
| IW6 | 878 of 878 | `mp_battery3` |
| IW7 | 696 of 697 (`mp_vlobby_room` on disk is a modified file) | `mp_frontend`, `mp_afghan` |
| IW8 | 9471 of 9494 (`code_post_gfx` and old frontend zones fail, see `docs/IW8_SUPPORT.md`) | `mp_rust`, `srv_mp_rust`, `techsets_mp_rust` |
| S1 | 3058 of 3058 | `mp_prison` |
| S2 | 268 of 268 | `common`, `mp_aachen_v2` |
| T9 | No full count yet | none |

The total changes when the zone folder changes. A zone that passed before and fails now is a regression.

`buildzone` and `verifyzone` prove the file, not the picture. A map port also needs a load in the game and a test by the user.

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
- Name an unknown field `unknown<offset>` until the asm proves its meaning. Do not name a field from one example.
- Do not leave debug code, test switches or temporary output in a pull request.

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

`CONTRIBUTING.md` has the steps for a new contributor. These rules apply to each pull request:

- Keep one topic in each pull request: one game, one converter or one asset type. Do not mix a refactor with a fix.
- Change only the lines that the fix needs. Do not reformat other lines. Keep the line endings of each file, and run `git diff --check`.
- For each layout change, give the proof in the description: the `Load_*` function, its address and the game build.
- Give each check that you ran and its result: build, `verifyzone` count, round trip, test in the game. Report the build result and the game result separately.
- List each check that you did not run as "Not verified".
- Do not commit machine paths, user names, drive letters, dumps, built zones or game files.
- Put research notes in the pull request description or in `docs/`, not in code comments.
- Update this file when you add a command, a flag, a game, a dump or a verification count.

## Agent playbook

This section is for agents that work alone for a long time, for example during a map port.

### Work loop

1. Find the code with the "Source layout" table and one search. Do not list the repository or search a whole drive.
2. Read the function, 100 to 200 lines from the search hit. Never open a large file from line 1.
3. Write the cause in one sentence before you edit. If you cannot, get more evidence first.
4. Make the smallest edit that fixes the cause. Copy the style of the code around it.
5. Build once. Fix the first compile error first.
6. Run the checks in "Verification" that apply to the change.
7. Reply with the build state, the changes as short bullets, and what you did not verify.

### Speed

| Step | Time | What to do |
|---|---|---|
| One `Get-Content` of `ship_asm.txt` (1.5M lines) | about 30 s, many times | `python tools/asm_slice.py <dump> <address or name regex>` takes less than 1 s |
| Zone reads from a slow disk | about 20 min of a map dump | Start the dump early, in the background |
| Streamed images | about 10 min of a map dump | Keep `dump/<zone>/.zonetool_cache`. The next dump skips the files |
| Material conversion | about 30 min of a map dump | Run the dump once. Fix build-side bugs without a new dump |
| `gfx_map` dump of an IW7 map to IW8 | about 19 min | Dump only the asset types that the change touches, with `dumpasset` |
| `buildzone` of a large map | about 6 min | Run it in the background |
| Full rebuild after a new `--games` set | more than 10 min | Keep the same set |

- Run long commands in the background and redirect output with `*> file.log`. `Tee-Object` holds the output, so you see no progress.
- A harness can stop a foreground shell command after about 2 minutes. Run a longer job in the background and wait for its notification. Do not poll in a loop.
- Compare hex addresses as integers, not as strings.
- Before a long dump, check that the exe that you run is the one that you just built (size and time).
- Batch reads. List every function that you need, then read them all in one step.
- Do not run a dump and a build of the same zone at the same time.
- Do not hash or compare large package files when a size and time check is enough.

### Search recipes

```bash
python tools/asm_slice.py h1_1.4 -l 14060B312 1406060E9 140605A69
python tools/asm_slice.py h1_1.4 14060B312 -a -m 80
python tools/asm_slice.py h1 "Load_Material$"
python tools/asm_slice.py s2 -c Load_MaterialTechniqueSet
python tools/asm_slice.py iw8 Load_GfxWorld -m 400
python tools/asm_slice.py iw7 -r "aRSmodelinstanc"
python tools/asm_slice.py iw7 -r "\+74h\]"
python tools/asm_slice.py t9 -l 7ff6d8072b64
grep -n "struct GfxStateBits" -A12 src/games/h1/structs.hpp
grep -n "r_smodelInstancedRenderer" re_reference/iw7/ship/strings.txt
```

- Dump names: `h1` (1.15, RVAs), `h1_1.4` (full addresses), `h2`, `iw6`, `iw7`, `iw8`, `s1`, `s2`, `s4`, `t9`.
- `-l` maps a call stack to function names. `-a` prints the lines around an address. `-c` lists every call site of a function.
- `-r` searches the text of each line with a regex, not an address. Use it for a string label, a global or a field offset.
- The build log names each asset that it parses. Search the log for the asset name.
- A zonetool log from a Windows PowerShell 5.1 redirect is UTF-16, and `grep` finds nothing in it. Check with `file <log>`, then convert with `iconv -f UTF-16LE -t UTF-8 <log>`.
- `rg` can miss a line in a CRLF file when the pattern ends with `$`. Use `--crlf`.

### Theories

1. Write 3 candidate causes, not 1. For each candidate, write what it predicts that the others do not.
2. Do the cheapest check that can remove a candidate first.
3. Write the cause as a chain. Give each link its proof: a register, a memory value, a log line or a code line (`file:line`). A link without proof is a guess.
4. A theory must explain every observation. Example: two runs crash at the same address with the same registers, so a timing theory is wrong.
5. Write the result that you expect before you test in the game. If the result is different, the theory is wrong. Remove the first fix before you try a second fix.
6. After three null results in one line of tests, stop that line. Go back to the list of inputs and bisect.
7. Keep a "Ruled out" list in the task file. Do not test an item on it again without a new reason. A test result is weak when the test itself was saturated or broken.

### A game crash

1. Read the crash data of the client:
   - h1-mod and iw7-mod: `<game folder>/minidumps/<newest>/info.txt` gives the exception address, registers, call stack and last zone. The `.dmp` file is in the same folder.
   - IW8 replay client: the `mw120rproxy.crash.*.log` files give the RVA and the call stack.
   - If there is no crash file, look for a Windows process dump in `%LOCALAPPDATA%\CrashDumps`.
2. Map the call stack with `python tools/asm_slice.py <dump> -l <addresses>`. Use the dump of the exe that crashed (for iw7-mod, the module with base 0x140000000 is `iw7_ship.exe`).
3. Read the faulting instruction with `-a`. Write which register holds which struct and field.
4. A minidump holds the stack, but not the heap or the zone memory. While the crash dialog is open, the process is alive. Read its memory with `tools/livemem.py`:
   - `python tools/livemem.py <pid> read <address> 40` prints 0x40 bytes.
   - `python tools/livemem.py <pid> refs <address> --stride 28 --field 140` finds the struct that owns an array that reaches `<address>`. This example finds the H1 `Material` whose `stateBitsTable` holds a `GfxStateBits` (0x28 bytes).
   - `python tools/livemem.py <pid> h1-material <address>` lists the techniques that have no valid state bits.
5. Find the asset in `dump/<zone>/` and in the build log. Then find the code that made the bad value.
6. Close the crashed process only after you have the evidence.

Known crashes:

- H1 1.4 `sub_14060B200` reads `GfxStateBits.zone` from `stateBitsTable[stateBitsEntry[techType] + pass]`. If a technique has `stateBitsEntry` 0xFF, the game reads entry 255 and crashes. `buildzone` logs this case as `has no state bits for technique`.
- A null map entities or clip map pointer in `Scr_ResetEntityParsePoint` at map load: the zone has no asset with the name that the game looks up (`maps/mp/<map>.d3dbsp`). Check the `rename` rows and the CSV type names.
- A null image in the renderer at map load: a world image that the target renderer reads without a null check is missing. Compare the world images with a stock map.
- A Havok out-of-memory error at load: the collision tree forces a rebuild at load. Write the tree in the form that the stock data uses.

### Test in the game

Ask the user before you start the game or send input to its window. Many users test by hand. Then give the user a short list for each test: a spot (a `setviewpos` line or a landmark) and the thing to look for.

A visual feature works only after a game screenshot or a report from the user shows it. `buildzone` and `verifyzone` prove the file, not the picture. A debug output in a shader is allowed for one test build. Remove it before the next build.

1. Build the zone and package it for the client (see "Port a map").
2. Start the game from the game folder, for example `h1-mod.exe -noupdate -multiplayer +devmap <map>`. `devmap` enables cheats. Most Call of Duty clients have `map` and `devmap`.
3. If the user asks you to control the game window, use `tools/game_window.ps1`. `-process_name <name>` selects a process other than `h1-mod`.
   - `shot <file.jpg> [scale]` saves the client area of the game window only.
   - `click <x> <y>` clicks at client coordinates. Divide the coordinates of a scaled shot by the scale.
   - `key tilde` opens the console, `type "<text>"` types text and `key enter` runs it. Check with a shot that the console is open before you type. Without the console, a key can open the chat.
   - Stop all input when the user plays. The user can move during your test.
4. Select "Auto-assign" and a class. Then move with `setviewpos <x> <y> <z> <yaw> <pitch>`.
5. After each move, take one shot and look for a new crash file.

### Known mistakes

- A search of a whole drive or a recursive listing of the game folder or a scratch folder. The output fills your context, and the search takes minutes.
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
- A claim from truncated output. `head -20` of a long list made an agent report zones as missing. Read the full count before you report a missing item.
- A copy command with a placeholder path such as `<IW8>`. The copy fails, and the user tests an old package.
- A `sed -i` on a CRLF file. It can remove the CR characters. Check the line endings after each edit.
- A Python heredoc with backslashes in a shell. Write the script with a file tool and run the file.

### When the build fails

1. Find the first `error C` or `error LNK` line, and fix that error first.
2. If the same error comes back, read 30 lines around it again. Do not try the same edit again.
3. After three failed tries, stop. Report the exact error line and what you tried.

### When you are stuck

Say what you searched, what you found and what is missing. Ask one specific question. Do not fill the gap with a guess.

### Checklist before you reply

1. The build of this run succeeded, and the exe that you ran is the new one.
2. `verifyzone` passes for each zone that you built.
3. `git status --short` lists only the files that you meant to change.
4. `git diff` has no unrelated edits, no reformatting and no debug code.
5. The task file has a short dated entry with the result.
6. Nothing is committed, unless the user asked for a commit.

### Reply format

```
Built and copied. Zone built to usermaps. Tested in the game: no crash at 6 spots.

- techset.cpp parse_statebits: opens the merged file from its own search path.
- material.cpp write: logs techniques that have no state bits.

Not verified: IW6 build (IW6 is not in this --games set).
```
