project "libpng"
    kind "StaticLib"
    language "C"
    architecture "x86"
    systemversion "latest"

    targetdir "bin"
    objdir "obj"

    files {
        "*.c",
        "*.h"
    }

    includedirs {
        "../zlib"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "WIN32_LEAN_AND_MEAN",
        "PNG_NO_MMX_CODE",
        "PNG_SETJMP_NOT_SUPPORTED"
    }
