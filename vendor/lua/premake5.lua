project "Lua_Server"
	language "C++"
	kind "SharedLib"
	targetname "lua5.1"
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

	configuration "windows"
		defines { "LUA_BUILD_AS_DLL" }

if os.get() == "windows" then
	project "Lua_Client"
		language "C++"
		kind "SharedLib"
		targetname "lua5.1"
		targetdir(buildpath("mods/deathmatch"))

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
end
