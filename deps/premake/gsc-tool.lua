gsc_tool = {
	source = path.join(dependencies.basePath, "gsc-tool"),
	engines = {},
}

local engine_sources = {
	h1 = "h1",
	h2 = "h2",
	iw6 = "iw6_pc",
	iw7 = "iw7",
	s1 = "s1_pc",
}

function gsc_tool.import()
	links {"xsk-gsc-utils"}
	gsc_tool.includes()
end

function gsc_tool.import_engine(game)
	gsc_tool.engines[game] = true
	dependencies.used[gsc_tool] = true
	links {"xsk-gsc-" .. game, "xsk-gsc-common"}
	gsc_tool.import()
end

function gsc_tool.includes()
	includedirs {
		path.join(gsc_tool.source, "include"),
	}
end

local function engine_project(name)
	local engine = engine_sources[name]

	project("xsk-gsc-" .. name)
		kind "StaticLib"
		language "C++"
		warnings "Off"

		filter "action:vs*"
			buildoptions "/Zc:__cplusplus"
		filter {}

		files {
			path.join(gsc_tool.source, "include/xsk/gsc/engine/" .. engine .. ".hpp"),
			path.join(gsc_tool.source, "src/gsc/engine/" .. engine .. ".cpp"),
			path.join(gsc_tool.source, "src/gsc/engine/" .. engine .. "_code.cpp"),
			path.join(gsc_tool.source, "src/gsc/engine/" .. engine .. "_func.cpp"),
			path.join(gsc_tool.source, "src/gsc/engine/" .. engine .. "_meth.cpp"),
			path.join(gsc_tool.source, "src/gsc/engine/" .. engine .. "_token.cpp"),
		}

		gsc_tool.includes()
end

function gsc_tool.project()
	project "xsk-gsc-utils"
		kind "StaticLib"
		language "C++"
		warnings "Off"

		files {
			path.join(gsc_tool.source, "include/xsk/utils/*.hpp"),
			path.join(gsc_tool.source, "src/utils/*.cpp"),
		}

		gsc_tool.includes()
		zlib.includes()

	if next(gsc_tool.engines) == nil then
		return
	end

	project "xsk-gsc-common"
		kind "StaticLib"
		language "C++"
		warnings "Off"

		filter "action:vs*"
			buildoptions "/Zc:__cplusplus"
		filter {}

		files {
			path.join(gsc_tool.source, "include/xsk/stdinc.hpp"),
			path.join(gsc_tool.source, "src/gsc/*.cpp"),
			path.join(gsc_tool.source, "src/gsc/common/*.cpp"),
			path.join(gsc_tool.source, "include/xsk/gsc/common/*.hpp"),
		}

		gsc_tool.includes()

	local names = table.keys(gsc_tool.engines)
	table.sort(names)

	for _, name in ipairs(names) do
		engine_project(name)
	end
end

table.insert(dependencies, gsc_tool)
