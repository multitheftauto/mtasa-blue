project "maetro32"
	language "C++"
	kind "SharedLib"
	targetname "maetro32"
	targetdir(buildpath("."))
	clangtidy "On"
	staticruntime "Off"
	linkoptions { "/NODEFAULTLIB" }
	flags { "NoRuntimeChecks", "NoBufferSecurityCheck" }

	vpaths {
		["Headers/**"] = "**.h",
		["Sources/**"] = "*.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	postbuildcommands { copy "server", copy "mta", copy "mta/cef" }

	filter "system:not windows or platforms:not x86"
		flags { "ExcludeFromBuild" }

project "maetro64"
	language "C++"
	kind "SharedLib"
	targetname "maetro64"
	targetdir(buildpath("mta"))
	clangtidy "On"
	staticruntime "Off"
	linkoptions { "/NODEFAULTLIB" }
	flags { "NoRuntimeChecks", "NoBufferSecurityCheck" }
	architecture "x86_64"

	vpaths {
		["Headers/**"] = "**.h",
		["Sources/**"] = "*.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	postbuildcommands { copy "server/x64" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter { "platforms:not x86", "platforms:not x64" }
		flags { "ExcludeFromBuild" }
