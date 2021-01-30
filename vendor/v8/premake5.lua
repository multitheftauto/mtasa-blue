project "v8"
	language "C++"
	targetname "Mtav8"
	kind "SharedLib"
	targetdir(buildpath("server"))
	
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
		
		"StdInc.h",
		"StdInc.cpp",
	}
	
	--linkoptions {
	--	"-stdlib=libc++"
	--}
	
	includedirs {
		".",
		"./include",
		"./sources",
		"./sources/include",
        "./../../Shared/sdk",
		}

	defines { "SDK_WITH_BCRYPT" }

	links {
		"cryptopp", "blowfish_bcrypt"
	}

	links {
		"library/win32bit/v8.dll.lib",
		"library/win32bit/v8_libbase.dll.lib",
		"library/win32bit/v8_libplatform.dll.lib",
	}
