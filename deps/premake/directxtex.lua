directxtex = {
	source = path.join(dependencies.basePath, "DirectXTex/DirectXTex"),
	common = path.join(dependencies.basePath, "DirectXTex/Common")
}

function directxtex.import()
	links { "DirectXTex" }
	directxtex.includes()
end

function directxtex.includes()
	includedirs {
		directxtex.source,
		directxtex.common
	}
end

function directxtex.project()
	project "DirectXTex"
		language "C++"

		directxtex.includes()

		files {
			path.join(directxtex.common, "*.h"),
			path.join(directxtex.source, "*.h"),
			path.join(directxtex.source, "*.cpp"),
			path.join(directxtex.source, "*.inl"),
		}

		removefiles {
			path.join(directxtex.source, "BCDirectCompute.cpp"),
			path.join(directxtex.source, "BCDirectCompute.h"),
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, directxtex)
