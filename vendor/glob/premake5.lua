project "glob"
	language "C++"
	kind "StaticLib"
	targetname "glob"
	warnings "Off"

	vpaths {
		["Headers/*"] = "include/**.h",
		["Sources"] = "source/**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.h",
	}

	includedirs {
		"source",
		"include"
	}
