h1 = {}
function h1:project()
    project "h1"
		kind "ConsoleApp"
		language "C++"

		targetname "h1-zonetool"

		pchheader "std_include.hpp"
		pchsource "src/h1/std_include.cpp"

		linkoptions {"/IGNORE:4254", "/DYNAMICBASE:NO", "/SAFESEH:NO", "/LARGEADDRESSAWARE", "/LAST:.main", "/PDBCompress"}

		files {
			"./src/h1/**.rc", 
			"./src/h1/**.hpp", 
			"./src/h1/**.cpp", 
			"./src/h1/resources/**.*"
		}

		includedirs {
			"./src", 
			"./src/h1", 
			"./src/shared", 
			"%{prj.location}/src"
		}

		resincludedirs {"$(ProjectDir)src"}

		dependson {"tlsdll"}

		links {"shared"}

		prebuildcommands {"pushd %{_MAIN_SCRIPT_DIR}", "tools\\premake5 generate-buildinfo", "popd"}

		if COMPUTER_NAME == "PC-JOEL" then
			debugdir "E:\\Steam\\steamapps\\common\\Call of Duty Modern Warfare Remastered"
			debugcommand "E:\\Steam\\steamapps\\common\\Call of Duty Modern Warfare Remastered\\$(TargetName)$(TargetExt)"
		end
		postbuildcommands {
			"if \"%COMPUTERNAME%\" == \"PC-JOEL\" ( copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"E:\\Steam\\steamapps\\common\\Call of Duty Modern Warfare Remastered\\$(TargetName)$(TargetExt)\" )",
		}

		if _OPTIONS["h1-copy-to"] then
			postbuildcommands {"copy /y \"$(TargetPath)\" \"" .. _OPTIONS["copy-to"] .. "\""}
		end

		dependencies.imports()
end