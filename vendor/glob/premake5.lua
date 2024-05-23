project "glob"
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"
	targetname "glob"

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
