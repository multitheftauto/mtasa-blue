project "Launcher"
	language "C++"
	kind "ConsoleApp"
	targetdir(buildpath("server"))
	
	includedirs { "../sdk" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"*.rc", "**.ico"},
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	
	filter "system:windows"
		targetname "MTA Server"
		files {
			"launcher.rc",
			"resource/mtaicon.ico"
		}

	filter "system:linux"
		links { "dl" }

	filter {"system:linux", "platforms:x86"}
		targetname "mta-server"

	filter {"system:linux", "platforms:x64"}
		targetname "mta-server64"
