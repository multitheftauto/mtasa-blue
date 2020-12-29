project "breakpad"
	language "C++"
	kind "StaticLib"
	targetname "breakpad"

	includedirs { "src", "src/third_party/glog/src" }
	vpaths {
		["Headers/*"] = "src/**.h",
		["Sources/*"] = {"src/**.cc", "src/**.c"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",

		"src/**.h",
		"src/client/*.cc",
		"src/common/*.cc",
		"src/common/*.c",
		"src/processor/*.cc",

		"src/third_party/glog/src/*.cc",
		"src/third_party/libdisasm/*.c",
		"src/third_party/protobuf/protobuf/src/**.c"
	}

	filter "system:linux"
		files {
			"src/client/linux/**.cc",
			"src/common/linux/**.cc",
			"src/common/dwarf/**.cc"
		}

	filter "system:macosx"
		files {
			"src/client/mac/**.cc",
			"src/client/mac/**.mm",
			"src/common/mac/**.mm",
			"src/common/mac/**.cc"
		}

	-- Is there a better way to allow vendor to `include <BreakpadDefines.h>`
	-- It should include "src/client/apple/Framework/BreakpadDefines.h".
	filter "system:macosx"
		includedirs { "src/client/apple/Framework", "src/common/mac" }

	filter {}
		-- Ignore tests
		excludes(os.matchfiles("**/*_unittest.cc"))
		excludes(os.matchfiles("**/*_test.cc"))
		excludes(os.matchfiles("**/*_selftest.cc"))

		excludes {
			"src/client/linux/sender/google_crash_report_sender.cc",
			"src/common/linux/tests/**.cc",
			"src/client/mac/tests/**",
			"src/client/mac/handler/testcases/**",
			"src/common/stabs_reader.cc"
		}

	--[[filter "system:windows"
		files {
			"src/client/windows/**.cc",
			"src/common/windows/**.cc"
		}]]

	filter "system:not linux"
		flags { "ExcludeFromBuild" }
