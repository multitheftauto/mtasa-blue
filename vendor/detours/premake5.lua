project "detours"
    language "C++"
    kind "StaticLib"
    
    defines {
        "WIN32_LEAN_AND_MEAN",
        -- "_WIN32_WINNT=0x501", Not necessary, predefined in root premake5.lua
    }

    files {
        "4.0.1/src/creatwth.cpp",
        "4.0.1/src/detours.h",
        "4.0.1/src/detours.cpp",
        "4.0.1/src/detver.h",
        "4.0.1/src/image.cpp",
        "4.0.1/src/modules.cpp",
        "4.0.1/src/disolx86.cpp",
        "4.0.1/src/disolx64.cpp",
        "4.0.1/src/disasm.cpp",
    }

    vpaths {
        ["Headers/*"] = "4.0.1/src/**.h",
        ["Sources/*"] = "4.0.1/src/**.cpp",
        ["*"] = "premake5.lua"
    }

    filter "system:not windows"
        flags { "ExcludeFromBuild" }

    filter "architecture:x64"
        flags { "ExcludeFromBuild" }
