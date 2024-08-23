project "Deathmatch"
	language "C++"
	kind "SharedLib"
	targetname "deathmatch"
	targetdir(buildpath("server/mods/deathmatch"))

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	filter "system:windows"
		includedirs { "../../../vendor/sparsehash/src/windows" }

	filter {}
		includedirs {
			"../../../Shared/sdk",
			"../../sdk",
			"../../../vendor/bochs",
			"../../../vendor/pme",
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
			"../../../vendor/sparsehash/src/",
			"../../../vendor/DPP/discord/include",
			"logic",
			"utils",
			"."
		}

	defines { "SDK_WITH_BCRYPT" }
	undefines { "DPP_BUILD" }
	links {
		"Lua_Server", "sqlite", "ehs", "cryptopp", "pme", "pcre", "json-c", "zip", "glob", "zlib", "blowfish_bcrypt",
	}
	libdirs {
		buildpath('server'),
		buildpath('server/mods/deathmatch'),
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
		libdirs { buildpath('server/x64') }
	filter "platforms:arm"
		targetdir(buildpath("server/arm"))
		libdirs { buildpath('server/arm') }
	filter "platforms:arm64"
		targetdir(buildpath("server/arm64"))
		libdirs { buildpath('server/arm64') }
	
	filter { "configurations:Debug", "architecture:x64" }
		links { "discord-bot-x64_d.lib" }

	filter { "configurations:Release", "architecture:x64" }
		links { "discord-bot-x64.lib" }

	filter { "configurations:Debug", "architecture:x86" }
		links { "discord-bot-x86_d.lib" }

	filter { "configurations:Release", "architecture:x86" }
		links { "discord-bot-x86.lib" }