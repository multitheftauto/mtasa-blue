-- cpp-httplib: A C++11 single-header HTTP/HTTPS library
-- Header-only, no source files needed

project "httplib"
	language "C++"
	kind "StaticLib"
	targetname "httplib"
	warnings "Off"

	-- httplib can optionally use OpenSSL; we don't enable it here
	-- as SSL is handled separately via EHS's existing OpenSSL setup
	defines {
		"CPPHTTPLIB_USE_POLL",
	}

	includedirs {
		".",
	}

	vpaths {
		["Headers/*"] = "*.h",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
	}

	filter "system:windows"
		links { "ws2_32", "crypt32" }

	filter "system:linux"
		links { "pthread" }
