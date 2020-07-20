project "Sample"
    language "C++"
    targetname("sample")
    
    kind "ConsoleApp"
    
    includedirs
    {
        "."
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
    
    links
    {
        "RPC"
    }

    DeclareCompilationFlags()
