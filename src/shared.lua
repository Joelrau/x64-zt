shared = {}
function shared:project()
    project "shared"
		kind "StaticLib"
		language "C++"

		files {
			"./src/shared/**.hpp", 
			"./src/shared/**.cpp"
		}

		includedirs {
			"./src/shared", 
			"%{prj.location}/src"
		}

		resincludedirs {"$(ProjectDir)src"}

		dependencies.imports()
end