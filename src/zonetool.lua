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

		if COMPUTER_NAME == "PC-JOEL" then
			debugdir "E:\\Steam\\steamapps\\common\\Call of Duty Modern Warfare Remastered"
			debugcommand "E:\\Steam\\steamapps\\common\\Call of Duty Modern Warfare Remastered\\$(TargetName)$(TargetExt)"
		end
		postbuildcommands {
			"if \"%COMPUTERNAME%\" == \"PC-JOEL\" ( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"E:\\Steam\\steamapps\\common\\Call of Duty Modern Warfare Remastered\\$(TargetName)$(TargetExt)\" )",
			"if \"%COMPUTERNAME%\" == \"PC-JOEL\" ( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"E:\\Blizzard\\Call of Duty Modern Warfare 2 Campaign Remastered\\$(TargetName)$(TargetExt)\" )",
			"if \"%COMPUTERNAME%\" == \"PC-JOEL\" ( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"D:\\Steam\\steamapps\\common\\Call of Duty Advanced Warfare\\$(TargetName)$(TargetExt)\" )",
		}

		if _OPTIONS["copy-to"] then
			postbuildcommands {"copy /y \"$(TargetPath)\" \"" .. _OPTIONS["copy-to"] .. "\""}
		end

		dependencies.imports()
end