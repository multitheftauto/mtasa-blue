project "Client Webbrowser"
	language "C++"
	kind "SharedLib"
	targetname "cefweb"
	targetdir(buildpath("mta"))

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }
		linkoptions { "/SAFESEH:NO" }
		buildoptions { "-Zm130" }

	filter {}
		includedirs {
			"../../Shared/sdk",
			".",
			"../sdk",
			"../../vendor/cef3/cef",
			"../../vendor/sparsehash/src/"
		}

	libdirs {
		"../../vendor/cef3/cef/Release"
	}


	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.h",
		"**.cpp"
	}

	links {
		"libcef", "CEF", "Psapi.lib", "version.lib", "Winmm.lib", "Ws2_32.lib", "DbgHelp.lib"
	}

	defines {
		"PSAPI_VERSION=1"
	}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
