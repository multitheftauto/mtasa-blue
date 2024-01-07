project "discord-rpc"
	targetname "discord-rpc"
	language "C++"
	kind "StaticLib"

	includedirs { 
		"discord/include",
		"discord/thirdparty/rapidjson/include"
	}

	defines {
		"DISCORD_DISABLE_IO_THREAD"
	}

	files {
		"premake5.lua",
		"discord/src/discord_rpc.cpp",
		"discord/src/rpc_connection.cpp",
		"discord/src/serialization.cpp",
		"discord/src/connection_win.cpp",
		"discord/src/discord_register_win.cpp",
	}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }