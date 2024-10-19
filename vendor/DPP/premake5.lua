project "DiscordBot"
    language "C++"
    kind "SharedLib"
    targetname "discord-bot"
 
    includedirs {
        "discord/include",
        "discord/include/dpp",
        "discord/win32/include"
    }
 
    vpaths {
        ["Headers/*"] = "discord/include/dpp/**.h",
        ["Sources/*"] = "discord/src/dpp/**.cpp",
        ["*"] = "premake5.lua"
    }
 
    files {
        "premake5.lua",
        "discord/include/dpp/**.h",
        "discord/src/dpp/**.cpp",
    }
 
    filter {}
        defines {
            "FD_SETSIZE=1024",
            "DPP_BUILD",
            "AVX_TYPE=2",
            "HAVE_VOICE",
            "dpp_EXPORTS",
        }
 
    filter "configurations:Debug"
        links {
            'opus_d.lib',
        }
 
    filter "configurations:Release"
        links {
            'opus.lib',
        }

    filter "platforms:x86"
        links {
            'libssl-3.lib',
            'libcrypto-3.lib',
        }
        targetdir(buildpath("server/mods/deathmatch"))
    filter "platforms:x64"
        links {
            'libssl-3-x64.lib',
            'libcrypto-3-x64.lib',
        }
        targetdir(buildpath("server/x64"))
 
    filter "system:windows"
        defines {
            "DPP_OS=Windows",
            "OPENSSL_SYS_WIN32",
            "_WINSOCK_DEPRECATED_NO_WARNINGS",
            "WIN32_LEAN_AND_MEAN",
        }
        links {
            'libsodium.lib',
            'zlib',
        }
        buildoptions { '/bigobj' }
    
    filter { "configurations:Debug", "platforms:x86" }
        libdirs {
            'discord/libs/x86/Debug'
        }
		targetsuffix "-x86_d"
    filter { "configurations:Release", "platforms:x86" }
        libdirs {
            'discord/libs/x86/Release'
        }
		targetsuffix "-x86"
    
    filter { "configurations:Debug", "platforms:x64" }
        libdirs {
            'discord/libs/x64/Debug'
        }
		targetsuffix "-x64_d"
    filter { "configurations:Release", "platforms:x64" }
        libdirs {
            'discord/libs/x64/Release'
        }
		targetsuffix "-x64"