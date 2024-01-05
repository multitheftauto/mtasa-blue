project "Effekseer"
	language "C++"
	kind "SharedLib"
	targetname "Effekseer"
	targetdir(buildpath("mta"))

	includedirs {
		"../../Shared/sdk",
		"../sdk",
	}

	links { "Effekseer-vendor", "d3dx9" }

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	includedirs {
		"../../vendor/effekseer/effekseer-submodule/Dev/Cpp/Effekseer",
		"../../vendor/effekseer/effekseer-submodule/Dev/Cpp/EffekseerRendererDX9",
		"../../vendor/effekseer/effekseer-submodule/Dev/Cpp/EffekseerRendererCommon",
		"../../Shared/sdk",
		"../sdk",
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

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
