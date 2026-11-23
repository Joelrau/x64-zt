umbra3 = {
	source = path.join(dependencies.basePath, "umbra3"),
}

function umbra3.import()
	links { "umbra3" }
	umbra3.includes()
end

function umbra3.includes()
	includedirs {
		path.join(umbra3.source, "interface"),
		path.join(umbra3.source, "interface/runtime"),
		path.join(umbra3.source, "interface/optimizer"),
		path.join(umbra3.source, "source"),
		path.join(umbra3.source, "source/common"),
		path.join(umbra3.source, "source/runtime"),
		path.join(umbra3.source, "source/optimizer"),
		path.join(umbra3.source, "source/standard"),
	}
end

function umbra3.project()
	project "umbra3"
		language "C++"
		kind "StaticLib"
		characterset "MBCS"

		-- The SDK predates C++20 comparison rewriting, and the optimizer is unusably slow unoptimized.
		cppdialect "C++14"
		optimize "Speed"
		runtime "Release"
		removedefines { "_DEBUG", "DEBUG" }
		defines { "NDEBUG" }

		umbra3.includes()

		files {
			path.join(umbra3.source, "interface/**.hpp"),
			path.join(umbra3.source, "source/common/*.cpp"),
			path.join(umbra3.source, "source/common/*.hpp"),
			path.join(umbra3.source, "source/common/windows/*.cpp"),
			path.join(umbra3.source, "source/common/windows/*.inl"),
			path.join(umbra3.source, "source/standard/*.cpp"),
			path.join(umbra3.source, "source/standard/*.hpp"),
			path.join(umbra3.source, "source/runtime/*.cpp"),
			path.join(umbra3.source, "source/runtime/*.hpp"),
			path.join(umbra3.source, "source/optimizer/*.cpp"),
			path.join(umbra3.source, "source/optimizer/*.hpp"),
		}

		-- umbra_stub_allocator.cpp replaces the SDK heap and defines the same symbols.
		removefiles {
			path.join(umbra3.source, "source/common/umbraMemory.cpp"),
		}

		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_WARNINGS",
			"UMBRA_UNLOCKED",
			"UMBRA_IW7_CELL_EXIT_PORTALS",
		}

		warnings "Off"
end

table.insert(dependencies, umbra3)
