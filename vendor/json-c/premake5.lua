project "json-c"
	language "C++"
	kind "StaticLib"
	targetname "json-c"

	includedirs { "." }
	defines { "_LIB" }

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

	filter "system:macosx"
		defines {"HAVE_XLOCALE_H"}

		-- `#include <machine/endian.h>` works fine but `#include <endian.h>` does not
		-- see linkhash.c line 23 (introduced in b95bda01e80359e)
		--
		-- This line should fix this, but it doesn't work because `/usr/local/include`
		-- doesn't exist on macOS. If you want to fix this include, modify
		--  shipped_for_mta_linux to re-enable inclusion of `endian.h` on macOS.
		-- includedirs {"/usr/local/include/machine"}
