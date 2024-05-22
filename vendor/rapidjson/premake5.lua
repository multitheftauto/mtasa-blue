project "rapidjson"
    targetname "rapidjson"
    language "C++"
    kind "None"

    defines {
        "RAPIDJSON_SSE2",
        "RAPIDJSON_HAS_STDSTRING",
    }
    
    includedirs {
        "include/rapidjson",
        "include/rapidjson/error",
        "include/rapidjson/internal",
        "include/rapidjson/msinttypes",
    }

    files {
        "include/rapidjson/**.h",
        "include/rapidjson/error/**.h",
        "include/rapidjson/internal/**.h",
        "include/rapidjson/msinttypes/**.h",
    }
    
    filter "system:linux"
	    defines { "RAPIDJSON_SSE42" }

    filter "platforms:x64"
	    defines { "RAPIDJSON_SSE42" }

    filter "platforms:arm"
        defines { "RAPIDJSON_NEON" }

    filter "platforms:arm64"
        defines { "RAPIDJSON_NEON" }
    