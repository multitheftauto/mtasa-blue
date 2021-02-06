project "v8"
	language "C++"
	targetname "v8"
	kind "SharedLib"
	targetdir(buildpath("server"))
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths { 
		["*"] = {"premake5.lua", "StdInc.h", "StdInc.cpp"}
	}
	
	files {
		"premake5.lua",
		
		"StdInc.h",
		"StdInc.cpp",
		"v8/**.h",
	}
	
	--linkoptions {
	--	"-stdlib=libc++"
	--}
	
	includedirs {
		".",
		"./include",
		"./sources",
		"./sources/include",
	}

