h2 = {}
function h2:project()
    project "h2"
		kind "ConsoleApp"
		language "C++"

		targetname "h2-zonetool"

		pchheader "std_include.hpp"
		pchsource "src/h2/std_include.cpp"

		linkoptions {"/IGNORE:4254", "/DYNAMICBASE:NO", "/SAFESEH:NO", "/LARGEADDRESSAWARE", "/LAST:.main", "/PDBCompress"}

		files {
			"./src/h2/**.rc", 
			"./src/h2/**.hpp", 
			"./src/h2/**.cpp", 
			"./src/h2/resources/**.*"
		}

		includedirs {
			"./src",
			"./src/h2", 
			"./src/shared", 
			"%{prj.location}/src"
		}

		resincludedirs {"$(ProjectDir)src"}

		dependson {"tlsdll"}

		links {"shared"}

		prebuildcommands {"pushd %{_MAIN_SCRIPT_DIR}", "tools\\premake5 generate-buildinfo", "popd"}

		if COMPUTER_NAME == "PC-JOEL" then
			debugdir "E:\\Blizzard\\Call of Duty Modern Warfare 2 Campaign Remastered"
			debugcommand "E:\\Blizzard\\Call of Duty Modern Warfare 2 Campaign Remastered\\$(TargetName)$(TargetExt)"
		end
		postbuildcommands {
			"if \"%COMPUTERNAME%\" == \"PC-JOEL\" ( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"E:\\Blizzard\\Call of Duty Modern Warfare 2 Campaign Remastered\\$(TargetName)$(TargetExt)\" )",
		}

		if _OPTIONS["h2-copy-to"] then
			postbuildcommands {"copy /y \"$(TargetPath)\" \"" .. _OPTIONS["copy-to"] .. "\""}
		end

		dependencies.imports()
end