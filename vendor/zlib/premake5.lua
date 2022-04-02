project "zlib"
	language "C"
	kind "StaticLib"
	targetname "zlib"

	vpaths {
		["Headers/*"] = "*.h",
		["Sources/*"] = "*.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.c"
	}

	excludes {
		"example.c"
	}

	defines { "verbose=-1" }    -- Stop "bit length overflow" warning

	filter "system:Mac"
		defines { "Z_HAVE_UNISTD_H" }