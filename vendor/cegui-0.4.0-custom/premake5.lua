project "CEGUI"
	language "C++"
	kind "StaticLib"
	targetname "CEGUI"

	pchheader "StdInc.h"
	pchsource "src/StdInc.cpp"

	includedirs {
		"include",
		"dependencies/include",
		"../freetype/include"
	}

	links { "freetype" }

	defines {
		"CEGUIBASE_EXPORTS",
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	vpaths {
		["Headers/*"] = "include/**.h",
		["Sources/*"] = "src/**.cpp",
		["Sources/*"] = "src/**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"src/**.cpp",
		"src/**.c",
		"include/**.h",
	}

	excludes {
		"src/renderers/**",
		"src/pcre/ucptypetable.c",
		"src/pcre/ucptable.c",
		"src/pcre/ucp.c"
	}

	filter "files:src/tinyxml/**.cpp"
		flags { "NoPCH" }
	filter "files:src/pcre/**.c"
		flags { "NoPCH" }

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }
	
	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter {"system:windows"}
		linkoptions { "/ignore:4221" }
		disablewarnings { "4221" }
