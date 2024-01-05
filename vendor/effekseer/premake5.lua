project "Effekseer-vendor"
	language "C++"
	kind "StaticLib"
	targetdir(buildpath("mta"))
	targetname "effekseer-vendor"

	defines { "WIN32", "_WIN32", "_WIN32_WINNT=0x601", "_MSC_PLATFORM_TOOLSET=$(PlatformToolsetVersion)", "__EFFEKSEER_USE_LIBPNG__", "__EFFEKSEER_RENDERER_INTERNAL_LOADER__" }
	links { "libpng", "zlib", "d3dx9", "d3d9" }

	includedirs {
		"effekseer-submodule/Dev/Cpp/Effekseer",
		"effekseer-submodule/Dev/Cpp/EffekseerRendererCommon",
		"effekseer-submodule/Dev/Cpp/EffekseerRendererDX9",
		"effekseer-submodule/Dev/Cpp/EffekseerMaterialCompiler",
		"effekseer-submodule/Dev/Cpp/",
		"../libpng",
	}

	vpaths {
		["Headers/*"] = "effekseer-submodule/Dev/Cpp/**.h",
		["Sources/*"] = "effekseer-submodule/Dev/Cpp/**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"effekseer-submodule/Dev/Cpp/Effekseer/**.cpp",
		"effekseer-submodule/Dev/Cpp/Effekseer/**.h",
		"effekseer-submodule/Dev/Cpp/EffekseerRendererCommon/**.cpp",
		"effekseer-submodule/Dev/Cpp/EffekseerRendererCommon/**.h",
		"effekseer-submodule/Dev/Cpp/EffekseerRendererDX9/**.cpp",
		"effekseer-submodule/Dev/Cpp/EffekseerRendererDX9/**.h",
		"effekseer-submodule/Dev/Cpp/EffekseerMaterialCompiler/Common/**.cpp",
		"effekseer-submodule/Dev/Cpp/EffekseerMaterialCompiler/Common/**.h",
		"effekseer-submodule/Dev/Cpp/EffekseerMaterialCompiler/HLSLGenerator/**.cpp",
		"effekseer-submodule/Dev/Cpp/EffekseerMaterialCompiler/HLSLGenerator/**.h",
		"effekseer-submodule/Dev/Cpp/EffekseerMaterialCompiler/DirectX9/**.cpp",
		"effekseer-submodule/Dev/Cpp/EffekseerMaterialCompiler/DirectX9/**.h",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }