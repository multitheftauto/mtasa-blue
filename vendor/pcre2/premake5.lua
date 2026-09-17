project "pcre2"
	language "C++"
	targetname "pcre2"
	warnings "Off"

	defines {
		"HAVE_CONFIG_H",
		"PCRE2_CODE_UNIT_WIDTH=8",
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs { "." }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.c",
		"*.h"
	}

	-- Exclude files we don't need (test programs, grep, etc.)
	excludes {
		"pcre2test.c",
		"pcre2grep.c",
		"pcre2posix_test.c",
		"pcre2posix.c",
		"pcre2posix.h",
		"pcre2_fuzzsupport.c",
		"pcre2_dftables.c",
		"pcre2_jit_test.c"
	}

	filter "system:windows"
		kind "SharedLib"
		targetdir(buildpath("server/mods/deathmatch"))

	filter {"system:windows", "platforms:x86"}
		postbuildcommands {
			copy "mods/deathmatch"
		}

	filter {"system:windows", "platforms:x64"}
		targetdir(buildpath("server/x64"))

	filter {"system:windows", "platforms:arm"}
		targetdir(buildpath("server/arm"))

	filter {"system:windows", "platforms:arm64"}
		targetdir(buildpath("server/arm64"))

	filter "system:not windows"
		defines { "PCRE2_STATIC", "HAVE_STRTOLL" }
		kind "StaticLib"

	filter "system:windows"
		linkoptions { "/ignore:4217", "/ignore:4049" }
		disablewarnings { "4251" }
