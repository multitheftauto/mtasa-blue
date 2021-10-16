project "LuaModules"
	language "C++"
	targetname "LuaModules"
	kind "StaticLib"
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	includedirs {
		".",
		"include",
		"../../../Shared/",
		"../../../vendor/lua/src",
	}

	vpaths { 
		["Headers"] = "**.h",
		["Interfaces"] = "include/interfaces/**.h",
		["Sources"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.h",
		"**.cpp"
	}

	links {
		"Lua_Server"
	}
