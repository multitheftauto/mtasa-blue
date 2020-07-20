project "DiscordRPC"
    language "C++"
    targetname("discordrpc")
    characterset "Unicode"
    targetdir(buildpath("mta"))
    
    kind "SharedLib"
    
    defines
    {
        "DISCORD_BUILDING_SDK",
        "DISCORD_DISABLE_IO_THREAD",
        "DISCORD_DYNAMIC_LIB",
        "DISCORD_WINDOWS"
    }
    
    includedirs
    {
        ".",
        "../../thirdparty/rapidjson-last/include",
        "../../include"
    }

    vpaths
    {
        ["Resources/*"] = {"**.rc"},
        ["Headers/**"] = "**.h",
        ["Sources/**"] = "**.cpp",
        ["*"] = "premake5.lua"
    }

    files
    {
        "premake5.lua",
        "**.h",
        "**.cpp",
        "**.rc"
    }
