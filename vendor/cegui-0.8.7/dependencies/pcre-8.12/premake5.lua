project "pcre-8.12"
	language "C++"
	kind "SharedLib"
	targetname "pcre"
	targetdir(buildpath("mta"))

	includedirs {
		"."
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	defines {
		"SUPPORT_UTF8",
		"HAVE_CONFIG_H",
		"pcre_EXPORTS"
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