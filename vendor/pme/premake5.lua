project "pme"
	language "C++"
	kind "StaticLib"
	targetname "pme"

	includedirs { "../pcre" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",

		"pme.h",
		"pme.cpp"
	}
