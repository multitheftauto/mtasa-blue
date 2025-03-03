project "unrar"
	language "C++"
	kind "StaticLib"
	targetname "unrar"

	defines { "RARDLL" }

	vpaths {
		["Headers/*"] = "**.hpp",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.hpp",
		"*.cpp"
	}

	filter "system:Windows*"
		files { "isnt.cpp" }