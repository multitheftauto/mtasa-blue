project "Client Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	
	includedirs { 
		"../sdk",
		"../../vendor/cef3",
		"../../vendor/tinygettext",
		"../../vendor/zlib",
		"../../vendor/jpeg/jpeg-8d",
		"../../vendor/pthreads/include" 
	}

	libdirs {
		"../../vendor/detours/lib"
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
	