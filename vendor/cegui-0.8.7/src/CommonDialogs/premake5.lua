project "CEGUICommonDialogs"
	language "C++"
	kind "SharedLib"
	targetname "CEGUICommonDialogs"
	targetdir(buildpath("mta"))

	includedirs {
		"../../include",
		"../../../freetype/include",
		"../../../pcre"
	}
    
    links {
        "CEGUI-0.8.7",
		"freetype", 
		"pcre",
		"winmm"
    }

	defines {
		"CEGUICOMMONDIALOGS_EXPORTS",
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	vpaths {
		["Headers/*"] = "../../include/CEGUI/CommonDialogs/**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.c",
		"../../include/CEGUI/CommonDialogs/**.h",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter {"system:windows"}
		linkoptions { "/ignore:4221" }
		disablewarnings { "4221" }

    configuration "Debug"
		links { "dbghelp" }
	 configuration {}
