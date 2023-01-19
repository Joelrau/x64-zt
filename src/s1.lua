s1 = {}
function s1:project()
    project "s1"
		kind "ConsoleApp"
		language "C++"

		targetname "s1-zonetool"

		pchheader "std_include.hpp"
		pchsource "src/s1/std_include.cpp"

		linkoptions {"/IGNORE:4254", "/DYNAMICBASE:NO", "/SAFESEH:NO", "/LARGEADDRESSAWARE", "/LAST:.main", "/PDBCompress"}

		files {
			"./src/s1/**.rc", 
			"./src/s1/**.hpp", 
			"./src/s1/**.cpp", 
			"./src/s1/resources/**.*"
		}

		includedirs {
			"./src", 
			"./src/s1", 
			"./src/shared", 
			"%{prj.location}/src"
		}

		resincludedirs {"$(ProjectDir)src"}

		dependson {"tlsdll"}

		links {"shared"}

		prebuildcommands {"pushd %{_MAIN_SCRIPT_DIR}", "tools\\premake5 generate-buildinfo", "popd"}

		if COMPUTER_NAME == "PC-JOEL" then
			debugdir "D:\\Steam\\steamapps\\common\\Call of Duty Advanced Warfare"
			debugcommand "D:\\Steam\\steamapps\\common\\Call of Duty Advanced Warfare\\$(TargetName)$(TargetExt)"
		end
		postbuildcommands {
			"if \"%COMPUTERNAME%\" == \"PC-JOEL\" ( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"D:\\Steam\\steamapps\\common\\Call of Duty Advanced Warfare\\$(TargetName)$(TargetExt)\" )",
		}

		if _OPTIONS["s1-copy-to"] then
			postbuildcommands {"copy /y \"$(TargetPath)\" \"" .. _OPTIONS["copy-to"] .. "\""}
		end

		dependencies.imports()
end