project "ide-ipl-lib"
	language "C++"
	kind "StaticLib"
	targetname "ide-ipl-lib"

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"types/bbox.h",
		"types/bsphere.h",
		"types/entry.h",
		"types/ideipl_line.h",
		"types/point.h",
		"types/quat.h",
		"types/rgba.h",
		"types/section.h",
		"types/ideipl_line.cpp",
		"ide/gtasa.h",
		"ide/gtasa.cpp",
		"*.cpp"
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }
