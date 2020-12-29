project "DirectX9GUIRenderer"
	language "C++"
	kind "StaticLib"
	targetname "DirectX9GUIRenderer"

	defines { "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING" }
	
	includedirs { 
		"../sdk",
		"../../../include"
	 }
	 
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"d3d9renderer.cpp",
		"d3d9texture.cpp",
		"../../../include/renderers/d3d9texture.h",
		"../../../include/renderers/d3d9renderer.h"
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
	filter "system:not windows"
        flags { "ExcludeFromBuild" }