project "Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	
	includedirs { 
		"../sdk", 
		"../../vendor/google-breakpad/src",
		"../../vendor/sparsehash/current/src/",
		"../../vendor/sparsehash/current/src/windows"
	}
	
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

	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
      
	filter "configurations: windows"
		includedirs { "../../vendor/detours/include" }
		links { "../../vendor/detours/lib/detours.lib", "Imagehlp" }
	