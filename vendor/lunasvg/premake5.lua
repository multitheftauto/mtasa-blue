project "lunasvg"
  language "C++"
  cppdialect "C++17"
  kind "SharedLib"
  targetname "lunasvg"
  targetdir(buildpath("mta"))
  floatingpoint "Fast"
  rtti "Off"

  defines { "LUNASVG_EXPORTS", "LUNASVG_SHARED", "_CRT_SECURE_NO_WARNINGS" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

  files { 
    "premake5.lua",
    "**.cpp",
    "**.h",
    "**.c"
  }

  includedirs { 
    "3rdparty/stb", 
    "3rdparty/plutovg", 
    "3rdparty/software", 
    "source/geometry", 
    "source/graphics", 
    "source", 
    "include", 
    "contrib"
  }

configuration "vs2019"
  flags { "MultiProcessorCompile" }

configuration "vs2019 and Release"
  flags { "LinkTimeOptimization" }