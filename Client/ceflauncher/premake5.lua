project "CEFLauncher"
	language "C++"
	kind "WindowedApp"
	targetname "ceflauncher"
	targetdir(buildpath("mta/cef"))
	
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

	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 