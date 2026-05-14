stb = {
    source = path.join(dependencies.basePath, "stb"),
}

function stb.import()
    stb.includes()
end

function stb.includes()
    includedirs {
        stb.source
    }
end

function stb.project()
    project "stb"
        language "C++"

        stb.includes()

        files {
            path.join(stb.source, "*.h"),
            path.join(stb.source, "*.cpp"),
        }

        warnings "Off"
        kind "StaticLib"
end

table.insert(dependencies, stb)