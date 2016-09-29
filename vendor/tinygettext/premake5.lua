project "tinygettext"
	language "C++"
	kind "StaticLib"
	targetname "tinygettext"
	
	includedirs { "." }
	disablewarnings { "4800", "4309", "4503", "4099", "4503" }
	
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
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 