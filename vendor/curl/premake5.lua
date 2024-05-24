-- This file is inspired by: https://github.com/premake/premake-core/blob/master/contrib/curl/premake5.lua
project "curl"
	language "C"
	kind "StaticLib"
	targetname "curl"

	includedirs { "include", "lib", "../mbedtls/include", "../zlib" }
	defines { "BUILDING_LIBCURL", "CURL_STATICLIB", "HTTP_ONLY", "USE_ZLIB", "HAVE_LIBZ", "HAVE_ZLIB_H", "HAVE_CONFIG_H", "CURL_DISABLE_LIBCURL_OPTION" }
	warnings "off"

	files {
		"premake5.lua",
		"include/**.h",
		"lib/**.c",
		"lib/**.h"
	}
	removefiles {
		"lib/amigaos.c",
		"lib/amigaos.h",
		"lib/config-amigaos.h",
		"lib/config-dos.h",
		"lib/config-mac.h",
		"lib/config-os400.h",
		"lib/config-plan9.h",
		"lib/config-riscos.h",
		"lib/config-win32ce.h",
		"lib/setup-os400.h",
		"lib/setup-vms.h"
	}

	filter { "system:windows" }
		defines { "USE_SCHANNEL", "USE_WINDOWS_SSPI", "USE_WIN32_IDN" }
		links { "crypt32", "Normaliz" }

	filter { "system:not windows" }
		defines { "USE_MBEDTLS" }

	filter { "system:linux or bsd or macosx" }
		defines { "CURL_HIDDEN_SYMBOLS" }

		-- find the location of the ca bundle
		local ca = nil
		for _, f in ipairs {
			"/etc/ssl/certs/ca-certificates.crt",
			"/etc/pki/tls/certs/ca-bundle.crt",
			"/usr/share/ssl/certs/ca-bundle.crt",
			"/usr/local/share/certs/ca-root.crt",
			"/usr/local/share/certs/ca-root-nss.crt",
			"/etc/ssl/cert.pem" } do
			if os.isfile(f) then
				ca = f
				break
			end
		end
		if ca then
			defines { 'CURL_CA_BUNDLE="' .. ca .. '"' }
		end
