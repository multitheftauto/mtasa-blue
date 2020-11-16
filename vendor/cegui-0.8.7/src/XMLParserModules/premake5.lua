project "CEGUITinyXMLParser"
	language "C++"
	kind "SharedLib"
	targetname "CEGUITinyXMLParser"
	targetdir(buildpath("mta"))

	includedirs {
		"../../include",
		"../../../freetype/include",
        "../../../pcre",
        "../../dependencies/tinyxml-2.6.2"
	}
    
    links {
        "CEGUI-0.8.7",
        "tinyxml-2.6.2",
		"freetype", 
		"pcre",
        "winmm",
        "d3d9",
    }

	defines {
		"CEGUITINYXMLPARSER_EXPORTS",
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	vpaths {
		["Headers/*"] = "../../include/CEGUI/XMLParserModules/TinyXML/**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.c",
		"../../include/CEGUI/XMLParserModules/TinyXML/**.h",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter {"system:windows"}
		linkoptions { "/ignore:4221" }
		disablewarnings { "4221" }

    configuration "Debug"
        links { "dbghelp", "d3dx9d" }
    configuration "Release"
        links { "d3dx9" }
	 configuration {}
