project "XML"
	language "C++"
	kind "SharedLib"
	targetname "xmll"
	targetdir(buildpath("server"))

	includedirs {
		"../sdk",
		"../../vendor/tinyxml"
	}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths {
		["Headers/**"] = "**.h",
		["Sources/**"] = "*.cpp",
		["*"] = "premake5.lua"
	}

	links { "tinyxml" }

	defines {
		"TIXML_USE_STL"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	filter {"system:windows", "platforms:x86"}
		postbuildcommands {
			copy "mta"
		}

	filter "system:linux"
		links { "rt" }

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))

	filter "platforms:arm"
		targetdir(buildpath("server/arm"))

	filter "platforms:arm64"
		targetdir(buildpath("server/arm64"))
