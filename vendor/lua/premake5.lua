project "Lua"
	language "C++"
	kind "SharedLib"
	targetname "Lua"

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
	
	-- Todo: Client only
	defines {
		"LUA_USE_APICHECK"
	}

	configuration "windows"
		defines { "LUA_BUILD_AS_DLL" }

