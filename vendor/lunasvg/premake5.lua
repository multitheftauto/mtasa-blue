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
		["Headers/3rdparty/*"] = "3rdparty/**.h",
		["Sources"] = "source/**.cpp",
        ["Sources/*"] = "source/**.c",
		["Sources/3rdparty"] = "3rdparty/**.cpp",
		["Sources/3rdparty/*"] = "3rdparty/**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.h",
		"**.c"
	}

	includedirs {
		"3rdparty/plutovg",
		"source",
		"include"
	}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }
