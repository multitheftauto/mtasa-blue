project "json-c"
	language "C++"
	kind "StaticLib"
	targetname "json-c"
	
	includedirs { "." }
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
	
	filter "system:windows"
		includedirs { "shipped_for_mta_win32" }
		disablewarnings { "4244" }
	
	filter "system:not windows"
		includedirs { "shipped_for_mta_linux" }
