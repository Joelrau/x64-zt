# Contributing to x64-zt

This file tells you how to set up, how to choose a task, how to test, and how to send a pull request in for x64-zt. We thank you for being interested in helping out!

## Ways to help

You do not need to write C++ to help.

- Test a converted map or weapon in the game and report what you see. Give screenshots and positions (see "Report a bug"). Any small details, lack of conversions, bugs are good ways to report issues and help us improve.
- Make RenderDoc captures of a render bug in the source game and in the target game. 2 renderdoc captures together help validate lots of things between two games to get accurate visuals and data working.
- Port a weapon and test it. In x64-zt-gui, `Tools > Quick port weapon` ports an IW7 weapon to an IW8 package in one job. `AGENTS.md` "Port a weapon" has the routes and the known problems.
- Dump stock zones and compare them with a converted zone.

If you would like to get hands with the engine though, you can:

- Name unknown fields of a asset struct and document it better.
- Fix a reader, a writer or a converter.
- Add a game. `AGENTS.md` "Add a new game" gives the milestones.

## Set up

1. Install Visual Studio 2022 with the "Desktop development with C++" workload, Git and Python 3.
2. Clone the repository with its submodules:

   ```bash
   git clone --recurse-submodules https://github.com/auroramod/x64-zt.git
   cd x64-zt
   git checkout develop
   ```

3. Make the projects for the games that you work on. Use `all` if you are not sure:

   ```bash
   generate.bat --games=h1,iw7
   ```

4. Build Release x64. Open `build/x64-zt.sln`, or run `tools\build.bat h1+iw7`. The result is `build/bin/x64/Release/zonetool.exe`.
5. Unpack the asm of the game exe dumps:

   ```bash
   python tools/asm_pack.py unpack
   ```

6. Make a local `docs/` folder for your notes. Git ignores it.

Keep the same `--games` set while you work. A different set rebuilds everything.

## The game exe dumps

`re_reference/` holds an IDA text export of the exe of each supported game. It is the main research source of the project. It lets you and your AI read the zone loader of a game (`Load_*` functions), its structs and its renderer without a copy of IDA.

| File | Content |
|---|---|
| `ship_asm.txt.xz` | The packed disassembly. `python tools/asm_pack.py unpack` writes `ship_asm.txt` |
| `func_index.txt`, `funcs.txt` | Each function with its address, size and name |
| `names.txt`, `strings.txt` | Named addresses and strings |
| `types.h` | The local types of the IDA database |

The `ship_asm.txt` files are 85 MB to 600 MB. Do not open them in an editor. Use `tools/asm_slice.py`:

```bash
python tools/asm_slice.py iw7 "Load_Material$"
python tools/asm_slice.py h1_1.4 -l 14060B312 1406060E9
python tools/asm_slice.py iw7 -r "aRSmodelinstanc"
```

`AGENTS.md` "Research with re_reference" tells how to find a loader, a field reader or a dvar. `re_reference/README.md` lists the dumps.

## Choose a task

- Choose one topic: one game, one converter or one asset type.
- Look at the open issues and the "Verification" table in `AGENTS.md`. A zone that fails `verifyzone` is a good first task.
- For a large task, open a draft pull request or an issue early. Tell the others what you work on, so that two people do not do the same work.
- Make a task file `docs/<TASK>.md`. Write the goal, the open items, the proven facts and a dated log. `AGENTS.md` "Working formula" gives the format.

## Work with an AI agent

x64-zt has rules for agents in `AGENTS.md`. `CLAUDE.md` loads the same file. The rules work with any capable model, also with smaller models, if the agent follows them step by step.

- Point your agent at `AGENTS.md` at the start of each session.
- Give the agent the task file. After a long session, tell it to read the task file again.
- Tell the agent the folders of your games. `AGENTS.md` does not list them.
- Answer the questions of the agent. It asks when the files do not give the answer. You know how the game must look.
- Give the agent screenshots when it asks for them. See "Report a bug".
- Read each diff before you commit it. Remove comments that explain the research, debug code, raw offsets and structs in `.cpp` files. `AGENTS.md` "Code style" and "Comments" give the rules.
- Do not accept a fix that the agent did not test. The agent must list each check that it did not run.

