project "Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	targetdir(buildpath("server"))
	
	filter "system:windows"
		includedirs { "../../vendor/sparsehash/current/src/windows" }
		linkoptions { "/SAFESEH\:NO" }
	
	filter {}
		includedirs { 
			"../sdk", 
			"../../vendor/google-breakpad/src",
			"../../vendor/sparsehash/current/src/",
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

      
	filter "system:windows"
		libdirs {
			"../../vendor/detours/lib"
		}
		includedirs { 
			"../../vendor/detours/include"
		}
		links { "detours", "Imagehlp" }
	
	filter "system:not windows"
		excludes { "CExceptionInformation_Impl.cpp" }

	filter "system:linux"
		links { "ncursesw", "breakpad", "rt" }
		buildoptions { "-pthread" }
		linkoptions { "-pthread" }
	
	filter "platforms:x64"
		targetdir(buildpath("server/x64"))
