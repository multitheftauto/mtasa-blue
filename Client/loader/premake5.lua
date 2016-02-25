project "Loader"
	language "C++"
	kind "SharedLib"
	targetname "loader"
	
	includedirs { 
		"../sdk",
		"../../vendor"
	}

	libdirs {
		"../../vendor/detours/lib"
	}

	links {
		"unrar", "d3d9"
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
	