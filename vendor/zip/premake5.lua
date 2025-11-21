project "zip"
	language "C"
	kind "StaticLib"
	targetname "zip"

	includedirs  { "../zlib" }
	defines { "WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP", "IOWIN32_USING_WINRT_API=0" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.c"
	}

	filter "system:not windows"
		excludes { "iowin32.h", "iowin32.c" }
