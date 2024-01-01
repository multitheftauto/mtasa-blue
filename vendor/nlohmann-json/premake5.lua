project "nlohmann-json"
	targetname "nlohmann-json"
	language "C++"
	kind "StaticLib"

	includedirs {
		"nlohmann/include",
		"nlohmann/single_include",
    
		"nlohmann/include/thirdparty/hedley",
		"nlohmann/include/detail/output",
		"nlohmann/include/detail/meta",
		"nlohmann/include/detail/meta/call_std",
		"nlohmann/include/detail/iterators",
		"nlohmann/include/detail/input",
		"nlohmann/include/detail/conversions",
	}

	defines {
		"JSON_NO_IO",
		"JSON_HAS_CPP_17",
	}

	files {
		"premake5.lua",
		"nlohmann/single_include/nlohmann/*.hpp",

		"nlohmann/include/nlohmann/*.hpp",
		"nlohmann/include/nlohmann/detail/*/*.hpp",
		"nlohmann/include/nlohmann/thirdparty/*/*.hpp",
	}

	--filter "architecture:not x86"
	--	flags { "ExcludeFromBuild" }
	--filter "system:not windows"
	--	flags { "ExcludeFromBuild" }