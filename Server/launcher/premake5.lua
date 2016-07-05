project "Launcher"
	language "C++"
	kind "ConsoleApp"
	targetname "MTA Server"
	targetdir(buildpath("server"))
	
	includedirs { "../sdk" }
	
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

	filter "system:linux" {
		links { "dl" }
	}
