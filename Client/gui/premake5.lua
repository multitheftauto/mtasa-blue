project "GUI"
	language "C++"
	kind "SharedLib"
	targetname "cgui"
	targetdir(buildpath("mta"))

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }

	filter {}
		includedirs {
            "../../Shared/sdk",
			"../sdk",
			"../../vendor/sparsehash/src",
			"../../vendor/freetype/include",
			"../../vendor/imgui",
			"../../vendor/imgui/backends",
		}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	defines {
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	links {
        "ImGui",
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
