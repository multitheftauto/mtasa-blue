project "Game SA"
	language "C++"
	kind "SharedLib"
	targetname "game_sa"
	targetdir(buildpath("mta"))

	-- HACK(Jusonex): Temp fix for ebp not being set in naked functions
	-- More information on this in multiplayer_sa's premake5.lua
	cppdialect "C++14"

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

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
