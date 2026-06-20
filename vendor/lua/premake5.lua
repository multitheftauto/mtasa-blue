project "Lua"
	language "C++"
	kind "StaticLib"
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
