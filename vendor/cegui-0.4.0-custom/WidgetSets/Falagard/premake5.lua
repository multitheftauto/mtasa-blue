project "Falagard"
	language "C++"
	kind "StaticLib"
	targetname "Falagard"
	
	pchheader "StdInc.h"
	pchsource "src/StdInc.cpp"
	
	defines { "FALAGARDBASE_EXPORTS" }
	
	includedirs { 
		"include",
		"../../include" -- CEGUI Includes
	}
	 
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"src/**.cpp",
		"include/**.h",
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
	filter "system:not windows"
		flags { "ExcludeFromBuild" } 
	
	filter {"system:windows"}
		disablewarnings { "4297" }