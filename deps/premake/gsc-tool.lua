gsc_tool = {
    source = path.join(dependencies.basePath, "gsc-tool/src")
}

function gsc_tool.import()
    links {"xsk-gsc-s1", "xsk-gsc-h1", "xsk-gsc-h2", "xsk-gsc-utils"}
    gsc_tool.includes()
end

function gsc_tool.includes()
    includedirs {
		gsc_tool.source,
        path.join(dependencies.basePath, "extra/gsc-tool")
    }
end

-- https://github.com/xensik/gsc-tool/blob/dev/premake5.lua#L95
function gsc_tool.project()
    project "xsk-gsc-utils"
    kind "StaticLib"
    language "C++"

    pchheader "stdafx.hpp"
    pchsource(path.join(gsc_tool.source, "utils/stdafx.cpp"))

    files {
        path.join(gsc_tool.source, "utils/**.h"),
        path.join(gsc_tool.source, "utils/**.hpp"),
        path.join(gsc_tool.source, "utils/**.cpp")
    }

    includedirs {
        path.join(gsc_tool.source, "utils"),
        gsc_tool.source
    }

    zlib.includes()
	
	project "xsk-gsc-s1"
    kind "StaticLib"
    language "C++"

    pchheader "stdafx.hpp"
    pchsource(path.join(gsc_tool.source, "s1/stdafx.cpp"))

    files {
        path.join(gsc_tool.source, "s1/**.h"),
        path.join(gsc_tool.source, "s1/**.hpp"),
        path.join(gsc_tool.source, "s1/**.cpp"),
        path.join(dependencies.basePath, "extra/gsc-tool/s1/interface.cpp")
    }

    includedirs {
        path.join(gsc_tool.source, "s1"),
        gsc_tool.source,
        path.join(dependencies.basePath, "extra/gsc-tool/s1")
    }
	
	filter "action:vs*"
    buildoptions "/bigobj"
    buildoptions "/Zc:__cplusplus"
    filter {}

    project "xsk-gsc-h1"
    kind "StaticLib"
    language "C++"

    pchheader "stdafx.hpp"
    pchsource(path.join(gsc_tool.source, "h1/stdafx.cpp"))

    files {
        path.join(gsc_tool.source, "h1/**.h"),
        path.join(gsc_tool.source, "h1/**.hpp"),
        path.join(gsc_tool.source, "h1/**.cpp"),
        path.join(dependencies.basePath, "extra/gsc-tool/h1/interface.cpp")
    }

    includedirs {
        path.join(gsc_tool.source, "h1"),
        gsc_tool.source,
        path.join(dependencies.basePath, "extra/gsc-tool/h1")
    }
	
	filter "action:vs*"
    buildoptions "/bigobj"
    buildoptions "/Zc:__cplusplus"
    filter {}

    project "xsk-gsc-h2"
    kind "StaticLib"
    language "C++"

    pchheader "stdafx.hpp"
    pchsource(path.join(gsc_tool.source, "h2/stdafx.cpp"))

    files {
        path.join(gsc_tool.source, "h2/**.h"),
        path.join(gsc_tool.source, "h2/**.hpp"),
        path.join(gsc_tool.source, "h2/**.cpp"),
        path.join(dependencies.basePath, "extra/gsc-tool/h2/interface.cpp")
    }

    includedirs {
        path.join(gsc_tool.source, "h2"),
        gsc_tool.source,
        path.join(dependencies.basePath, "extra/gsc-tool/h2")
    }
	
    filter "action:vs*"
    buildoptions "/bigobj"
    buildoptions "/Zc:__cplusplus"
    filter {}
end

table.insert(dependencies, gsc_tool)
