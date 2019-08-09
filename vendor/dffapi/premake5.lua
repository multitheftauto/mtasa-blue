project "dffapi"
	language "C++"
	kind "StaticLib"
	targetname "dffapi"

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp",
	}
