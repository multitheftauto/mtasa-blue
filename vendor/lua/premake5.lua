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

	defines { "LUA_BUILD_AS_DLL" }

	filter "system:windows"
		kind "SharedLib"
		targetdir(buildpath("server/mods/deathmatch"))

	filter "system:not windows"
		kind "StaticLib"

	filter {"system:windows", "platforms:x64"}
		targetdir(buildpath("server/x64"))

	filter {"system:windows", "platforms:arm"}
		targetdir(buildpath("server/arm"))

	filter {"system:windows", "platforms:arm64"}
		targetdir(buildpath("server/arm64"))


if os.target() == "windows" then
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
			"LUA_USE_APICHECK",
			"LUA_BUILD_AS_DLL"
		}

        filter "platforms:not x86"
            flags { "ExcludeFromBuild" } 
end
