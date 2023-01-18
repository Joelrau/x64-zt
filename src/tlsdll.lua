tlsdll = {}
function tlsdll:project()
    project "tlsdll"
		kind "SharedLib"
		language "C++"

		files {
			"./src/tlsdll/**.rc", 
			"./src/tlsdll/**.hpp", 
			"./src/tlsdll/**.cpp", 
			"./src/tlsdll/resources/**.*"
		}

		includedirs {
			"./src/tlsdll", 
			"%{prj.location}/src"
		}

		links {"shared"}

		resincludedirs {"$(ProjectDir)src"}
end