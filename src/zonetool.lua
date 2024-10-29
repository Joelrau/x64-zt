zonetool = {}
function zonetool:project()
    project "zonetool"
		kind "ConsoleApp"
		language "C++"

		targetname "zonetool"

		pchheader "std_include.hpp"
		pchsource "src/zonetool/std_include.cpp"

		linkoptions {"/IGNORE:4254", "/DYNAMICBASE:NO", "/SAFESEH:NO", "/LARGEADDRESSAWARE", "/LAST:.main", "/PDBCompress"}

		files {
			"./src/zonetool/**.rc", 
			"./src/zonetool/**.hpp", 
			"./src/zonetool/**.cpp", 
			"./src/zonetool/resources/**.*"
		}

		includedirs {
			"./src", 
			"./src/zonetool", 
			"./src/common", 
			"%{prj.location}/src"
		}

		resincludedirs {"$(ProjectDir)src"}

		dependson {"tlsdll"}

		links {"common"}

		prebuildcommands {"pushd %{_MAIN_SCRIPT_DIR}", "tools\\premake5 generate-buildinfo", "popd"}

		if COMPUTER_NAME == "JOEL-PC" then
			debugdir "D:\\Games\\PC\\IW7"
			debugcommand "D:\\Games\\PC\\IW7\\$(TargetName)$(TargetExt)"
			postbuildcommands {
				"( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"D:\\Games\\PC\\H1\\$(TargetName)$(TargetExt)\" )",
				"( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"D:\\Games\\PC\\H2\\$(TargetName)$(TargetExt)\" )",
				"( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"D:\\Games\\PC\\S1\\$(TargetName)$(TargetExt)\" )",
				"( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"D:\\Games\\PC\\IW6\\$(TargetName)$(TargetExt)\" )",
				"( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"D:\\Games\\PC\\IW7\\$(TargetName)$(TargetExt)\" )",
				"( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty Black Ops III\\UnrankedServer\\$(TargetName)$(TargetExt)\" )",
			}
		end

		local function addcopyopt(name)
			if (_OPTIONS[name] ~= nil) then
				postbuildcommands {"copy /y \"$(TargetPath)\" \"" .. _OPTIONS[name] .. "\""}
			end
		end

		addcopyopt("copy-to")
		addcopyopt("iw6-copy-to")
		addcopyopt("s1-copy-to")
		addcopyopt("h1-copy-to")
		addcopyopt("h2-copy-to")
		addcopyopt("iw7-copy-to")
		
		dependencies.imports()
end