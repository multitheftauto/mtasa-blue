project "discordgsdk"
	language "C++"
	kind "StaticLib"
	targetname "discordgsdk"

	vpaths {
		["Headers/*"] = "cpp/*.h",
		["Sources/*"] = "cpp/*.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"cpp/*.cpp",
		"cpp/*.h",
	}
