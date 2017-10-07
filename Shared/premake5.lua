project "Shared"
	language "C++"
	kind "StaticLib"
	targetname "shared"
		
	vpaths { 
		["Headers/*"] = { "**.h", "**.hpp" },
		["*"] = "premake5.lua",
	}
		
	files {
		"premake5.lua",
		"**.h",
		"**.hpp",
	}

	removefiles {
		"mods/**",
		"publicsdk/**",
		"XML/**",
	}

	flags { "ExcludeFromBuild" } 
