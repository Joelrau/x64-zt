zonetool_iw8 = {}
function zonetool_iw8:project()
    project "zonetool-iw8"
		kind "SharedLib"
		language "C++"

		targetname "XInput9_1_0"

		linkoptions {"/IGNORE:4254"}

		files {
			"./src/zonetool-iw8/**.hpp",
			"./src/zonetool-iw8/**.cpp",
		}

		includedirs {
			"./src/zonetool-iw8",
		}

		local function addcopyopt(name)
			if (_OPTIONS[name] ~= nil) then
				postbuildcommands {"copy /y \"$(TargetPath)\" \"" .. _OPTIONS[name] .. "\""}
			end
		end

		addcopyopt("iw8-copy-to")
end
