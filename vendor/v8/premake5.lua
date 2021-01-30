project "v8"
	language "C++"
	targetname "Mtav8"
	kind "SharedLib"
	--kind "StaticLib"
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
		--"./v8/v8/buildtools/third_party/libc++/trunk",
		--"./v8/v8/out/ia32.release/gen",
		--"./v8/v8/buildtools/third_party/libc++/trunk/utils/google-benchmark/include"
		}

	--defines { "BUILDING_V8_SHARED" }
	--defines { "BUILDING_V8_PLATFORM_SHARED", "BUILDING_V8_SHARED" }
-- 	defines { "USING_V8_PLATFORM_SHARED", "USING_V8_SHARED", "USING_V8_BASE_SHARED" }

	defines { "SDK_WITH_BCRYPT" }

	links {
		"cryptopp", "blowfish_bcrypt"
	}

	links {
		"library/win32bit/v8.dll.lib",
		"library/win32bit/v8_libbase.dll.lib",
		"library/win32bit/v8_libplatform.dll.lib",

		--[["library/win32bit/bytecode_builtins_list_generator.lib",
		"library/win32bit/icui18n.dll.lib",
		"library/win32bit/icuuc.dll.lib",
		"library/win32bit/libc++.dll.lib",
		"library/win32bit/mksnapshot.lib",
		"library/win32bit/torque.lib",
		"library/win32bit/zlib.dll.lib",]]
	}
