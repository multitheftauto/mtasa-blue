project "Dbconmy"
	language "C++"
	kind "SharedLib"
	targetname "dbconmy"
	
	includedirs { 
		"../sdk", 
		"../../vendor/google-breakpad/src",
		"../../vendor/sparsehash/current/src/",
		"../../vendor/sparsehash/current/src/windows"
	}
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	-- TODO: Link Breakpad
	
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
	
	configuration "not windows"
		includedirs { "/usr/include/mysql" }
		links { "mysqlclient" }
	
	configuration "windows"
		includedirs { "../../vendor/mysql/include" }
		links { "../../vendor/mysql/lib/x86/libmysql.lib" }
		