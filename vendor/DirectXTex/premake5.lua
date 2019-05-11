project "DirectXTex"
	language "C++"
	kind "StaticLib"
	targetname "DirectXTex"
	
	defines { "_UNICODE","UNICODE","WIN32","NDEBUG","_LIB","_WIN7_PLATFORM_UPDATE","_WIN32_WINNT=0x0601",
			  "_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS" }

	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"BC.cpp", "BC4BC5.cpp", "BC6HBC7.cpp", "BCDirectCompute.cpp", "DirectXTexCompress.cpp", "DirectXTexCompressGPU.cpp", 
		"DirectXTexConvert.cpp", "DirectXTexDDS.cpp", "DirectXTexFlipRotate.cpp", "DirectXTexHDR.cpp", "DirectXTexImage.cpp", 
		"DirectXTexMipmaps.cpp", "DirectXTexMisc.cpp", "DirectXTexNormalMaps.cpp", "DirectXTexPMAlpha.cpp", "DirectXTexResize.cpp", 
		"DirectXTexTGA.cpp", "DirectXTexUtil.cpp","DirectXTexWIC.cpp"
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
	filter "system:not windows"
		flags { "ExcludeFromBuild" } 