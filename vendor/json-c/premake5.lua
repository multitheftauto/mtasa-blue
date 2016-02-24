project "json-c"
	language "C++"
	kind "StaticLib"
	targetname "json-c"
	
	includedirs { "shipped_for_mta_win32" }
	defines { "_LIB" }
	
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
	