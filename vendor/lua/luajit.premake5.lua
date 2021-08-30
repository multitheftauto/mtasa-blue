project "LuaJIT"
    kind "SharedLib"
    compileas "C"
    targetname "luajit"
    targetdir(buildpath("mods/deathmatch"))
    
    vectorextensions "SSE2"

    dependson "BuildVM" -- Add dependency to make sure its run before this
    links { "src/lj_*.obj", "src/lib_*.obj" } -- BuildVM generates object files, so lets link those as well..

    vpaths {
        ["Headers/*"] = "src/lj_*.h",
        ["Soruces/*"] = "src/lj_*.c",
        ["*"] = "luajit.premake5.lua",
    }

    files {
		"luajit.premake5.lua",

		"src/lj_*.c",
		--"src/lj_*.h",

		"src/lib_*.c",
		--"src/lib_*.h",
	}

    defines {
		"_CRT_SECURE_NO_DEPRECATE",
		"_CRT_STDIO_INLINE=__declspec(dllexport)__inline"
	}

    filter "kind:ConsoleApp"
        files "src/luajit.c" -- `main` function

    filter "kind:SharedLib"
        defines "LUA_BUILD_AS_DLL"

    --filter "configurations:Debug"
        defines "LUA_USE_APICHECK" -- TODO uncomment above line

    filter {}