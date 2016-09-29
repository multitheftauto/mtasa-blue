project "Deathmatch"
	language "C++"
	kind "SharedLib"
	targetname "deathmatch"
	targetdir(buildpath("server/mods/deathmatch"))

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	filter "system:windows"
		includedirs { "../../../vendor/sparsehash/current/src/windows" }
	
	filter {}
		includedirs { 
			"../../sdk", 
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
			"../../../vendor/sparsehash/current/src/",
			"logic", 
			"utils",
			".",
			"../../../Shared/gta"
		}
	
	links {
		"Lua_Server", "pthread", "sqlite", "ehs", "cryptopp", "pme", "pcre", "json-c", "zip", "zlib"
	}
	
	vpaths {
		["Headers/*"] = {"**.h", "../../../Shared/mods/deathmatch/**.h", "../../../Server/**.h", "../../../**.h"},
		["Sources/*"] = {"**.cpp", "../../../Shared/mods/deathmatch/**.cpp", "../../../Shared/**.cpp", "../../../vendor/**.cpp", "../../../Server/**.cpp", "../../../**.cpp"},
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"**.h",
		"**.cpp",
		"../../../Shared/mods/deathmatch/logic/**.cpp",
		"../../../Shared/mods/deathmatch/logic/**.h",
		"../../../Shared/animation/CEasingCurve.cpp", 
		"../../../Shared/animation/CPositionRotationAnimation.cpp",
		"../../sdk/MTAPlatform.cpp",
		"../../sdk/MTAPlatform.h",
		"../../version.h",
		-- Todo: Replace these two by using the CryptoPP functions instead
		"../../../vendor/bochs/bochs_internal/crc32.cpp",
	}
	
	filter "system:windows"
		includedirs { "../../../vendor/pthreads/include" }
		buildoptions { "-Zm130" }
		links { "ws2_32" }

	filter {"system:windows", "toolset:*120*"}
		links { "Psapi.lib" }
		
	filter "system:not windows"
		buildoptions { "-Wno-narrowing" } -- We should fix the warnings at some point
		links { "rt" }
	
	filter "platforms:x64"
		targetdir(buildpath("server/x64"))
