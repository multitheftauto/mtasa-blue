project "CEFLauncher DLL"
	language "C++"
	kind "SharedLib"
	targetname "ceflauncher_dll"
	targetdir(buildpath("Client"))
	
	includedirs { 
		"../../vendor/cef3" 
	}

	defines { "UNICODE" }
	
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
	
	links { "CEF" }
		
	filter "configurations:Debug"
		links { "../../vendor/cef3/Debug/libcef.lib" }

	filter "configurations:Release"
		links { "../../vendor/cef3/Release/libcef.lib" }

	