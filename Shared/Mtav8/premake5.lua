project "Mtav8"
	language "C++"
	targetname "Mtav8"
	kind "SharedLib"
	targetdir(buildpath("server/v8"))
	
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths { 
		["Headers"] = "include/*.h",
		["Headers/Classes"] = "include/classes/*.h",
		["Headers/Async"] = "include/async/*.h",
		["Sources"] = "src/*.cpp",
		["Sources/Classes"] = "src/classes/*.cpp",
		["Sources/Async"] = "src/async/*.cpp",
		["*"] = {"premake5.lua", "StdInc.h", "StdInc.cpp"}
	}
	
	files {
		"premake5.lua",
		"src/*.cpp",
		"src/async/*.cpp",
		"src/classes/*.cpp",
		"include/*.h",
		"include/async/*.h",
		"include/classes/*.h",
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
		"../sdk"
		}

	defines { "SDK_WITH_BCRYPT" }

	links {
		"cryptopp", "blowfish_bcrypt",
		
		"./../../vendor/v8/library/win32bit/v8.dll.lib",
		"./../../vendor/v8/library/win32bit/v8_libbase.dll.lib",
		"./../../vendor/v8/library/win32bit/v8_libplatform.dll.lib",
	}
	
