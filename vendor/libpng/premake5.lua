project "libpng"
	language "C++"
	kind "StaticLib"
	targetname "libpng"
	
	includedirs { "../zlib" }
	defines { "NDEBUG", "PNG_SETJMP_NOT_SUPPORTED", "WIN32_LEAN_AND_MEAN" }
	
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
	