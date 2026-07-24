project "tinyxml2"
	language "C++"
	kind "StaticLib"
	targetname "tinyxml2"
	warnings "Off"

	includedirs {
		"../../Shared/sdk",
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
