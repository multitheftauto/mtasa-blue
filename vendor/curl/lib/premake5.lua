project "curl"
	language "C++"
	kind "SharedLib"
	targetname "libcurl"
	targetdir(buildpath("server"))
	
	includedirs { 
		"../lib",
		"../include",
		"../../zlib"
	}
	disablewarnings { "4996" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"**.h",
		"**.c"
	}
	removefiles {
		"amigaos.c",
		"amigaos.h",
		"config-amigaos.h",
		"config-mac.h",
		"config-os400.h",
		"config-riscos.h",
		"config-symbian.h",
		"config-tpf.h",
		"config-win32ce.h",
		"setup-os400.h",
	}
	
	defines {
		"BUILDING_LIBCURL",
		"HAVE_LIBZ",
		"HAVE_ZLIB_H",
		"CURL_DISABLE_LDAP"
	}
	
	links { "zlib" }

	filter "system:windows"
		defines { 
			"USE_WINDOWS_SSPI",
			"USE_SCHANNEL",
			"USE_WIN32_IDN",
			"WANT_IDN_PROTOTYPES"
		}
		links { "ws2_32", "Normaliz", "Crypt32" }
		
	filter {"system:windows", "platforms:x86"}
		postbuildcommands {
			copy "mta"
		}

	filter {"system:windows", "platforms:x86", "configurations:Debug"}
		postbuildcommands {
			-- Fix net(c).dll requiring the release build
			"copy \"%{wks.location}..\\Bin\\server\\libcurl_d.dll\" \"%{wks.location}..\\Bin\\mta\\libcurl.dll\"",
			"copy \"%{wks.location}..\\Bin\\server\\libcurl_d.dll\" \"%{wks.location}..\\Bin\\server\\libcurl.dll\""
		}

	filter {"system:windows", "platforms:x64", "configurations:Debug"}
		postbuildcommands {
			-- Fix net.dll requiring the release build
			"copy \"%{wks.location}..\\Bin\\server\\x64\\libcurl_d.dll\" \"%{wks.location}..\\Bin\\server\\x64\\libcurl.dll\""
		}

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))
	
