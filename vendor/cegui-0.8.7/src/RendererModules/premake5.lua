project "CEGUIDirect3D9Renderer"
	language "C++"
	kind "SharedLib"
	targetname "CEGUIDirect3D9Renderer"
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
        "winmm",
        "d3d9",
    }

	defines {
		"CEGUIDIRECT3D9RENDERER_EXPORTS",
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	vpaths {
		["Headers/*"] = "../../include/CEGUI/RendererModules/Direct3D9/**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.c",
		"../../include/CEGUI/RendererModules/Direct3D9/**.h",
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
