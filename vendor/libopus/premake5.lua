project "libopus"
	language "C"
	kind "StaticLib"
	targetname "libopus"
	
	disablewarnings {
		"4244", -- warning C4244: '=': conversion from '?' to '?', possible loss of data
	}

	defines {
		"OPUS_BUILD",
		"USE_ALLOCA"
	}

	includedirs {
		"include",
		"src",
		"celt",
		"silk",
		"silk/fixed",
		"silk/float"
	}
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"include/*.h",
		"src/*.h",
		"src/*.c",
		"celt/*.h",
		"celt/*.c",
		"silk/fixed/*.h",
		"silk/fixed/*.c",
		"silk/float/*.h",
		"silk/float/*.c",
		"silk/*.h",
		"silk/*.c"
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }