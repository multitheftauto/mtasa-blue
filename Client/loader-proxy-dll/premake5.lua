project "Loader Proxy DLL"
	language "C++"
	kind "SharedLib"
	targetname "winmm"
	targetdir(buildpath("mta"))
    targetsuffix ""

	filter "system:windows"
		linkoptions { "/SAFESEH:NO" }

	includedirs {
		"../sdk",
		"../../vendor"
	}

	links {
        "Shlwapi"
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["Resources/*"] = "*.def",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp",
		"*.def"
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
