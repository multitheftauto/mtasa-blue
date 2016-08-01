project "pthread"
	language "C++"
	kind "SharedLib"
	targetname "pthread"
	targetdir(buildpath("server"))
		
	includedirs { 
		"include"
	}

	vpaths { 
		["Headers"] = "**.h",
		["Sources"] = "**.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"include/pthread.c",
		"include/config.h",
		"include/context.h",
		"include/implement.h",
		"include/need_errno.h",
		"include/pthread.h",
		"include/sched.h",
		"include/semaphore.h",
	}

	filter {"system:windows", "platforms:x86"}
		postbuildcommands {
			copy "mta"
		}		

	filter "system:windows"
		defines {
			"HAVE_PTW32_CONFIG_H",
			"PTW32_BUILD_INLINED"
		}
		
	