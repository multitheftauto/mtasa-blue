project "Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	targetdir(buildpath("server"))

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/current/src/windows" }
		linkoptions { "/SAFESEH:NO" }

	filter {}
		includedirs {
			"../../Shared/sdk",
			"../sdk",
			"../../vendor/google-breakpad/src",
			"../../vendor/sparsehash/current/src/",
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

	filter { "system:windows", "platforms:x86" }
		includedirs {
			"../../vendor/detours/4.0.1/src"
		}
		links { "detours", "Imagehlp" }

	filter "system:not windows"
		excludes { "CExceptionInformation_Impl.cpp" }

	filter "system:linux"
		links { "breakpad", "rt" }
		buildoptions { "-pthread" }
		linkoptions { "-pthread" }
		linkoptions { "-l:libncursesw.so.6" }

	filter "system:macosx"
		links { "ncurses", "breakpad", "CoreFoundation.framework" }
		buildoptions { "-pthread" }
		linkoptions { "-pthread" }

		-- This makes ncurses `get_wch` work
		defines { "_XOPEN_SOURCE_EXTENDED=1" }

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))

	filter "platforms:arm"
		targetdir(buildpath("server/arm"))

	filter "platforms:arm64"
		targetdir(buildpath("server/arm64"))
