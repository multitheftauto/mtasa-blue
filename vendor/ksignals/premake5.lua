project "ksignals"
	language "C++"
	kind "StaticLib"
	targetname "ksignals"
	warnings "Off"

	vpaths {
		["Headers/*"] = "**.h",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
	}
