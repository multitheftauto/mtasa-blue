-- The _ACTION variable can be nil, which will be annoying.
-- Let's make an action that won't be nil.
action = _ACTION or ""

targetName = "discordrpc"
targetOs = os.target()

function os.getenv2(env)
    return os.getenv(env) or ""
end

function DeclareCompilationFlags()
    filter {}   -- reset configuration

    cppdialect "C++17"
    symbols "On"
    characterset "Unicode"
    pic "On"
    systemversion "latest"
    flags { "MultiProcessorCompile" }
    
    staticruntime "On"
    
    defines
    {
        "DISCORD_DISABLE_IO_THREAD",
        "DISCORD_DYNAMIC_LIB",
        "DISCORD_WINDOWS"
    }

    filter "configurations:Debug"
        defines { "DISCORD_DEBUG" }
        optimize "Off"
        editandcontinue "On"

    filter "configurations:Release"
        optimize "Full"
        exceptionhandling "Off"
        rtti "Off"
    
    filter{}
end

-- Debug-windows-x86_64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

function GenerateWorkspace()
    workspace "DiscordRPC"
        location("build/" .. action .. "-" .. targetOs)  -- ex: vs2017-android
        configurations { "Debug", "Release" }

        startproject "sample"

        targetdir("build/bin/" .. outputdir .. "/%{prj.name}")
        --objdir ("../build/bin-int/" .. outputdir .. "/%{prj.name}")
        debugdir("build/bin/" .. outputdir .. "/%{prj.name}")

        filter "system:windows"
            platforms { "x86", "x64" }
            
        filter{}
        
        includedirs
        {
            "include"
        }

        -- Include the projects we are going to build
        include "src/rpc"
        include "src/sample"
end

GenerateWorkspace()
