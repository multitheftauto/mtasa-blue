project "Lua_Server"
	language "C++"
	targetname "lua5.1"
	targetdir(buildpath("server/mods/deathmatch"))
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
	local targetpath = "%{wks.location}/../Shared/publicsdk/include/"

	postbuildcommands {
		"{COPY} "..luapath.."lua.h "..targetpath,
		"{COPY} "..luapath.."luaconf.h "..targetpath,
		"{COPY} "..luapath.."lauxlib.h "..targetpath,
		"{COPY} "..luapath.."lualib.h "..targetpath
	}

	filter "system:windows"
		defines { "LUA_BUILD_AS_DLL" }

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))

	filter {"system:linux", "platforms:x86"}
		postbuildcommands { "{COPY} "..buildpath("server/mods/deathmatch").."lua5.1.so %{wks.location}/../Shared/publicsdk/lib/" }
	
	filter {"system:linux", "platforms:x64"}
		postbuildcommands { "{COPY} "..buildpath("server/x64").."lua5.1.so %{wks.location}/../Shared/publicsdk/lib/" }


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

-- Build static version for Linux module backwards compatibility
project "Lua_Server_Static"
	language "C++"
	kind "StaticLib"

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
