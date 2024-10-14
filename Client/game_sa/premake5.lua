project "Game SA"
	language "C++"
	kind "SharedLib"
	targetname "game_sa"
	targetdir(buildpath("mta"))

	cppdialect "C++20"

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths {
		["Headers/*"] = { "**.h", "**.hpp" },
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }

	filter {}

	includedirs {
		"../../Shared/sdk",
		"../sdk/",
		"../../vendor/sparsehash/src/"
	}

	files {
		"premake5.lua",
		"**.h",
		"**.hpp",
		"**.cpp"
	}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
