project "simdjson"
    targetname "simdjson"
    language "C++"
    kind "StaticLib"

    includedirs {
        "singleheader",
    }

    files {
        "singleheader/simdjson.h",
        "singleheader/simdjson.cpp",
    }

    filter "platforms:arm64"
        defines {"SIMDJSON_IMPLEMENTATION_ARM64=1" }