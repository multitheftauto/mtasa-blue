project "pme"
	language "C++"
	kind "StaticLib"
	targetname "pme"
	warnings "Off"

	includedirs { "../pcre2" }

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
