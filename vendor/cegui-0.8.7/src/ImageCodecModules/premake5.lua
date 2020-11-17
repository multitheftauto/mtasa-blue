project "CEGUISILLYImageCodec"
	language "C++"
	kind "SharedLib"
	targetname "CEGUISILLYImageCodec"
	targetdir(buildpath("mta"))

	includedirs {
		"../../include",
		"../../dependencies/SILLY/include",
		"../../../freetype/include",
		"../../dependencies/pcre-8.12"
	}
    
    links {
        "CEGUISILLYModule",
        "CEGUI-0.8.7",
		"freetype", 
		"pcre-8.12",
		"dbghelp",
		"winmm"
    }

	defines {
		"CEGUISILLYIMAGECODEC_EXPORTS",
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	vpaths {
		["Headers/*"] = "../../include/CEGUI/ImageCodecModules/**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.c",
		"../../include/CEGUI/ImageCodecModules/**.h",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter {"system:windows"}
		linkoptions { "/ignore:4221" }
		disablewarnings { "4221" }
