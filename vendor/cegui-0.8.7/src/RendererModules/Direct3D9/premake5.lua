project "CEGUIDirect3D9Renderer"
	language "C++"
	kind "StaticLib"
	targetname "CEGUIDirect3D9Renderer"

	defines { 
        "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
        "CEGUIDIRECT3D9RENDERER_EXPORTS"
    }
	
	includedirs { 
		"../../../include"
	 }
	 
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.cpp",
		"../../../include/CEGUI/RendererModules/Direct3D9/*.h",
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
	filter "system:not windows"
        flags { "ExcludeFromBuild" }