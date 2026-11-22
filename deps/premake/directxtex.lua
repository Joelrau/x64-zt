directxtex = {
	source = path.join(dependencies.basePath, "DirectXTex/DirectXTex"),
	common = path.join(dependencies.basePath, "DirectXTex/Common"),
	shaders = path.join(_MAIN_SCRIPT_DIR, "build/generated/directxtex_shaders")
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

		includedirs {directxtex.shaders}

		local shaders = path.translate(directxtex.shaders)
		local shader_source = path.translate(path.getabsolute(path.join(directxtex.source, "Shaders")))
		prebuildcommands {
			"if not exist \"" .. shaders .. "\\BC6HEncode_TryModeLE10CS_cs40.inc\" (",
			"set \"CompileShadersOutput=" .. shaders .. "\"",
			"pushd \"" .. shader_source .. "\"",
			"call \"" .. shader_source .. "\\CompileShaders.cmd\" || exit /b 1",
			"popd",
			")",
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, directxtex)
