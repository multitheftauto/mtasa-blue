project "Lua"
	language "C++"
	kind "SharedLib"
	targetname "Lua"
	targetdir(buildpath("server/mods/deathmatch"))

	vpaths { 
		["Headers"] = "**.h",
		["Sources"] = "**.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"src/**.c",
		"src/**.h",
	}
	
	defines {
		"LUA_USE_APICHECK"
	}

	configuration "windows"
		defines { "LUA_BUILD_AS_DLL" }

	filter "system:windows"
		postbuildcommands {
			copy "mta"
		}