project "Client SDK"
	language "C++"
	kind "StaticLib"
	targetname "sdk"
	clangtidy "On"

	vpaths {
		["Headers/*"] = { "**.h", "**.hpp" },
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.h",
		"**.hpp",
	}

	flags { "ExcludeFromBuild" }
