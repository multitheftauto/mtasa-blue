project "GUI"
	language "C++"
	kind "SharedLib"
	targetname "gui"
	
	includedirs { 
		"../sdk",
		"../../vendor/cegui-0.4.0-custom/include"
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
	