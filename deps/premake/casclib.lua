casclib = {
	source = path.join(dependencies.basePath, "CascLib"),
}

function casclib.import()
	links { "casclib", "ws2_32" }
	casclib.includes()
end

function casclib.includes()
	includedirs {
		path.join(casclib.source, "src"),
	}

	defines {
		"CASCLIB_NO_AUTO_LINK_LIBRARY",
	}
end

function casclib.project()
	project "casclib"
		language "C++"

		casclib.includes()
		zlib.includes()

		files {
			path.join(casclib.source, "src/*.h"),
			path.join(casclib.source, "src/*.cpp"),
			path.join(casclib.source, "src/common/*.cpp"),
			path.join(casclib.source, "src/hashes/*.cpp"),
			path.join(casclib.source, "src/overwatch/aes.cpp"),
			path.join(casclib.source, "src/overwatch/apm.cpp"),
			path.join(casclib.source, "src/overwatch/cmf.cpp"),
			path.join(casclib.source, "src/jenkins/lookup3.c"),
		}

		defines {
			"CASC_USE_SYSTEM_ZLIB",
			"_CRT_SECURE_NO_DEPRECATE",
		}

		links { "zlib" }

		warnings "Off"
		fatalwarnings {}
		kind "StaticLib"
end

table.insert(dependencies, casclib)
