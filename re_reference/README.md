# re_reference

This folder holds IDA text exports of the game executables that x64-zt supports. Use them to check a struct layout, an offset or a game behavior against the asm without IDA. Agents are the main users, but humans can use them too.

| Path | Content |
|---|---|
| `h1/mp_1.15/` | H1 MP 1.15. Addresses are RVAs |
| `h1/mp_1.4/` | H1 MP 1.4, the build that h1-mod runs. Image base `0x140000000`, no Arxan clean output |
| `h2/ship/` | H2 (MW2CR) ship |
| `iw6/ship/` | IW6 (Ghosts) ship |
| `iw7/ship/` | IW7 Steam latest |
| `iw8/ship/` | IW8 (MW 2019) ship |
| `s1/ship/` | S1 (AW) ship |
| `s2/ship/` | S2 (WWII) ship |

## Rules for agents

- Read "Research with re_reference" in `AGENTS.md` first. It has the file list, the search steps, the asm format and the timings.
- Never open `ship_asm.txt` with a file tool, `Get-Content` or `ReadAllLines`. The files are 85 MB to 330 MB.
- Print one function with `python tools/asm_slice.py <dump> <name or address>`. It reads only the line range from `func_index.txt`.
- For a fast text search, use `rg -n -m <count>` on `ship_asm.txt`. Then map all hit addresses to functions with one `asm_slice.py -l` call.
- Write a regex in the IDA form: `\+0A2h\]`, not `0xa2`.
- Trust the addresses, not the names. Many H1 1.15 names are wrong, because the compiler folds identical functions into one.
- Use the dump of the build that made the address. h1-mod runs H1 1.4, so an h1-mod crash address needs `h1_1.4`.
- Do not edit the export files. Export the dump again from IDA when the game build changes.
- Git holds `ship_asm.txt.xz`, not `ship_asm.txt`. `asm_slice.py` unpacks it on the first use. Before you use `rg`, run `python tools/asm_pack.py unpack`.
- To add a dump, put it in `<game>/<build>/` and add its name to `DUMPS` in `tools/asm_slice.py`. Then run `python tools/asm_pack.py pack <dump>`.
