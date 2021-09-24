project "ImGui"
	language "C++"
	kind "SharedLib"
	targetname "ImGui"
	targetdir(buildpath("mta"))

	includedirs {
		".",
		"../freetype/include",
	}

    links {
        "freetype"
    }

    defines {
        "IMGUI_ENABLE_FREETYPE",
        "IMGUI_IMPL_WIN32_DISABLE_GAMEPAD",
    }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.h",
        "**.cpp",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter {"system:windows"}
        cppdialect "C++17"
		staticruntime "On"