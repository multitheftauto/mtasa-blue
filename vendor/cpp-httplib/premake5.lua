-- cpp-httplib: A C++11 single-header HTTP/HTTPS library
-- Header-only, no source files needed

project "httplib"
	language "C++"
	kind "StaticLib"
	targetname "httplib"
	warnings "Off"

	-- httplib can optionally use OpenSSL for HTTPS, enabled via the
	-- CPPHTTPLIB_OPENSSL_SUPPORT define. We don't enable it: the built-in web
	-- server serves plain HTTP only (same as the previous EHS-based server).
	defines {
		"CPPHTTPLIB_USE_POLL",
	}

	includedirs {
		".",
	}

	vpaths {
		["Headers/*"] = "*.h",
		["Sources/*"] = "*.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"stub.cpp",
	}

	filter "system:windows"
		links { "ws2_32" }

	filter "system:linux"
		links { "pthread" }
