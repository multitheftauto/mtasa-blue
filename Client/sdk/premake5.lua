project "Client SDK"
	language "C++"
	kind "StaticLib"
	targetname "sdk"
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	vpaths { 
		["Headers/*"] = { "**.h", "**.hpp" },
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	includedirs {
		"../../vendor/sparsehash/current/src/"
	}
	
	files {
		"premake5.lua",
		"**.h",
		"**.hpp",
		"**.cpp"
	}
	
	filter "system:windows"
		includedirs { "../../vendor/sparsehash/current/src/windows" }

	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
