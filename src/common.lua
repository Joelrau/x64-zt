common = {}
function common:project()
    project "common"
		kind "StaticLib"
		language "C++"

		files {
			"./src/common/**.hpp", 
			"./src/common/**.cpp"
		}

		includedirs {
			"./src/common", 
			"%{prj.location}/src"
		}

		resincludedirs {"$(ProjectDir)src"}

		dependencies.imports()
end