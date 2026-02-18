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

    filter "system:not windows"
        flags { "ExcludeFromBuild" }

	filter {"platforms:x86"}
		postbuildcommands { copy "server", copy "mta", copy "mta/cef" }

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))

	filter "platforms:arm"
		targetdir(buildpath("server/arm"))

	filter "platforms:arm64"
		targetdir(buildpath("server/arm64"))
