-- Workaround for `kind "None"` only being supported for Visual Studio.
if _ACTION == "gmake" then
	return
end

project "Server SDK"
	language "C++"
	kind "None"
	targetname "sdk"

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
