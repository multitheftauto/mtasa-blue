local function copyserverlib( libname )
	filter { "configurations:Debug", "platforms:x86" }
		postbuildcommands {
			"{COPYFILE} " .. buildpath("server/" .. libname .. "_d.dll") .. " " .. buildpath("mods/deathmatch/" .. libname .. "_d.dll")
		}

	filter { "configurations:not Debug", "platforms:x86" }
		postbuildcommands {
			"{COPYFILE} " .. buildpath("server/" .. libname.. ".dll") .. " " .. buildpath("mods/deathmatch/" .. libname .. ".dll")
		}
end


--[[
	Lua 5.1
]]
project "Lua5_1"
	language "C++"
	kind "SharedLib"
	targetname "lua5.1"
	targetdir(buildpath("server"))

	vpaths { 
		["Headers"] = "**.h",
		["Sources"] = "**.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"lua51/**.c",
		"lua51/**.h",
	}

	defines {
		"LUA_BUILD_AS_DLL"
	}

if os.target() == "windows" then
	defines "LUA_USE_APICHECK"

	-- Copy a server library to client
	copyserverlib( "lua5.1" )
end

	filter {"platforms:x64"}
		targetdir(buildpath("server/x64"))

	filter {"platforms:arm"}
		targetdir(buildpath("server/arm"))

	filter {"platforms:arm64"}
		targetdir(buildpath("server/arm64"))

--[[
	Luau
]]
project "Luau"
	language "C++"
	kind "SharedLib"
	targetname "luau"
	targetdir(buildpath("server"))

	vpaths { 
		["Headers"] = "**.h",
		["Sources"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"luau/**.cpp",
		"luau/**.h"
	}

	includedirs {
		"luau/Ast/include",
		"luau/Common/include",
		"luau/Compiler/include",
		"luau/VM/include",
	}

	if os.target() == "windows" then
		defines {
			"LUA_API=extern \"C\" __declspec(dllexport)",
			"LUACODE_API=extern \"C\" __declspec(dllexport)"
		}

		-- Luau libname is same for server and client in windows
		copyserverlib( "luau" )	
	else
		defines {
			"LUA_API=extern \"C\" __attribute__((visibility(\"default\")))",
			"LUACODE_API=extern \"C\" __attribute__((visibility(\"default\")))"
		}
	end	

	filter {"platforms:x64"}
		targetdir(buildpath("server/x64"))

	filter {"platforms:arm"}
		targetdir(buildpath("server/arm"))

	filter {"platforms:arm64"}
		targetdir(buildpath("server/arm64"))

--[[
	Lua_Server
]]
project "Lua_Server"
	language "C++"
	kind "StaticLib"
	targetname "vlua"

	vpaths { 
		["Headers"] = "**.h",
		["Sources"] = "**.c",
		["Sources"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"src/**.c",
		"src/**.cpp",
		"src/**.h"
	}

	defines "VLUA_EXPORT"

	filter {"platforms:x64"}
		defines "VLUA_PLATFORM=\"x64/\""

	filter {"platforms:arm"}
		defines "VLUA_PLATFORM=\"arm/\""

	filter {"platforms:arm64"}
		defines "VLUA_PLATFORM=\"arm64/\""

	filter {"platforms:x86"}
		defines "VLUA_PLATFORM="

	filter {"system:windows", "platforms:arm"}
		targetdir(buildpath("server/arm"))

	filter {"system:windows", "platforms:arm64"}
		targetdir(buildpath("server/arm64"))


--[[
	Lua_Client
]]
if os.target() == "windows" then
	project "Lua_Client"
		language "C++"
		kind "StaticLib"
		targetname "vluac"

		vpaths { 
			["Headers"] = "**.h",
			["Sources"] = "**.c",
			["Sources"] = "**.cpp",
			["*"] = "premake5.lua"
		}
	
		files {
			"premake5.lua",
			"src/**.c",
			"src/**.cpp",
			"src/**.h"		
		}	
	
		defines {
			"LUA_USE_APICHECK",
			"VLUA_EXPORT",
			"VLUA_PLATFORM="
		}

        filter "platforms:not x86"
            flags { "ExcludeFromBuild" }
end
