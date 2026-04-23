zonetool_iw8 = {}
function zonetool_iw8:project()
    project "zonetool-iw8"
		kind "SharedLib"
		language "C++"

		targetname "XInput9_1_0"

		pchheader "std_include.hpp"
		pchsource "src/zonetool-iw8/std_include.cpp"

		linkoptions {"/IGNORE:4254"}

		files {
			"./src/zonetool-iw8/**.hpp",
			"./src/zonetool-iw8/**.cpp",
			"./src/zonetool/resource.hpp",
			"./src/zonetool/game/mode.*",
			"./src/zonetool/game/shared.*",
			"./src/zonetool/game/iw8/**",
			"./src/zonetool/component/iw8/**",
			"./src/zonetool/zonetool/iw8/**",
			"./src/zonetool/zonetool/shared/**",
			"./src/zonetool/zonetool/utils/**",
			"./src/zonetool/loader/component_interface.hpp",
			"./src/zonetool/loader/component_loader.hpp",
			"./src/zonetool/loader/component_loader.cpp",
		}

		includedirs {
			"./src",
			"./src/zonetool-iw8",
			"./src/zonetool",
			"./src/common",
			"%{prj.location}/src",
		}

		resincludedirs {"$(ProjectDir)src"}

		links {"common"}

		local function addcopyopt(name)
			if (_OPTIONS[name] ~= nil) then
				postbuildcommands {"copy /y \"$(TargetPath)\" \"" .. _OPTIONS[name] .. "\""}
			end
		end

		addcopyopt("iw8-copy-to")

		dependencies.imports()
end
