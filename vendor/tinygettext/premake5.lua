project "tinygettext"
	language "C++"
	kind "StaticLib"
	targetname "tinygettext"
	
	includedirs { "." }
	
	vpaths { 
		["Headers/*"] = "**.hpp",
		["Sources/*"] = "*.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.hpp",
		"*.cpp"
	}
	