project "Loader Proxy"
	language "C++"
	kind "SharedLib"
	targetname "mtasa"
	targetdir(buildpath("mta"))
	targetsuffix ""

	vpaths {
		["Headers/*"] = "**.h",
		["Sources"] = { "*.cpp", "*.asm" },
		["Resources/*"] = "*.def",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"exports.def",
		"mtasa.asm",
		"setup.cpp",
		"main.cpp",
	}

	filter "system:windows"
		linkoptions { "/SAFESEH:NO" }

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
