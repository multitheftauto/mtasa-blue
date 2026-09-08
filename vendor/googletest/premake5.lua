-- Google Test v1.17.0
-- Downloaded from https://github.com/google/googletest/releases/tag/v1.17.0

project "gtest"
	language "C++"
	kind "StaticLib"
	targetname "gtest"
	warnings "Off"

	includedirs {
		"include",
		"."  -- Required for gtest-all.cc to find src/ internal headers
	}

	files {
		"premake5.lua",
		"src/gtest-all.cc"
	}

	defines {
		"GTEST_HAS_PTHREAD=0"
	}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
