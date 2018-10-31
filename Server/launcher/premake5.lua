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
		flags { "StaticRuntime" } 
		files {
			"launcher.rc",
			"resource/mtaicon.ico"
		}

	filter "system:linux"
		links { "dl" }
		buildoptions { "-pthread", "-fvisibility=default" }
		linkoptions { "-pthread", "-rdynamic" }

	filter {"system:linux", "platforms:x86"}
		targetname "mta-server"

	filter {"system:linux", "platforms:x64"}
		targetname "mta-server64"
	
	filter {"system:windows", "platforms:x64"}
		targetname "MTA Server64"
