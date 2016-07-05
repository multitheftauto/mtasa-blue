project "Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	targetdir(buildpath("server"))
	
	includedirs { 
		"../sdk", 
		"../../vendor/google-breakpad/src",
		"../../vendor/sparsehash/current/src/",
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
		includedirs { 
			"../../vendor/detours/include",
			"../../vendor/sparsehash/current/src/windows"
		}
		links { "detours", "Imagehlp" }
	
	filter "system:not windows"
		excludes { "CExceptionInformation_Impl.cpp" }