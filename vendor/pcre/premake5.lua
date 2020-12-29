project "pcre"
	language "C++"
	targetname "pcre3"

	defines { "HAVE_CONFIG_H" }
	includedirs { "." }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = {"**.c", "**.cc"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.c",
		"*.cc",
		"*.h"
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

	filter "system:not windows"
		defines { "HAVE_STRTOLL" }
		kind "StaticLib"

	filter {"system:windows"}
		linkoptions { "/ignore:4217", "/ignore:4049" }
		disablewarnings { "4251" }
