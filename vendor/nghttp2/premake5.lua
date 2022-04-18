-- This file is inspired by: https://github.com/premake/premake-core/blob/master/contrib/curl/premake5.lua
project "nghttp2"
	language "C"
	kind "SharedLib"
	targetname "nghttp2"
	targetdir(buildpath("server"))

	includedirs { ".", "lib/includes" }
	defines { "BUILDING_NGHTTP2", "HAVE_CONFIG_H" }
	warnings "off"

	files {
		"premake5.lua",
		"config.h",
		"config-linux.h",
		"config-macos.h",
		"config-win32.h",
		"**.h",
		"**.c"
	}

	filter {"system:windows", "platforms:x86"}
		postbuildcommands {
			copy "mta"
		}

	filter {"system:windows", "platforms:x86", "configurations:Debug"}
		postbuildcommands {
			-- Fix net(c).dll requiring the release build
			"copy \"%{wks.location}..\\Bin\\server\\nghttp2_d.dll\" \"%{wks.location}..\\Bin\\mta\\nghttp2.dll\"",
			"copy \"%{wks.location}..\\Bin\\server\\nghttp2_d.dll\" \"%{wks.location}..\\Bin\\server\\nghttp2.dll\""
		}

	filter {"system:windows", "platforms:x64", "configurations:Debug"}
		postbuildcommands {
			-- Fix net.dll requiring the release build
			"copy \"%{wks.location}..\\Bin\\server\\x64\\nghttp2_d.dll\" \"%{wks.location}..\\Bin\\server\\x64\\nghttp2.dll\""
		}

	filter {"system:windows", "platforms:x64"}
		targetdir(buildpath("server/x64"))
