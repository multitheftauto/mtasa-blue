project "dffapi"
		
language "C"
kind "StaticLib"
targetname "dffapi"

vpaths { 
	["Headers"] = "**.h",
	["Sources"] = "**.c",
	["*"] = "premake5.lua"
}
		
pchheader "StdInc.h"
pchsource "StdInc.c"
	
files {
	"premake5.lua",
	"src/**.c",
	"include/**.h",
	"StdInc.h",
	"StdInc.c",
}
		
includedirs {
	".",
	"include",
}

