project "blowfish_bcrypt"
	language "C++"
	kind "StaticLib"
	targetname "blowfish_bcrypt"

	vpaths {
		["Headers/*"] = "*.h",
		["Sources/*"] = "*.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.c",
	}

	filter { "platforms:x86 or x64" }
		files { "*.S" }
