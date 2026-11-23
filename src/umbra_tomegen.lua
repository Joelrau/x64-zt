umbra_tomegen = {}

function umbra_tomegen:project()
	project "umbra-tomegen"
		kind "ConsoleApp"
		language "C++"
		characterset "MBCS"

		cppdialect "C++17"
		optimize "Speed"
		runtime "Release"
		removedefines { "_DEBUG", "DEBUG" }
		defines { "NDEBUG", "_CRT_SECURE_NO_WARNINGS" }

		files {
			"./src/umbra_tomegen/**.hpp",
			"./src/umbra_tomegen/**.cpp",
			"./src/common/utils/umbra/scene.hpp",
		}

		includedirs {
			"./src/common",
		}

		dependencies.use(umbra3)
end
