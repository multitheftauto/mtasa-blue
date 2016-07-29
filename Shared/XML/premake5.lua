project "XML"
	language "C++"
	kind "SharedLib"
	targetname "xmll"
	targetdir(buildpath("server"))
	
	includedirs { 
		"../sdk",
		"../../vendor/tinyxml"
	}
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	vpaths { 
		["Headers/**"] = "**.h",
		["Sources/**"] = "*.cpp",
		["*"] = "premake5.lua"
	}
	
	links { "tinyxml" }
	
	defines {
		"TIXML_USE_STL"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	
	filter "system:windows"
		postbuildcommands {
			copy "mta"
		}
	
