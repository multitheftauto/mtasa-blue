solution "ml_base"
	configurations { "Debug", "Release" }
	location ( "Build" )
	targetdir "Bin/%{cfg.buildcfg}" 
	
	flags { "C++14" }
	platforms { "x86", "x64" }
	pic "On"
	symbols "On"

	filter "system:windows"
		defines { "WINDOWS", "WIN32" }

	filter "configurations:Debug"
		defines { "DEBUG" }

	filter "configurations:Release"
		flags { "Optimize" }

	
	project "ml_base"
		language "C++"
		kind "SharedLib"
		targetname "ml_base"
		
		includedirs { "include" }
		libdirs { "lib" }

		vpaths {
			["Headers/*"] = "**.h",
			["Sources/*"] = "**.cpp",
			["*"] = "premake5.lua"
		}

		files {
			"premake5.lua",
			"**.cpp",
			"**.h"
		}

		filter {"system:linux", "platforms:x86" }
			linkoptions { "-Wl,-rpath=mods/deathmatch" }

		filter {"system:linux", "platforms:x64" }
			linkoptions { "-Wl,-rpath=x64" }

		filter "system:linux"
			linkoptions { "-l:lua5.1.so" }

		filter "system:windows"
			links { "lua5.1" }

