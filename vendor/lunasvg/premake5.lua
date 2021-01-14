project "lunasvg"
	language "C++"
	kind "StaticLib"
	targetname "lunasvg"

	includedirs {
		"include",
		"source",
		"source/graphics",
		"source/geometry",
		"3rdparty/plutovg",
		"3rdparty/software",
		"3rdparty/stb"
	}

	defines {
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
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
