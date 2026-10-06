core = {}

function core.includes()
	includedirs {
		"./src/core",
		"./src/common",
		"%{wks.location}/generated",
	}

	dependencies.use(gsl, json, zlib, lz4, zstd, libtomcrypt, libtommath)
end

function core:project()
	project "core"
		kind "StaticLib"
		language "C++"

		pchheader "std_include.hpp"
		pchsource "src/core/std_include.cpp"

		files {
			"./src/core/**.hpp",
			"./src/core/**.cpp",
		}

		links {"common"}

		core.includes()
end
