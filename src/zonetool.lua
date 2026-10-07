zonetool = {}

function zonetool:project()
	project "zonetool"
		kind "ConsoleApp"
		language "C++"

		targetname "zonetool"

		pchheader "std_include.hpp"
		pchsource "src/cli/std_include.cpp"

		files {
			"./src/cli/**.rc",
			"./src/cli/**.hpp",
			"./src/cli/**.cpp",
			"./src/cli/resources/**.*",
		}

		includedirs {
			"./src/cli",
			"./src/games",
			"./src",
			"%{prj.location}/src",
		}

		resincludedirs {"$(ProjectDir)src"}

		links {"core", "common"}

		prebuildcommands {"pushd %{_MAIN_SCRIPT_DIR}", "tools\\premake5 generate-buildinfo", "popd"}

		if _OPTIONS["copy-to"] then
			postbuildcommands {"copy /y \"$(TargetPath)\" \"" .. _OPTIONS["copy-to"] .. "\""}
		end

		core.includes()
		games.import()
end
