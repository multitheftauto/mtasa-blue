project "libspeex"
	language "C"
	kind "StaticLib"
	targetname "libspeex"
	warnings "Off"

	defines {
		"HAVE_CONFIG_H"
	}

	includedirs {
		".",
		"libspeex"
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"speex/*.h",
		"libspeex/*.h",
		"libspeex/*.c",
		"libspeexdsp/*.h",
		"libspeexdsp/*.c"
	}

	excludes {
		-- These files are in libspeex and libspeexdsp
		"libspeexdsp/kiss_fft.c",
		"libspeexdsp/kiss_fftr.c",
		"libspeexdsp/smallft.c",
		"libspeexdsp/fftwrap.c",
	}

	filter "system:windows"
		disablewarnings {
			"4244", -- warning C4244: '=': conversion from '?' to '?', possible loss of data
			"4305", -- warning C4305: 'initializing': truncation from '?' to '?'
			"4018", -- warning C4018: '<': signed/unsigned mismatch
		}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
