project "portaudio"
	language "C++"
	kind "StaticLib"
	targetname "portaudio"
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.c"
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 