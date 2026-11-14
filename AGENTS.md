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

8. Never open a `ship_asm.txt` file with a file tool, `Get-Content` or `ReadAllLines`. Each file is more than 100 MB. Use `tools/asm_slice.py`.
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
| `buildzone <zone> [-use_usermaps]` | Reads `zone_source/<zone>.csv` and the files in `zonetool/<zone>/`, then writes `<game folder>/zone/<zone>.ff`. `-use_usermaps` writes the zone to `<game folder>/usermaps/<zone>/` instead. Not for IW8 |
| `generatecsv <map> [sp] [--path <dir>]` | Writes `zone_source/<map>.csv` from the files in `zonetool/<map>/` |

Without a command, `zonetool.exe` opens an interactive console that waits for input. An agent always passes a command. `zonetool.exe -h` prints every command and flag.

- `dump/`, `zonetool/`, `zonetool_paths/`, `zone_source/` and `asset_listing/` are relative to the working directory. The usual working directory is the game folder.
- A `-` flag can go anywhere on the command line. Put a `--` option where the table shows it.
- Game selection: `-game <name>`, then the game exe in the `-path` folder or the exe folder (`game::binaries`), then `zonetool.json` `game`.
- Game folder: `-path <folder>`, then `zonetool.json` `paths.<game>`, then the exe folder if it has `zone/`, then the Steam default folder.
- `-target_path <folder>` sets the game folder of the `--target` game. Without it: `zonetool.json` `paths.<game>`, then the Steam default folder.
- IW8 has no `zone/` folder. x64-zt reads its fastfiles from the CASC storage in `Data/`, applies the `.fp` patch of each zone, and loads `oo2core_*_win64.dll` from the game folder for Oodle. Streamed images come from the `pak_*.xpak` files in CASC.
- `zonetool.json` is next to `zonetool.exe`. Example: `{"game": "h1", "paths": {"h1": "path/to/mwr"}}`.
- `dumpasset` finds only assets that the zone holds, not referenced ones (`type,,name` rows in the CSV). Type names are the CSV type names.
- With `--target`, a converter loads its `required_zones` (h1_s2: `common_core_mp`, `common_mp` and others) and can also dump common-zone assets that a converted asset uses (h1_s2 `dump_used`: images, xanims, sounds with their loaded sounds). The S2 `buildzone` skips the ones S2 already ships through the `ignore` rows.
- H1 to S2 output can need S2x: a map zone cannot replace a rawfile that S2 `common_mp` already holds, so h1_s2 writes such files as `override/<name>` and S2x loads `override/<name>` first. This covers `vision/<map>.vision` when `common_mp` has the same name, and `override/animtrees/destructibles.atr` (the stock tree plus the H1 scriptable anims it lacks, since S2 looks scriptable anims up in the hardcoded `destructibles` tree). h1_s2 reads `<S2 folder>/zone/common_mp.ff` to find them. A hand-filtered `zone_source/<map>.csv` needs the `rawfile,override/...` rows.
- Flags for `dumpzone`: `-dds` writes DDS images. `-dump_streamed_image` reads `imagefile*.pak` and writes streamed images. `-castdump` writes a `.cast` file next to each xmodel and xanim; IW6, S1, H1, H2 and IW7 share `src/core/assets/cast_dump.hpp`. See `docs/S2_SUPPORT.md`.
- Flags for `buildzone`: `-more_memory` doubles the zone memory limit. `-keep_mp_traverse_nodes` keeps invalid mp traverse nodes in aipaths (H2).
- The `require` row in a build CSV loads another zone into the asset database. There is no `loadzone` command.
- `buildzone` writes `<zone>.ff` and `<zone>.pak` to the `zone/` folder of the game folder. It replaces a zone of the same name.
- `-no_cache` writes every output again and ignores the build cache.

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
tools/                 build.bat, premake5.exe, asm_slice.py, livemem.py, game_window.ps1, s2_hub_to_h1.ps1
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
| IW7 | 686 of 687 (`mp_vlobby_room` on disk is a modified file) | `mp_frontend`, `mp_afghan` |
| IW8 | 9471 of 9494 (`code_post_gfx` and old frontend zones fail, see `docs/IW8_SUPPORT.md`) | None. IW8 has no `buildzone` |
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

Default: no comment. Before you add a comment, try to delete it. Less is better, and nothing is best.

Write a comment only for one of these cases:

- a deviation from the asm or from v1 that a reader would "fix" back;
- a hack or workaround that can break later;
- a TODO;
- an address or a name that the code cannot show, for example an unnamed builtin.

Form:

- Put the comment at the end of the line that it describes. Do not put it on its own line above the code.
- Use the fewest words possible. A name and an address is enough.
- One line only. No comment blocks, no comments above functions, no section banners.

Do not write:

- how the game or the client uses a value;
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
grep -n "struct GfxStateBits" -A12 src/games/h1/structs.hpp
```

- Dump names: `h1` (1.15, RVAs), `h1_1.4` (full addresses), `h2`, `iw6`, `iw7`, `iw8`, `s1`, `s2`.
- `-l` maps a call stack to function names. `-a` prints the lines around an address. `-c` lists every call site of a function.
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
