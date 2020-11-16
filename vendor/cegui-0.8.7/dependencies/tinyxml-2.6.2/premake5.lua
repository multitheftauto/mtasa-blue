project "tinyxml-2.6.2"
	language "C++"
	kind "SharedLib"
	targetname "tinyxml-2.6.2"
	targetdir(buildpath("mta"))

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	defines {
		"tinyxml_EXPORTS"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.c",
		"**.h",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter {"system:windows"}
		linkoptions { "/ignore:4221" }
		disablewarnings { "4221" }