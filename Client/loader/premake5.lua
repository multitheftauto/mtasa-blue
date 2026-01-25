project "Loader"
	language "C++"
	kind "SharedLib"
	targetname "loader"
	targetdir(buildpath("mta"))
	disablewarnings {
		"4996", -- use of symbol with __declspec(deprecated)
	}

	includedirs {
		"../../Shared/sdk",
		"../sdk",
		"../../vendor",
		"../../vendor/detours/4.0.1/src",
	}

	links {
		"unrar", "d3d9",
		"detours", "Imagehlp",
		"DbgHelp", "Psapi",
		"../../vendor/nvapi/x86/nvapi.lib",
		"cryptopp",
	}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	forceincludes { "StdInc.h" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["Resources/*"] = {"*.rc", "**.bmp"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp",
		"../core/FastFailCrashHandler/WerCrashHandler.h",
		"../core/FastFailCrashHandler/WerCrashHandler.cpp"
	}

	filter "system:windows"
		files {
			"loader.rc",
			"resource/splash.bmp"
		}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
