-- This file is inspired by: https://github.com/premake/premake-core/blob/master/contrib/curl/premake5.lua
project "fmt"
	language "C++"
	kind "StaticLib"
	targetname "fmt"

	includedirs { "include/fmt", "src" }

	vpaths {
		["Includes/*"] = "include/fmt/**",
		["Sources/*"] = "src/**",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"include/fmt/**",
		"src/**"
	}	
