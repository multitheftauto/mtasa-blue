project "Launcher"
	language "C++"
	kind "ConsoleApp"
	targetdir(buildpath("server"))
	targetname "mta-server"

	includedirs {
		"../../Shared/sdk",
		"../sdk",
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"*.rc", "**.ico"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	filter "system:windows"
		targetname "MTA Server"
		staticruntime "On"
		files {
			"launcher.rc",
			"resource/mtaicon.ico"
		}

	filter "system:not windows"
		links { "dl" }
		buildoptions { "-pthread", "-fvisibility=default" }
		linkoptions { "-pthread", "-rdynamic" }

	filter "platforms:arm"
		targetname "mta-server-arm"

	filter "platforms:arm64"
		targetname "mta-server-arm64"

	filter { "system:linux", "platforms:x64" }
		targetname "mta-server64"

	filter { "system:windows", "platforms:x64" }
		targetname "MTA Server64"

	filter { "system:windows", "platforms:arm" }
		targetname "MTA Server ARM"

	filter { "system:windows", "platforms:arm64" }
		targetname "MTA Server ARM64"
