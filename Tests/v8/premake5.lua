project "v8Tests"
	language "C++"
	targetname "v8Tests"
	kind "ConsoleApp"

	targetdir(buildpath("tests/v8"))
	
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
	
	includedirs {
		".",
		"./include",
		"./../../vendor/v8/sources",
		"./../../vendor/v8/sources/include",
		}
	
	links {
		"msvcrt.lib",

		"./../../vendor/v8/library/win32bit/v8.dll.lib",
		"./../../vendor/v8/library/win32bit/v8_libbase.dll.lib",
		"./../../vendor/v8/library/win32bit/v8_libplatform.dll.lib",
	}

	--postbuildcommands {
	--  "{COPY} %{cfg.buildtarget.directory}../../../vendor/v8/library/win32bit/dlls/required/v8.dll %{cfg.buildtarget.directory}/v8.dll",
    --}

