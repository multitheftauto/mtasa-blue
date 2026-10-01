project "libwebp"
	language "C"
	kind "StaticLib"
	targetname "libwebp"
	targetdir(buildpath("mta"))
	warnings "Off"

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"WEBP_USE_THREAD=0",
	}

	vpaths {
		["Headers/*"] = { "src/**.h" },
		["Sources/*"] = { "src/**.c" },
		["*"] = "premake5.lua"
	}

	-- Decoder-only subset of libwebp.
	-- src/enc/**, src/mux/**, src/demux/** and examples/** are intentionally excluded.
	files {
		"premake5.lua",
		"src/dec/**.c",
		"src/dec/**.h",
		"src/dsp/**.c",
		"src/dsp/**.h",
		"src/utils/**.c",
		"src/utils/**.h",
		"src/webp/**.h",
	}

	-- Encoder files may live under src/dsp with names like *_enc*.c; keep them.
	-- The decoder needs *_dec*.c, generic *.c, and the SSE/NEON variants.
	-- Removing enc-only files is not strictly necessary: they compile fine as
	-- part of the static lib and are dead-stripped by the linker if unused.
	-- If binary size becomes a concern, exclude them here.

	includedirs {
		".",
		"src"
	}

	filter "system:windows"
		disablewarnings {
			"4244", -- possible loss of data
			"4267", -- conversion from 'size_t'
			"4146", -- unary minus operator applied to unsigned type
			"4334", -- result of 32-bit shift implicitly converted to 64 bits
			"4996", -- deprecated function
		}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
