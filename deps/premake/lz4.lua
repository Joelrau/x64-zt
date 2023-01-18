lz4 = {
	source = path.join(dependencies.basePath, "lz4"),
}

function lz4.import()
	links { "lz4" }
	lz4.includes()
end

function lz4.includes()
	includedirs {
		path.join(lz4.source, "lib"),
	}

	defines {
		"ZLIB_CONST",
	}
end

function lz4.project()
	project "lz4"
		language "C"

		lz4.includes()

		files {
			path.join(lz4.source, "lib/*.h"),
			path.join(lz4.source, "lib/*.c"),
		}

		defines {
			"_CRT_SECURE_NO_DEPRECATE",
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, lz4)
