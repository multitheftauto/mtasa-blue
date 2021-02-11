-- This file is inspired by: https://github.com/premake/premake-core/blob/master/contrib/curl/premake5.lua
project "nghttp2"
	language "C"
	kind "StaticLib"
	targetname "nghttp2"

	includedirs { ".", "lib/includes" }
	defines { "ENABLE_LIB_ONLY", "ENABLE_STATIC_LIB", "HAVE_CONFIG_H" }
	warnings "off"

	files {
		"premake5.lua",
		"config.h",
		"config-linux.h",
		"config-macos.h",
		"config-win32.h",
		"lib/**.h",
		"lib/**.c"
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}