Good examples of converter code: `src/convert/iw6_h1/` was written by humans before AI and is the reference for native IW6 -> H1 techsets. The legacy IW5 to H1 converter is on the v1 `main` branch and is even present here. `src/convert/h1_s2/` and `src/convert/s2_h1/` were written with AI help and also work well, so that can give you a clue as to how to use it for this project properly.

## Test

Run the checks of `AGENTS.md` "Verification" that apply to your change:

| Change | Check |
|---|---|
| Container code | `zonetool.exe -game <game> inspect --all` |
| A reader | `zonetool.exe -game <game> verifyzone --all`. Compare the count with the table in `AGENTS.md` |
| A writer | Dump a zone, build it, run `verifyzone` of the built zone, then dump it again and compare the two dumps |
| A converter | Dump the map with `--target`, build it, run `verifyzone` and load it in the game |
| A weapon port | Build each zone of the package, run `verifyzone` of each zone, then `give` the weapon in the game and test hip fire, ADS, reload, melee and the sounds |

`buildzone` writes to the `zone/` folder of the game folder and replaces a zone of the same name. Back up the stock zone, or use a new zone name.

Clients for tests in the game:

| Game | Client |
|---|---|
| H1 | h1-mod. Load a map from `usermaps/` with `+devmap <map>` |
| IW7 | iw7-mod. Copy the zone to the `zone/` folder of the game |
| IW8 1.20 | The replay client (`mw120rproxy`) with a `mods/mw120r/maps/<id>/` package |
| S2 | S2x. Press PLAY, then `map <name>` |

The Debug builds of h1-mod and iw7-mod have in-game tools that help a lot:

- h1-mod Debug: viewers for gfxworld, comworld, mapents, materials, techsets, shaders, xmodels, weapons and more.
- iw7-mod Debug: an asset list, a navmesh view, asset dumps and a reflection probe generator (`r_reflectionProbeGenerate 1`).

Build them from the source of each client project. Ask the people of that project for help with a client feature.

## Report a bug

A good report lets a person or an agent find the cause without a guess.

1. Give the game, the build, the map and the command that you ran.
2. Give the log lines of x64-zt, mainly the `ZONETOOL_WARNING` and error lines.
3. For a crash, give the crash file of the client: `minidumps/<newest>/info.txt` for h1-mod and iw7-mod, or the `mw120rproxy.crash.*.log` file for the IW8 replay client.
4. For a visual bug, give 5 to 8 positions. At each position, give one screenshot from the source game and one from the target game. Use the same `setviewpos` in both games, and write the `setviewpos` line with each pair.
5. For a render bug, also give a RenderDoc capture of the same spot in both games if you can.

Take screenshots of the game window only.

## Send a pull request

1. Make a branch from `develop`.
2. Keep one topic in the pull request. Do not mix a refactor with a fix.
3. Change only the lines that the fix needs. Keep the line endings of each file. Run `git diff --check`.
4. Build Release before you push. The CI builds Release with warnings as errors.
5. Write the description:
   - What the change does, in a few short lines.
   - The proof for each layout change: the `Load_*` function, its address and the game build.
   - Each check that you ran and its result: the build, the `verifyzone` count, the round trip, the test in the game.
   - "Not verified:" and each check that you did not run.
   - A image of your featire/change would be nice as well
6. Do not commit machine paths, user names, drive letters, dumps, built zones or game files.
7. If you add a command, a flag, a game, a dump or a verification count, update `AGENTS.md`.

A pull request with a clear proof and a clear test list is easy to review. A pull request without them can stay open for a long time.

## Get help

Ask the community in the [Aurora Discord server](http://auroramod.dev/discord), or open an issue or a draft pull request. Do not be afraid to ask. Give what you tried, what you found and what is missing.
