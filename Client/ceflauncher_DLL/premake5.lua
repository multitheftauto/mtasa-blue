project "CEFLauncher DLL"
	language "C++"
	kind "SharedLib"
	targetname "CEFLauncher_DLL"
	targetdir(buildpath("mta/cef"))

	includedirs { "../../vendor/cef3/cef" }
	libdirs { "../../vendor/cef3/cef/Release" }

	defines { "UNICODE", "PSAPI_VERSION=1" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}


	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	links { "delayimp", "CEF",  "libcef.lib", "Psapi.lib", "version.lib", "Winmm.lib", "Ws2_32.lib", "DbgHelp.lib" }
	linkoptions { "/DELAYLOAD:libcef.dll" }

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
