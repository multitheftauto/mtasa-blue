project "fbx"
	language "C++"
	kind "StaticLib"
	targetname "fbx"

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp",
		"*.c",
	}
