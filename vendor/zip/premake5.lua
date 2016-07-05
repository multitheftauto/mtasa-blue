project "zip"
	language "C"
	kind "StaticLib"
	targetname "zip"
	
	includedirs  { "../zlib" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.c"
	}
	
	filter "system:linux"
		excludes { "iowin32.h", "iowin32.c" }
	