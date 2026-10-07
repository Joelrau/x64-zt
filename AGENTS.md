# AGENTS.md

Guide for humans and agents who work on x64-zt. Read it before you change code.

## What x64-zt is

x64-zt is a fastfile unlinker and linker for x64 Call of Duty titles. It reads `.ff` zones, dumps their assets to disk, builds new zones from those dumps, and converts assets from one game to another.

The `v2` branch is a standalone rewrite. It reads fastfiles directly and never loads a game exe via runtime methodology. v1 (`main`) loaded the game binary and hooked its zone loader. v2 keeps the v1 asset logic and the v1 dump formats. Current scope is H1 (MWR 1.15) and IW7. Other games are currently not supported, but can be done with due time easily.

## Build

This was only tested for Windows using Visual Studio 2022 via MSBuild and premake.

- `generate.bat --games=<games>` is how you select which games you want to build (all by default)
- `--games=h1`, `--games=h1,iw7` or `--games=all` selects the games
- `--no-convert` skips the converter projects if you are not doing any conversions. A converter project `convert-<a>-<b>` builds only when both games are selected. Each game is a separate static library, so a change in one game does not rebuild the other.
- `--copy-to=<folder>` copies `zonetool.exe` to that folder after each build.
- premake writes `build/generated/enabled_games.hpp` with `ZONETOOL_GAMES(x)` and `ZONETOOL_CONVERTERS(x)`. Do not edit it.

## Run

```bash
zonetool.exe -game h1 dumpzone mp_shipment
```

| Command | Result |
|---|---|
| `inspect <zone>`, `inspect --all` | Quick inspect a fastfile container and its data |
| `verifyzone <zone>`, `verifyzone --all` | Reads every asset and checks that the reader consumed the zone exactly |
| `dumpzone <zone> [--target <game>]` | Writes `dump/<zone>/` and `dump/<zone>/<zone>.csv`. `--target` converts the assets |
| `buildzone <zone>` | Reads `zone_source/<zone>.csv` and the files in `zonetool/<zone>/`, then writes `output/<zone>.ff` |

Without a command, `zonetool.exe` opens an interactive console.

- Game selection: `-game <name>`, then `zonetool.json` `game`.
- Game folder: `-path <folder>`, then `zonetool.json` `paths.<game>`, then the exe folder if it has `zone/`, then the Steam default folder.
- `zonetool.json` example: `{"game": "h1", "paths": {"h1": "D:/Games/MWR"}}`.
- Flags for `dumpzone`: `-dds` writes DDS images. `-dump_streamed_image` reads `imagefile*.pak` and writes streamed images.
- The `require` row in a build CSV loads another zone into the asset database. There is no `loadzone` command.
- `buildzone` never writes to the game folder.

## Source layout

