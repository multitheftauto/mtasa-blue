project "mbedtls"
	kind "StaticLib"
	language "C"

	includedirs {
		"3rdparty",
		"configs",
		"include",
		"library"
	}

	files {
		"library/**.c",
		"include/mbedtls/**.h"
	}

	defines {
		"KRML_VERIFIED_UINT128"
	}

	filter "system:windows"
		links { "ws2_32" }

	filter "architecture:x86_64"
		defines { "MBEDTLS_AESNI_C" }
