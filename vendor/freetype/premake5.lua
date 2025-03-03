-- This file is inspired by: https://github.com/premake/premake-core/blob/master/contrib/curl/premake5.lua
project "freetype"
	language "C"
	kind "StaticLib"
	targetname "freetype"

	includedirs { "include", "src",  }
	defines { "FT2_BUILD_LIBRARY=1", "_UNICODE", "UNICODE", "_LIB" }
	removedefines { "DEBUG" }

	files {
		"premake5.lua",
		"include/**.h",
		"src/**.c"
		"builds/**.c"
	}
