project "CEFLauncher DLL"
	language "C++"
	kind "SharedLib"
	targetname "ceflauncher_dll"
	
	includedirs { 
		"../../vendor/cef3" 
	}

	defines { "UNICODE" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["*"] = "premake5.lua"
	}
	
	links {
		"../../vendor/cef3/Debug/libcef"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	