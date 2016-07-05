project "zlib"
	language "C++"
	kind "StaticLib"
	targetname "zlib"
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "*.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.c"
	}
	