```
src/common/            utils: io, string, compression, crypto, flags
src/core/xfile/        container: header, stream file table, signed blocks, zlib, LZ4
src/core/zone/         zone_reader, zone_buffer, asset database, script strings
src/core/formats/      csv, iwi, imagefile, mapents, ddl
src/core/assets/       asset templates that more than one game uses
src/core/io/           filesystem, assetmanager (binary dump format)
src/games/<game>/      game.cpp, structs.hpp, <game>.hpp, assets/, zone/, dump.cpp, build.cpp
src/convert/<a>_<b>/   converter from game <a> to game <b>
src/cli/               main, commands, settings, game registry, crash handler
deps/                  submodules; deps/premake/*.lua defines each dependency project
tools/                 build.bat, premake5.exe, asm_slice.py
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
- RUNTIME data holds no file bytes. The game zero-fills it.
- Data read in TEMP is lost after `pop_stream`. Copy it with `duplicate` if you keep it.
- H1 pointers: `0xFDFDFDFFFFFFFFFF` is inline, `...FE` is inline with an insert slot. Other values are offsets: block `(v >> 32) & 0xF`, offset `(u32)v - 1`.
- IW7 pointers: -1 is shared (treat as inline), -2 is inline, -3 is insert. Other values are offsets: block is the high dword.
- The game reads most arrays inline when the pointer is not null, and ignores the value. Third-party zones store raw heap pointers there. `zone_reader` follows only a valid offset.
- Stock zones point some fields into arrays that are already written. The writers reuse these with `insert_sub_buffer` and `find_sub_buffer`, so built zones keep the stock size.

## Compatibility rules

- Keep working v1 logic. x64-zt asset code is studied and tested since 2021. If it works, do not change it.
- Keep the v1 dump formats. A dump from v1 must parse in v2. Change a format only when it is necessary, and tell the user.
- v2 has no game runtime. A database lookup can return null where v1 got a game default asset. Add a null check, not a fake default.
- Known v2 differences from v1: `dumpzone <zone> --target <game>` replaces `dumpzone <target> <zone>`, IW7 `proceduralbones` has a dump format, built zones share arrays the way stock zones do, and output goes to `output/`.

## Verification

Run these after a change to a reader, writer or the container code:

| Check | Command | Pass condition |
|---|---|---|
| Container | `inspect --all` | Every zone passes |
| Reader | `verifyzone --all` | H1: 6991 of 6991. IW7: 686 of 687 (`mp_vlobby_room` on disk is a modified file) |
| Writer | `dumpzone X`, `buildzone X`, `verifyzone X`, then `dumpzone` the built zone | The two dumps are equal, except pointer values and the known differences |

Good round-trip zones: H1 `mp_shipment`, `mp_crash`. IW7 `mp_frontend`, `mp_afghan`.

## Research material

Two folders are local only and gitignored. They are not on every machine.

- `docs/`: v2 design, format notes, the v2 checklist and log. Start at `docs/README.md`. `docs/v2_codebase.md` holds the current state and the known dump and round-trip differences.
- `re_reference/`: IDA text dumps of the game binaries (H1 MP 1.15, H1 MP 1.4, IW7 ship). See `re_reference/README.md`.

Rules for the IDA dumps:

- Do not read `ship_asm.txt` or `h1_asm.txt` in full. Each file is 100 to 330 MB.
- Find the function in `func_index.txt` (`line:; FUNC start end name`). Read only that line range with `sed -n`.
- Function names are often wrong because of identical code folding. Start at `Load_XAssetHeader`, find the `cmp edx, <type>` case, and follow the call targets.
- `tools/asm_slice.py <name regex> -g h1|h1_1.4|iw7` prints functions by name. It needs Python.

## Code style

- lowercase snake_case for functions, variables, classes and files. Engine structs keep their engine names (`XModel`, `GfxImage`).
- Tabs, braces on their own line, `this->` for members, `const auto` where the type is clear. Match the file you edit.
- Keep the structure flat. Do not make a file for one declaration. Put a small declaration in the header that already owns the topic. A list macro belongs in the game header, and a value that one file uses belongs in that file.
- No umbrella headers outside `<game>.hpp`. Include what you use.
- Name things so that the code reads without comments. Prefer a small well-named function to a comment over a block.

### Comments

Default: no comment. Code must explain itself through naming and structure. A comment is justified only when it carries information that the code cannot show:

- a non-obvious external constraint, for example game or format behavior verified in the asm;
- a deliberate deviation that a reader would want to "fix" back;
- a hack or workaround that can break later, with the reason;
- a reference that saves a research session.

Never write restatements, section banners, edit narration or commented-out code. If unsure whether a comment qualifies, it does not. Example of a valid comment: the raw heap pointer note in `zone_reader::read_array`.

### Technical writing

Docs, commit messages and user-facing text use ASD-STE100 style:

- Max 20 words per sentence in instructions, 25 in descriptions.
- Imperative for steps. One instruction per sentence. Condition before command.
- Simple tenses only. No present perfect, no -ing verbs, no should/would/may/might.
- Active voice. One word per meaning, no synonym rotation.
- No contractions. Keep articles and "that".
- Delete filler: simply, robust, seamlessly, leverage.
- Code and identifiers stay exact.

## Working notes for agents

- Commit only when the user asks.
- Never co-author yourself as whatever agent you are, just the person who wants to commit.
- Do implementation work in the main session.
- Put temporary files in a session scratchpad, not in the repository or `/tmp`.
- If Python is not installed on the main dev machine, use shell tools, node, or the edit tools.
- `core.autocrlf` is `true`. Some files are CRLF in the working tree. Keep the line endings of the file you edit.
- To test H1 in the game, copy `zonetool.exe` to the user's MWR 1.15 folder (if you don't know where it is, prompt a question and ask the user so you know - this will help save so much time), or build with `--copy-to`.
