project "CEFLauncher"
	language "C++"
	kind "WindowedApp"
	targetname "ceflauncher"
	
	includedirs { "../sdk" }
	
	links { "CEFLauncher DLL"}

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
	