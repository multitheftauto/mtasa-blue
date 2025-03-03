project "mbedtls"
	language "C"
	kind "StaticLib"

	includedirs {
		"configs",
		"include",
		"library",
		"../3rdparty/**",
	}

	files {
		"premake5.lua",
		"**.h",
		"**.c"
	}

	defines {
		"KRML_VERIFIED_UINT128",
	}

	filter "system:windows"
		links { "ws2_32" }

	filter "architecture:x86"
		defines {
			"MBEDTLS_AESNI_C"
		}
