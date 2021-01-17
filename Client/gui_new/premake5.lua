project "GUI New"
	language "C++"
	kind "SharedLib"
	targetname "cgui_new"
	targetdir(buildpath("mta"))

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }

	filter {}
		includedirs {
			"../sdk",
			"../../vendor/cegui-0.8.7/include",
			"../../vendor/sparsehash/src/",
			"../../vendor/freetype/include",
		}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	defines {
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	links {
		"CEGUI-0.8.7",
		"CEGUICoreWindowRendererSet",
		"CEGUISILLYImageCodec",
		"CEGUIDirect3D9Renderer",
		"CEGUITinyXMLParser",
		"d3dx9.lib",
		"d3d9.lib",
		"dxerr.lib"
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
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
        flags { "ExcludeFromBuild" }
        
    configuration "windows"
        buildoptions { "-Zm180" }

	configuration "Debug"
		links { "dbghelp" }
