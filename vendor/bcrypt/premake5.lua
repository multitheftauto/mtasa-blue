project "bcrypt"
	language "C++"
	kind "StaticLib"
	targetname "bcrypt"
	
	vpaths { 
		["Headers/*"] = "*.h",
		["Sources/*"] = "*.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		
		"*.h",
		"*.c",
		"*.S"
	}
