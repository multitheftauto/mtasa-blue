project "lunasvg"
	language "C++"
	kind "StaticLib"
	targetname "lunasvg"
	targetdir(buildpath("mta"))
	floatingpoint "Fast"
	rtti "Off"
	warnings "Off"

	defines {
		"PLUTOVG_BUILD",
		"LUNASVG_BUILD",
		"_CRT_SECURE_NO_WARNINGS"
	}

	vpaths {
		["Headers"] = "source/**.h",
		["Headers/*"] = "include/**.h",
		["Headers/plutovg/*"] = "plutovg/**.h",
		["Sources"] = "source/**.cpp",
        ["Sources/*"] = "source/**.c",
		["Sources/plutovg"] = "plutovg/**.cpp",
		["Sources/plutovg/*"] = "plutovg/**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.h",
		"**.c"
	}

	includedirs {
		"plutovg/include",
		"include",
		"source"
	}

	filter "system:windows"
		disablewarnings {
			"4244", -- warning C4244: '=': conversion from '?' to '?', possible loss of data
			"4018", -- warning C4018: '<': signed/unsigned mismatch
		}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
