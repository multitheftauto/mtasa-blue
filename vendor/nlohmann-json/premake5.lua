project "nlohmann-json"
  targetname "nlohmann-json"
  language "C++"
  kind "None" -- we want header-only

  includedirs {
      "nlohmann/single_include",
      "nlohmann/include",
  }

  defines {
      "JSON_NO_IO",
      "JSON_HAS_CPP_17",
  }

  files {
      "nlohmann/single_include/nlohmann/*.hpp",
      "nlohmann/include/nlohmann/*.hpp",
      "nlohmann/include/nlohmann/detail/*/*.hpp",
      "nlohmann/include/nlohmann/thirdparty/*/*.hpp",
  }

	--filter "architecture:not x86"
	--	flags { "ExcludeFromBuild" }
	--filter "system:not windows"
	--	flags { "ExcludeFromBuild" }