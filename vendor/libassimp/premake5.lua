project "assimp"
	language "C++"
	kind "StaticLib"
	targetname "assimp"

	includedirs { "include" }


	vpaths {
		["Headers/assimp/*"] = "include/assimp/*.h",
		["Headers/Obj/*"] = "code/Obj/*.h",
		["Sources/Obj/*"] = "code/Obj/*.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",

		"*.h",
		"*.cpp",
		"include/assimp/*.h",
		"code/Obj/*.h",
		"code/Obj/*.cpp",
	}
