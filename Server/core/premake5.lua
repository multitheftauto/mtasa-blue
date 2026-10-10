project "Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	targetdir(buildpath("server"))
	clangtidy "On"

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/current/src/windows" }
		-- Server requires Windows 10+ (cpp-httplib)
		defines { "_WIN32_WINNT=0x0A00" }

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

	-- 32-bit Windows server is no longer supported
	filter { "system:windows", "platforms:x86" }
		flags { "ExcludeFromBuild" }
