project "CEGUI-0.8.7"
	language "C++"
	kind "SharedLib"
	targetname "CEGUI-0.8.7"
	targetdir(buildpath("mta"))

	includedirs {
		"include",
		"../freetype/include",
		"dependencies/pcre-8.12"
	}

	links { 
		"freetype", 
		"pcre-8.12",
		"dbghelp",
		"winmm"
	}

	defines {
		"CEGUIBASE_EXPORTS",
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
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

	excludes {
		"src/ImageCodecModules/**",
		"include/CEGUI/ImageCodecModules/**",
		"src/CommonDialogs/**",
		"include/CEGUI/CommonDialogs/**",
		"src/ScriptModules/**",
		"include/CEGUI/ScriptModules/**",
		"src/RendererModules/**",
		"include/CEGUI/RendererModules/**",
		"src/WindowRendererSets/**",
		"include/CEGUI/WindowRendererSets/**",
		"src/XMLParserModules/**",
		"include/CEGUI/XMLParserModules/**",	
		"src/implementations/**",
		"include/CEGUI/implementations/**",
		"src/MinizipResourceProvider.cpp",
		"include/CEGUI/MinizipResourceProvider.h",
		"src/IconvStringTranscoder.cpp",
		"include/CEGUI/IconvStringTranscoder.h"
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter {"system:windows"}
		linkoptions { "/ignore:4221" }
		disablewarnings { "4221" }
