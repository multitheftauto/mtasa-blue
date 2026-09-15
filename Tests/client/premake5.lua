project "Tests_Client"
	language "C++"
	kind "ConsoleApp"
	targetname "Tests_Client"
	targetdir(buildpath("tests"))

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	includedirs {
		"../../Shared/sdk",
		"../../Client/sdk",
		"../../vendor/googletest/include",
		-- Needed so SharedUtil.h -> SharedUtil.Crypto.h can find <cryptopp/*.h> and <zlib/zlib.h>
		"../../vendor",
	}

	links { "gtest", "cryptopp", "blowfish_bcrypt", "zlib" }

	files {
		"premake5.lua",
		"**.h",
		"**.cpp"
	}

	defines {
		"GTEST_HAS_PTHREAD=0",
		"MTA_CLIENT",
		"SHARED_UTIL_WITH_HASH_MAP",
	}

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }
		links { "ws2_32", "shlwapi", "shell32" }

	filter {}
		includedirs { "../../vendor/sparsehash/src/" }

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
