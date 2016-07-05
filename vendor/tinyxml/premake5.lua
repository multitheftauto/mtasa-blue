project "tinyxml"
	language "C++"
	kind "StaticLib"
	targetname "tinyxml"
	
	defines { "TIXML_USE_STL" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	