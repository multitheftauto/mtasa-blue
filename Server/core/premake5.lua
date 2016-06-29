project "Core"
	language "C++"
	kind "SharedLib"
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
	
	configuration "windows"
		includedirs { "../../vendor/detours/include" }
		links { "../../vendor/detours/lib/detours.lib", "Imagehlp.lib" }
	