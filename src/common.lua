common = {}

function common:project()
	project "common"
		kind "StaticLib"
		language "C++"

		files {
			"./src/common/**.hpp",
			"./src/common/**.cpp",
		}

		includedirs {
			"./src/common",
		}

		dependencies.use(gsl, zlib, minizip, libtomcrypt, libtommath)
end
