project "SDK"
	language "C++"
	kind "SharedLib"
	targetname "sdk"
	targetdir(buildpath("mta"))
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	vpaths { 
		["Headers/*"] = { "**.h", "**.hpp" },
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	filter "system:windows"
		includedirs { "../../vendor/sparsehash/current/src/windows" }
	
	includedirs {
		"../../vendor/sparsehash/current/src/"
	}
	
	files {
		"premake5.lua",
		"**.h",
		"**.hpp",
		"**.cpp"
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
		
	filter "system:not windows"
		flags { "ExcludeFromBuild" }
