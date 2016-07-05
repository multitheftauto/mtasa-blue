project "CEFLauncher"
	language "C++"
	kind "WindowedApp"
	targetname "ceflauncher"
	targetdir(buildpath("Client"))
	
	includedirs { "../sdk" }
	
	links { "CEFLauncher DLL"}
	flags { "WinMain" }

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
