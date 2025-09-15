project "CEFLauncher"
	language "C++"
	kind "WindowedApp"
	targetname "CEFLauncher"
	targetdir(buildpath("mta/cef"))
	includedirs { "../sdk" }
	links { "CEFLauncher DLL"}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"*.rc", "**.ico", "**.xml", "**.manifest"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp",
		"*.manifest",
	}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }
	
	filter "system:not windows"
		flags { "ExcludeFromBuild" }
