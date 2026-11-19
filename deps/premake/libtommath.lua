libtommath = {
	source = path.join(dependencies.basePath, "libtommath"),
}

function libtommath.import()
	links {
		"libtommath"
	}

	libtommath.includes()
end

function libtommath.includes()
	includedirs {
		libtommath.source
	}

	defines {
		"LTM_DESC",
		"__STDC_IEC_559__",
		"MP_NO_DEV_URANDOM",
	}
end

function libtommath.project()
	project "libtommath"
		language "C"

		libtommath.includes()

		files {
			path.join(libtommath.source, "*.c"),
		}

		defines {
			"_LIB"
		}

		removedefines {
			"_DLL",
			"_USRDLL"
		}

		linkoptions {
			"-IGNORE:4221"
		}

		-- libtommath relies on dead code elimination (MP_HAS), which /Od doesn't do
		optimize "Speed"
		runtimechecks "Off" -- /RTC1 is incompatible with /O2

		-- keep the CRT matching the rest of the build (optimize would otherwise pick the release CRT)
		filter "configurations:Debug"
			runtime "Debug"
		filter "configurations:Release"
			runtime "Release"
		filter {}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, libtommath)
