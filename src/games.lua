games = {
	known = {"h1", "iw7"},
	enabled = {},
	dependencies = {
		h1 = function()
			dependencies.use(directxtex)
			gsc_tool.import_engine("h1")
		end,
		iw7 = function()
			dependencies.use(directxtex)
			gsc_tool.import_engine("iw7")
		end,
	},
}

local function select_games()
	local requested = _OPTIONS["games"] or "h1"
	if requested == "all" then
		return games.known
	end

	local selected = {}
	for name in requested:gmatch("[^,%s]+") do
		if not table.contains(games.known, name) then
			error("Unknown game \"" .. name .. "\". Known games: " .. table.concat(games.known, ", "))
		end
		table.insert(selected, name)
	end
	return selected
end

games.enabled = select_games()

function games.converters()
	if _OPTIONS["no-convert"] then
		return {}
	end

	local converters = {}
	for _, dir in ipairs(os.matchdirs("src/convert/*")) do
		local source, target = path.getname(dir):match("^(%w+)_(%w+)$")
		if source and table.contains(games.enabled, source) and table.contains(games.enabled, target) then
			table.insert(converters, {source = source, target = target})
		end
	end
	return converters
end

function games.write_enabled_header()
	local lines = {"#pragma once", ""}

	local game_list = {}
	for _, name in ipairs(games.enabled) do
		table.insert(game_list, "x(" .. name .. ")")
	end
	table.insert(lines, "#define ZONETOOL_GAMES(x) " .. table.concat(game_list, " "))

	local converter_list = {}
	for _, converter in ipairs(games.converters()) do
		table.insert(converter_list, "x(" .. converter.source .. ", " .. converter.target .. ")")
	end
	table.insert(lines, "#define ZONETOOL_CONVERTERS(x) " .. table.concat(converter_list, " "))

	local content = table.concat(lines, "\n") .. "\n"
	local folder = path.join(_MAIN_SCRIPT_DIR, "build/generated")
	local file = path.join(folder, "enabled_games.hpp")

	if io.readfile(file) ~= content then
		os.mkdir(folder)
		io.writefile(file, content)
	end
end

local function game_project(name)
	project("game-" .. name)
		kind "StaticLib"
		language "C++"

		pchheader "std_include.hpp"
		pchsource("src/games/" .. name .. "/std_include.cpp")

		files {
			"src/games/" .. name .. "/**.hpp",
			"src/games/" .. name .. "/**.cpp",
		}

		includedirs {
			"src/games/" .. name,
			"src/games",
			"src/core",
			"src/common",
			"src",
		}

		links {"core", "common"}

		core.includes()
		games.dependencies[name]()
end

local function converter_project(converter)
	local name = converter.source .. "_" .. converter.target

	project("convert-" .. converter.source .. "-" .. converter.target)
		kind "StaticLib"
		language "C++"

		pchheader "std_include.hpp"
		pchsource("src/convert/" .. name .. "/std_include.cpp")

		files {
			"src/convert/" .. name .. "/**.hpp",
			"src/convert/" .. name .. "/**.cpp",
		}

		includedirs {
			"src/convert/" .. name,
			"src/games",
			"src/core",
			"src/common",
			"src",
		}

		links {"game-" .. converter.source, "game-" .. converter.target}

		core.includes()
		games.dependencies[converter.source]()
		games.dependencies[converter.target]()
end

function games.projects()
	group "Games"
	for _, name in ipairs(games.enabled) do
		game_project(name)
	end

	group "Converters"
	for _, converter in ipairs(games.converters()) do
		converter_project(converter)
	end

	group ""
end

function games.import()
	for _, name in ipairs(games.enabled) do
		links {"game-" .. name}
		games.dependencies[name]()
	end

	for _, converter in ipairs(games.converters()) do
		links {"convert-" .. converter.source .. "-" .. converter.target}
	end
end
