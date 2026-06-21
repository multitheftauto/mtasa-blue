project "Deathmatch"
	language "C++"
	kind "SharedLib"
	targetname "deathmatch"
	targetdir(buildpath("server/mods/deathmatch"))
	clangtidy "On"

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	filter "system:windows"
		includedirs { "../../../vendor/sparsehash/src/windows" }
		-- cpp-httplib (included via CHTTPD.cpp) requires Windows 10+ APIs and enforces _WIN32_WINNT >= 0x0A00
		removedefines { "_WIN32_WINNT=0x601" }
		defines { "_WIN32_WINNT=0x0A00" }

	filter {}
		includedirs {
			"../../../Shared/sdk",
			"../../sdk",
			"../../../vendor/bochs",
			"../../../vendor/pme",
			"../../../vendor/cpp-httplib",
			"../../../vendor/zip",
			"../../../vendor/glob/include",
			"../../../vendor/zlib",
			"../../../vendor/pcre",
			"../../../vendor/json-c",
			"../../../vendor/lua/src",
			"../../../Shared/gta",
			"../../../Shared/mods/deathmatch/logic",
			"../../../Shared/animation",
			"../../../Shared/publicsdk/include",
			"../../../Shared",
			"../../../vendor/sparsehash/src/",
			"logic",
			"utils",
			"."
		}

	defines { "SDK_WITH_BCRYPT" }
	links {
		"Lua_Server", "sqlite", "cryptopp", "pme", "pcre", "json-c", "zip", "glob", "zlib", "blowfish_bcrypt",
	}

	vpaths {
		["Headers/*"] = {"**.h", "../../../Shared/mods/deathmatch/**.h", "../../**.h"},
		["Sources/*"] = {"**.cpp", "../../../Shared/mods/deathmatch/**.cpp", "../../../Shared/**.cpp", "../../../vendor/**.cpp", "../../**.cpp"},
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
		-- Todo: Replace these two by using the CryptoPP functions instead
		"../../../vendor/bochs/bochs_internal/bochs_crc32.cpp",
	}

	filter "system:windows"
		includedirs { "../../../vendor/pthreads/include" }
		buildoptions { "-Zm130" }
		links { "ws2_32", "pthread" }

	filter "system:not windows"
		buildoptions { "-Wno-narrowing" } -- We should fix the warnings at some point
		buildoptions { "-pthread" }
		linkoptions { "-pthread" }

	filter "system:linux"
		links { "rt" }

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))

	filter "platforms:arm"
		targetdir(buildpath("server/arm"))

	filter "platforms:arm64"
		targetdir(buildpath("server/arm64"))
