project "CEGUISILLYModule"
	language "C++"
	kind "SharedLib"
	targetname "CEGUISILLYModule"
	targetdir(buildpath("mta"))

	includedirs {
		"include",
		"../../../libpng",
		"../../../jpeg-9d",
		"../../../zlib"
	}
    
    links {
        "libpng",
        "jpeg",
        "zlib"
    }

    defines {
		"SILLY_HAVE_JPG",
		"SILLY_HAVE_PNG",
        "SILLY_EXPORTS"
    }

	vpaths {
		["Headers/*"] = "include/**.h",
		["Sources/*"] = "src/**.cpp",
		["Sources/*"] = "src/**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"src/**.cpp",
		"src/**.c",
		"include/**.h",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter {"system:windows"}
		linkoptions { "/ignore:4221" }
		disablewarnings { "4221" }