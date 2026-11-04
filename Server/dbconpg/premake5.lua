-- PostgreSQL connection library for dbConnect("postgresql", ...).
-- Kept separate from deathmatch (like dbconmy for MySQL) so libpq is only loaded when a script uses it.
-- See vendor/libpq/README.md for where the Windows binaries come from.
project "Dbconpg"
	language "C++"
	kind "SharedLib"
	targetname "dbconpg"
	targetdir(buildpath("server/mods/deathmatch"))
	clangtidy "On"

	filter "system:windows"
		includedirs {
			"../../vendor/libpq/include",
			"../../vendor/sparsehash/src/windows"
		}
		-- Server requires Windows 10+ (cpp-httplib)
		defines { "_WIN32_WINNT=0x0A00" }

	filter {}
		includedirs {
			"../../Shared/sdk",
			"../sdk",
			"../../vendor/google-breakpad/src",
			"../../vendor/sparsehash/src/"
		}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths {
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	-- The root premake5.lua only includes this project on Linux/macOS when libpq's headers are found
	filter "system:not windows"
		includedirs { DBCONPG_LIBPQ_INCLUDE_DIR }
		links { "pq" }

	filter "system:linux"
		links { "rt" }

	filter { "system:linux", "platforms:arm" }
		libdirs { "/usr/lib/arm-linux-gnueabihf" }

	filter { "system:linux", "platforms:arm64" }
		libdirs { "/usr/lib/aarch64-linux-gnu" }

	filter "system:macosx"
		-- brew install libpq
		libdirs {
			os.findlib("pq", {
				"/usr/local/opt/libpq/lib",
				"/opt/homebrew/opt/libpq/lib",
			})
		}

	filter { "system:windows", "platforms:x64" }
		links { "../../vendor/libpq/lib/x64/libpq.lib" }

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))

	filter "platforms:arm"
		targetdir(buildpath("server/arm"))

	filter "platforms:arm64"
		targetdir(buildpath("server/arm64"))

	-- 32-bit Windows server is no longer supported, and there is no Windows arm64 libpq yet
	-- (vendor/libpq/README.md). dbConnect("postgresql", ...) then fails with "could not load dbconpg".
	filter { "system:windows", "platforms:x86 or arm64" }
		flags { "ExcludeFromBuild" }
