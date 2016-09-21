project "pugixml"
	language "C++"
	kind "StaticLib"
	targetname "pugixml"
		
	includedirs { 
		"include"
	}

	vpaths { 
		["Headers"] = "**.hpp",
		["Sources"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"src/pugixml.cpp",
		"src/pugixml.hpp",
		"src/pugiconfig.hpp"
	}
	
	