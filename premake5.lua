solution "Freedomity"
	configurations {"Debug", "Release", "Nightly"}
	
	location "Build"
	
	flags { "C++14", "Symbols" }
	includedirs { "Shared/sdk" }
	
	configuration "Debug"
		defines { "MTA_DEBUG" }
		targetsuffix "_d"
		
	configuration "Release"
		flags { "Optimize" }
		
	configuration "*"
		group "Shared"
		-- TODO
		
		group "Server"
		include "Server/core"
		-- TODO
		
		group "Client"
		-- TODO
		
		group "Vendor"
		-- TODO