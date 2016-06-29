project "Deathmatch"
	language "C++"
	kind "SharedLib"
	targetname "deathmatch"
	
	includedirs { 
		"../../sdk", 
		"../../../vendor/pthreads/include", 
		"../../../vendor/bochs", 
		"../../../vendor/pme", 
		"../../../vendor/zip", 
		"../../../vendor/zlib", 
		"../../../vendor/pcre", 
		"../../../vendor/json-c", 
		"../../../vendor/bob_withers", 
		"../../../vendor/lua/src", 
		"../../../Shared/mods/deathmatch/logic", 
		"../../../Shared/animation", 
		"../../../Shared/publicsdk/include", 
		"logic", 
		"utils",
		""
	}
	
	links {
		"Lua", "pthread", "ws2_32"
	}
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"**.h",
		"**.cpp",
		"../../../Shared/mods/deathmatch/logic/**.cpp",
		"../../../Shared/mods/deathmatch/logic/**.h",
	}
	
	configuration "windows"
		buildoptions { "-Zm130" }
	