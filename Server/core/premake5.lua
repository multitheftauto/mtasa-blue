project "Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	targetdir(buildpath("server"))
	
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

      
	filter "system:windows"
		libdirs {
			"../../vendor/detours/lib"
		}
		includedirs { "../../vendor/detours/include" }
		links { "detours", "Imagehlp" }
	