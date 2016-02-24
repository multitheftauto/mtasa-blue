solution "MTASA"
	configurations {"Debug", "Release", "Nightly"}
	
	location "Build"
	
	flags { "C++14", "Symbols" }
	characterset "MBCS"
	includedirs { "Shared/sdk" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	
	configuration "Debug"
		defines { "MTA_DEBUG" }
		targetsuffix "_d"
		
	configuration "Release"
		flags { "Optimize" }
		
	configuration "windows"
		defines { "WIN32" }
		
	configuration "*"
		group "Shared"
		-- TODO
		
		group "Server"
		include "Server/core"
		include "Server/dbconmy"
		include "Server/launcher"
		-- TODO
		
		group "Client"
		-- TODO
		
		group "Vendor"
		
		-- TODO
		include "vendor/ehs"
		-- TODO
		include "vendor/json-c"
		include "vendor/libpng"
		-- TODO
		include "vendor/pcre"
		include "vendor/pme"
		include "vendor/portaudio"
		-- TODO
		include "vendor/sqlite"
		include "vendor/tinygettext"
		include "vendor/tinyxml"
		include "vendor/unrar"
		include "vendor/zip"
		include "vendor/zlib"