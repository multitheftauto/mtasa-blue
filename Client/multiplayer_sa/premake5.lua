project "Multiplayer SA"
	language "C++"
	kind "SharedLib"
	targetname "multiplayer_sa"
	targetdir(buildpath("mta"))
	
	filter "system:windows"
		includedirs { "../../vendor/sparsehash/current/src/windows" }
	
	filter {}
		includedirs { 
			"../sdk",
			"../../vendor/sparsehash/current/src/"
		}
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
	
	filter "system:not windows"
		flags { "ExcludeFromBuild" } 