# x64 ZoneTool

## Supported Games
* **IW6** (*Call of Duty: Ghosts*)
* **S1** (*Call of Duty: Advanced Warfare*)
* **H1** (*Call of Duty: Modern Warfare Remastered*)
* **H2** (*Call of Duty: Modern Warfare 2 Campaign Remastered*)

## Wiki
Check out the [wiki](https://github.com/Joelrau/x64-zt/wiki) for useful guides & information.

## Commands
* `loadzone <zone>`: Loads a zone
* `unloadzones`: Unloads zones
* `verifyzones <zone>`: Lists assets in a zone
* `dumpzone <zone>`: Dumps a zone
* `dumpzone <target game> <zone> <asset filter>`: Dumps a zone converting assets for a specific game
* `dumpasset <type> <name>`: Dumps a single assset
* `dumpmap <map>`: Dumps all required assets for a map
* `dumpmap <target game> <map> <asset filter> <skip common>`: Dumps and converts all required assets for a map

  ### Definitions
  * `asset filter`: A filter specifying all the asset types that should be dumped, if not specified or empty it will dump all asset types.  
  Asset types are separated by **commas**, **`_`** indicates and empty filter.   
  **Examples**: 
    - `dumpzone h1 mp_clowntown3 sound,material,techset,rawfile`  
    - `dumpmap h1 mp_clowntown3 _ true`
  * `skip common`: Skips common zones when dumping a map, can be `true` or `false`.
  * `target game`: The game to convert the assets to.

## Conversion support

|| **IW6** | **S1** | **H1** | **H2** |
| --- | --- | --- | --- | --- |
| **IW6** | ✔️ | ❌ | ✔️ | ❌ |
| **S1** | ❌ | ✔️ | ✔️ | ✔️ |
| **H1** | ❌ | ❌ | ✔️ | ✔️ |
| **H2** | ❌ | ❌ | ✔️ | ✔️ |
