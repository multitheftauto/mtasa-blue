project "Game SA"
	language "C++"
	kind "SharedLib"
	targetname "game_sa"
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["*"] = "premake5.lua"
	}
	
	includedirs {
		"../sdk"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	