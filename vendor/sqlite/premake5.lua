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

	defines { "SQLITE_ENABLE_MATH_FUNCTIONS", "SQLITE_ENABLE_JSON1" }

	filter {"system:windows"}
		disablewarnings { "4996" }
