project "Client Launcher"
	language "C++"
	kind "WindowedApp"
	targetname "Multi Theft Auto"
	targetdir(buildpath("."))
	debugdir(buildpath("."))

	includedirs {
		"../../Shared/sdk",
		"../sdk",
	}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	entrypoint "WinMainCRTStartup"

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"*.rc", "**.ico", "**.xml"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	filter "system:windows"
		staticruntime "On"
		files {
			"NEU/gameicon_NEU.ico",
			"NEU/Multi Theft Auto.gdf.xml",
			"launch.rc",
			"Multi Theft Auto.rc",
			"resource/mtaicon.ico"
		}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
