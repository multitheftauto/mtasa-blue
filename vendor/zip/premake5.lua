project "zip"
	language "C++"
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
	