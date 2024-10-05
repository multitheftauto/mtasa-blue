project "Dbconmy"
	language "C++"
	kind "SharedLib"
	targetname "dbconmy"
	targetdir(buildpath("server/mods/deathmatch"))

	filter "system:windows"
		includedirs {
			"../../vendor/mysql/include",
			"../../vendor/sparsehash/src/windows"
		}

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

	filter "system:linux"
		includedirs { "/usr/include/mysql" }
		links { "rt" }

	filter "system:macosx"
		includedirs {
			os.findheader("mysql.h", {
				"/usr/local/opt/mysql/include/mysql",
				"/opt/homebrew/include/mysql",
				"/opt/osxcross/macports/pkgs/opt/local/include/mysql8/mysql",
			})
		}
		libdirs {
			os.findlib("libmysqlclient.a", {
				"/usr/local/opt/mysql/lib",
				"/opt/homebrew/lib",
				"/opt/osxcross/macports/pkgs/opt/local/lib/mysql8/mysql",
			})
		}

	if GLIBC_COMPAT then
		filter { "system:linux" }
			buildoptions { "-pthread" }
			linkoptions { "-l:libmysqlclient.a", "-pthread", "-lssl", "-lcrypto" }
			links { "z", "dl", "m" }
	else
		filter "system:not windows"
			links { "mysqlclient" }
		filter {"system:linux", "platforms:x86"}
			libdirs { "/usr/lib32/mysql" }
		filter {"system:linux", "platforms:x64"}
			libdirs { "/usr/lib64/mysql" }
	end

	filter { "system:windows", "platforms:x64" }
		links { "../../vendor/mysql/lib/x64/libmysql.lib" }
	filter { "system:windows", "platforms:x86" }
		links { "../../vendor/mysql/lib/x86/libmysql.lib" }
	filter { "system:windows", "platforms:arm64" }
		links { "../../vendor/mysql/lib/arm64/libmysql.lib" }

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))

	filter "platforms:arm"
		targetdir(buildpath("server/arm"))

	filter "platforms:arm64"
		targetdir(buildpath("server/arm64"))
