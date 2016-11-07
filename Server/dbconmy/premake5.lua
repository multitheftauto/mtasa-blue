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
	
	filter "system:not windows"
		includedirs { "/usr/include/mysql" }
		links { "mysqlclient", "rt" }

	filter {"system:linux", "platforms:x86"}
		libdirs { "/usr/lib32/mysql" }
	filter {"system:linux", "platforms:x64"}
		libdirs { "/usr/lib64/mysql" }
	
	filter { "system:windows", "platforms:x64" }
		links { "../../vendor/mysql/lib/x64/libmysql.lib" }
	filter { "system:windows", "platforms:x86" }
		links { "../../vendor/mysql/lib/x86/libmysql.lib" }
	
	filter "platforms:x64"
		targetdir(buildpath("server/x64"))
