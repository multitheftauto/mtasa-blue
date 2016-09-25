project "XML"
	language "C++"
	kind "SharedLib"
	targetname "xmll"
	targetdir(buildpath("server"))
	
	includedirs { 
		"../sdk",
		"../../vendor/pugixml/src"
	}
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	vpaths { 
		["Headers/**"] = "**.h",
		["Sources/**"] = "*.cpp",
		["*"] = "premake5.lua"
	}
	
	links { "pugixml" }
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	
	filter {"system:windows", "platforms:x86"}
		postbuildcommands {
			copy "mta"
		}
	
	filter "platforms:x64"
		targetdir(buildpath("server/x64"))
