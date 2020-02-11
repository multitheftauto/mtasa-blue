project "Client Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	targetdir(buildpath("mta"))

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }
		linkoptions { "/SAFESEH:NO" }
		buildoptions { "-Zm130" }

	filter {}
		includedirs {
			".",
			"../sdk",
			"../../vendor/tinygettext",
			"../../vendor/zlib",
			"../../vendor/jpeg-9b",
			"../../vendor/pthreads/include",
			"../../vendor/sparsehash/src/",
			"../../vendor/hwbrk",
			"../../vendor/discordgsdk/cpp"
		}

	libdirs {
		"../../vendor/detours/lib",
		"../../vendor/discordgsdk/lib/x86",
	}


	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"**.rc", "../launch/resource/mtaicon.ico"},
		["*"] = "premake5.lua"
	}

	links { "hwbrk" }

	files {
		"premake5.lua",
		"../launch/resource/mtaicon.ico",
		"core.rc",
		"**.h",
		"**.cpp"
	}

	links {
		"ws2_32", "d3dx9", "Userenv", "DbgHelp", "xinput", "Imagehlp", "dxguid", "dinput8",
		"strmiids",	"odbc32", "odbccp32", "shlwapi", "winmm", "gdi32", "Imm32", "Psapi",
		"pthread", "libpng", "jpeg", "zlib", "tinygettext", "detours", "discordgsdk", "discord_game_sdk.dll"
	}

	defines {
		"INITGUID",
		"PNG_SETJMP_NOT_SUPPORTED"
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
