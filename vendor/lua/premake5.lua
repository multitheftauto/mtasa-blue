project "Lua_Server"
	language "C++"
	targetname "lua5.1"

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

	filter "system:windows"
		defines { "LUA_BUILD_AS_DLL" }
		kind "SharedLib"
		targetdir(buildpath("server/mods/deathmatch"))

	filter "system:not windows"
		kind "StaticLib"

	filter {"system:windows", "platforms:x64"}
		targetdir(buildpath("server/x64"))


if os.get() == "windows" then
	project "Lua_Client"
		language "C++"
		kind "SharedLib"
		targetname "lua5.1c"
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

        filter "platforms:x64"
            flags { "ExcludeFromBuild" } 
end

-- Build shared version for the publicsdk
project "Lua_Server_publicsdk"
	language "C++"
	targetdir("%{wks.location}../Shared/publicsdk/lib")
	targetname "lua5.1"
	kind "SharedLib"

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

	local luapath = _SCRIPT_DIR.."/src/"
	local targetpath = "%{wks.location}../Shared/publicsdk/include/"

	postbuildcommands {
		"{COPY} "..luapath.."lua.h "..targetpath,
		"{COPY} "..luapath.."luaconf.h "..targetpath,
		"{COPY} "..luapath.."lauxlib.h "..targetpath,
		"{COPY} "..luapath.."lualib.h "..targetpath
	}

	filter "system:windows"
		defines { "LUA_BUILD_AS_DLL" }
