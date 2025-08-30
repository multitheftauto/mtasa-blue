project "Game SA"
	language "C++"
	kind "SharedLib"
	targetname "game_sa"
	targetdir(buildpath("mta"))

	-- DO NOT REMOVE OR TURN THIS OPTION ON
	-- By turning this feature off, our code will be compiled with '/Zi' instead of '/ZI'.
	-- By enabling 'Edit and Continue' the compiler expects us to allocate enough stack space
	-- using __LOCAL_SIZE to ensure enough local space within __declspec(naked) functions for
	-- this feature. Our hook functions will never support this feature, because most of the
	-- time our hook functions don't have proper epilog code and neither prolog code.
	-- This is especially important for our post-build checking of __LOCAL_SIZE.
	-- https://learn.microsoft.com/en-us/cpp/cpp/considerations-for-writing-prolog-epilog-code
	-- https://learn.microsoft.com/en-us/cpp/build/reference/z7-zi-zi-debug-information-format
	editandcontinue "Off"

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths {
		["Headers/*"] = { "**.h", "**.hpp" },
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }

	filter {}

	includedirs {
		"../../Shared/sdk",
		"../sdk/",
		"../../vendor/sparsehash/src/"
	}

	files {
		"premake5.lua",
		"**.h",
		"**.hpp",
		"**.cpp"
	}

	postbuildcommands {
		"%[%{!wks.location}/../utils/hookcheck.exe] %[$(TargetPath)]"
	}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
