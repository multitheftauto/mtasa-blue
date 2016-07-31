project "Client Deathmatch"
	language "C++"
	kind "SharedLib"
	targetname "client"
	targetdir(buildpath("mods/deathmatch"))
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	links {
		"Lua_Client", "pcre", "json-c", "ws2_32", "portaudio", "zlib", 
		"../../../vendor/libspeex/libspeexd2013", -- todo: use debug variant
		"../../../vendor/bass/lib/bass",
		"../../../vendor/bass/lib/bass_fx",
		"../../../vendor/bass/lib/bassmix",
		"../../../vendor/bass/lib/tags"
	}
	
	vpaths { 
		["Headers/**"] = {"../**.h", "../../../Shared**.h"},
		["Sources/*"] = {"../**.cpp", "../**.c", "../../../Shared/**.cpp"},
		["*"] = "premake5.lua"
	}
	
	filter "system:windows"
		includedirs { "../../../vendor/sparsehash/current/src/windows" }
		linkoptions { "/SAFESEH\:NO" }
	
	filter {}
		includedirs {
			".",
			"./logic",
			"../../sdk/",
			"../../../vendor/pthreads/include", 
			"../../../vendor/bochs", 
			"../../../vendor/bass", 
			"../../../vendor/libspeex", 
			"../../../vendor/zlib", 
			"../../../vendor/pcre", 
			"../../../vendor/json-c", 
			"../../../vendor/bob_withers", 
			"../../../vendor/lua/src", 
			"../../../Shared/mods/deathmatch/logic", 
			"../../../Shared/animation", 
			"../../../vendor/sparsehash/current/src/"
	}
	
	files {
		"premake5.lua",
		"**.h",
		"**.cpp",
		"../../../Shared/mods/deathmatch/logic/**.cpp",
		"../../../Shared/mods/deathmatch/logic/**.h",
		"../../../Shared/animation/CEasingCurve.cpp", 
		"../../../Shared/animation/CPositionRotationAnimation.cpp",
		-- Todo: Replace these two by using the CryptoPP functions instead
		"../../../vendor/bob_withers/base64.cpp",
		"../../../vendor/bochs/bochs_internal/crc32.cpp"
	}
	
	defines {
		"LUA_USE_APICHECK"
	}
	
	configuration "windows"
		buildoptions { "-Zm130" }
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
		
	filter "system:not windows"
		flags { "ExcludeFromBuild" } 
