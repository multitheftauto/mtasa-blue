-- Workaround for `kind "None"` only being supported for Visual Studio.
if _ACTION == "gmake" then
	return
end

project "Shared"
	language "C++"
	kind "None"
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
