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
		-- brew install mysql-client libidn2
		includedirs {
			os.findheader("mysql.h", {
				"/usr/local/opt/mysql/include/mysql",
				"/opt/homebrew/include/mysql",
				"/opt/homebrew/opt/mysql-client/include/mysql",
			})
		}
		libdirs {
			os.findlib("libmysqlclient.a", {
				"/usr/local/opt/mysql/lib",
				"/opt/homebrew/lib",
				"/opt/homebrew/opt/mysql-client/lib",
			})
		}

	if GLIBC_COMPAT then
		filter { "system:linux" }
			buildoptions { "-pthread" }
			linkoptions { "-pthread" }
			links { "z", "dl", "m", "mysqlclient", "zstd", "ssl", "crypto", "resolv" }
	else
		filter "system:not windows"
			links { "mysqlclient" }
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
