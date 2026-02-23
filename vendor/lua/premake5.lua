project "Lua"
	language "C++"
	targetname "lua5.1"
	warnings "Off"

	vpaths {
		["Headers"] = "**.h",
		["Sources"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"src/**.c",
		"src/**.h",
	}

	kind "StaticLib"
