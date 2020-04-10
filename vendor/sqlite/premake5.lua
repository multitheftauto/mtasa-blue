project "sqlite"
	language "C"
	kind "StaticLib"
	targetname "sqlite"

	vpaths {
		["Headers/*"] = "*.h",
		["Sources/*"] = "*.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.c"
	}

	filter {"system:windows"}
		disablewarnings { "4996" }
