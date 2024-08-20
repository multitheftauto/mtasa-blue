project "DiscordBot"
    language "C++"
    kind "SharedLib"
    targetname "discord-bot"
	targetdir(buildpath("server"))
 
    includedirs {
        "discord/include",
        "discord/include/dpp",
        "discord/win32/include"
    }
 
    characterset "MBCS"
    functionlevellinking "off"
    editandcontinue 'off'
 
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
        runtime 'debug'
        defines {
            '_DEBUG',
        }
 
    filter "configurations:Release"
        runtime 'release'
        defines {
            'NDEBUG',
        }
 
    filter "system:windows"
        defines {
            "DPP_OS=Windows",
            "OPENSSL_SYS_WIN32",
            "_WINSOCK_DEPRECATED_NO_WARNINGS",
            "WIN32_LEAN_AND_MEAN",
        }
        libdirs {
            'discord/win32/lib/'
        }
        links {
            'libssl.lib',
            'libcrypto.lib',
            'zlib.lib',
            'libsodium.lib',
            'opus.lib',
            'kernel32.lib',
            'user32.lib',
            'gdi32.lib',
            'winspool.lib',
            'shell32.lib',
            'ole32.lib',
            'oleaut32.lib',
            'uuid.lib',
            'comdlg32.lib',
            'advapi32.lib',
        }
        buildoptions { '/bigobj' }