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

	filter "system:windows"
		buildoptions { 
			"/Zc:inline",
			"/Zc:throwingNew",
			"/diagnostics:caret",
			"/sdl",
			"/guard:cf"
		}
		editandcontinue "Off"
		linkoptions { "/guard:cf" }

	filter {"system:windows", "configurations:Debug"}
		defines { "_DEBUG" }

	filter {"system:windows", "configurations:Release"}
		optimize "Speed"
		defines { "NDEBUG" }

	filter {"system:windows", "configurations:Nightly"}
		optimize "Speed"
		defines { "NDEBUG" }

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }
	
	filter "system:not windows"
		flags { "ExcludeFromBuild" }
