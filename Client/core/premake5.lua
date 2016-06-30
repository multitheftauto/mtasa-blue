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
		"../../vendor/pthreads/include",
		"../../vendor/sparsehash/current/src/",
		"../../vendor/sparsehash/current/src/windows"
	}

	libdirs {
		"../../vendor/detours/lib"
	}
	
	links {
		"../../vendor/cef3/Debug/cef_sandbox"
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
	