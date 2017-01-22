project "detours"
	language "C++"
	kind "StaticLib"
	targetname "detours"

	vpaths { 
		["Headers"] = "**.h",
		["Sources"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
        "src/detours.h",
        "src/detours.cpp",
        "src/disasm.cpp",
        "src/modules.cpp",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
		
	filter "system:not windows"
		flags { "ExcludeFromBuild" } 

