project "Launcher"
	language "C++"
	kind "ConsoleApp"
	targetname "MTA Server"
	
	includedirs { "../sdk" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	