project "Server_Core"
	language "C++"
	kind "ConsoleApp"
	targetname "core"
	
	includedirs { "../sdk", "../../vendor/google-breakpad/src" }
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	-- TODO: Link Breakpad
	
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
	