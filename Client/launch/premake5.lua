project "Client Launcher"
	language "C++"
	kind "WindowedApp"
	targetname "Multi Theft Auto"
	
	includedirs { "../sdk" }
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	flags { "WinMain" }
	
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
	