project "lunasvg"
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"
	targetname "lunasvg"
	targetdir(buildpath("mta"))
	floatingpoint "Fast"
	rtti "Off"

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

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }
