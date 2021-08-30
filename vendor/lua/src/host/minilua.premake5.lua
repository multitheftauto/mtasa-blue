project "MiniLua"
    kind "ConsoleApp"
    targetname "minilua" 
    targetsuffix ""
    targetdir ".." -- spit out stuff directly into src/ folder
    compileas "C"

    vpaths {
        ["Soruces/*"] = "minilua.c",
        ["*"] = "minilua.premake5.lua"
    }

    files {
        "minilua.premake5.lua",
        "minilua.c"
    }

    defines {"_CRT_SECURE_NO_WARNINGS"}
    