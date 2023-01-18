zstd = {
	source = path.join(dependencies.basePath, "zstd"),
}

function zstd.import()
	links { "zstd" }
	zstd.includes()
end

function zstd.includes()
	includedirs {
		path.join(zstd.source, "lib"),
		path.join(zstd.source, "lib/common")
	}
end

function zstd.project()
	project "zstd"
		language "C"

		zstd.includes()

		files {
			path.join(zstd.source, "lib/**.h"),
			path.join(zstd.source, "lib/**.c"),
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, zstd)