project "Mtav8"
	language "C++"
	targetname "Mtav8"
	kind "SharedLib"

	targetdir(buildpath("server/v8"))
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths { 
		["Headers"] = "include/*.h",
		["Sources"] = "src/*.cpp",
		["*"] = {"premake5.lua", "StdInc.h", "StdInc.cpp"}
	}
	
	files {
		"premake5.lua",
		"src/*.cpp",
		"include/*.h",
		"v8/**.h",
		"**.hpp",
		
		"StdInc.h",
		"StdInc.cpp",
	}
	
	--linkoptions {
	--	"-stdlib=libc++"
	--}
	
	includedirs {
		".",
		"./include",
		"./../../vendor/v8/sources",
		"./../../vendor/v8/sources/include",
		"../sdk",
		"../mods/deathmatch/logic/**.h",
	}
    
	links {
		"./../../vendor/v8/library/win32bit/v8.dll.lib",
		"./../../vendor/v8/library/win32bit/v8_libbase.dll.lib",
		"./../../vendor/v8/library/win32bit/v8_libplatform.dll.lib",
	}
	

