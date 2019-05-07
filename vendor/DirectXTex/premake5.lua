project "DirectXTex"
	language "C++"
	kind "StaticLib"
	targetname "DirectXTex"
	
	defines { "_UNICODE","UNICODE","WIN32","NDEBUG","_LIB","_WIN7_PLATFORM_UPDATE","_WIN32_WINNT=0x0601",
			  "_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS" }

	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.c"
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
	filter "system:not windows"
		flags { "ExcludeFromBuild" } 