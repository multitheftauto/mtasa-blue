project "pcre"
	language "C++"
	kind "SharedLib"
	targetname "pcre3"
	
	defines { "HAVE_CONFIG_H" }
	includedirs { ".", "shipped_for_mta_win32" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["Sources/*"] = "**.cc",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		
		"*.h",
		"*.cc",
		"*.c"
	}
	
	excludes {
		"pcredemo.c",
		"pcretest.c",
		"dftables.c",
		"pcre_scanner_unittest.cc",
		"pcre_stringpiece.unittest.cc",
		"pcrecpp_unittest.cc",
		"pcregrep.c"
	}
	