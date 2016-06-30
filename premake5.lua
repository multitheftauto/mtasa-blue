solution "MTASA"
	configurations {"Debug", "Release", "Nightly"}
	
	location "Build"
	
	flags { "C++14", "Symbols" }
	characterset "MBCS"
	
	includedirs { 
		"Shared/sdk", 
		"vendor"
	}

	defines { 
		"_CRT_SECURE_NO_WARNINGS",
		"_SCL_SECURE_NO_WARNINGS",
		"_CRT_NONSTDC_NO_DEPRECATE"
	}
	
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
		include "Server/mods/deathmatch"
		-- TODO
		
		group "Client"
		include "Client/ceflauncher"
		include "Client/ceflauncher_DLL"
		include "Client/core"
		include "Client/game_sa"
		include "Client/gui"
		include "Client/launch"
		include "Client/loader"
		include "Client/multiplayer_sa"
		-- TODO
		--include "Client/mods/deathmatch"
		
		
		group "Vendor"
		
		include "vendor/cryptopp"
		include "vendor/ehs"
		include "vendor/json-c"
		include "vendor/libpng"
		include "vendor/lua"
		include "vendor/pthreads"
		include "vendor/pcre"
		include "vendor/pme"
		include "vendor/portaudio"
		include "vendor/sqlite"
		include "vendor/tinygettext"
		include "vendor/tinyxml"
		include "vendor/unrar"
		include "vendor/zip"
		include "vendor/zlib"