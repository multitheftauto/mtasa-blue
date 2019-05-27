if os.target() == "windows" then
	project "DffAPI"
		
		language "C++"
		kind "SharedLib"
		targetname "dffapi"
		targetdir(buildpath("mods/deathmatch"))

		vpaths { 
			["Headers"] = "**.h",
			["Sources"] = "**.c",
			["*"] = "premake5.lua"
		}
		
		pchheader "StdInc.h"
		pchsource "StdInc.c"
	
		files {
			"premake5.lua",
			"src/**.c",
			"include/**.h",
			"**.h",
			"**.c"
		}
		
		filter {}
		includedirs {
			".",
			"include",
		}
	

        filter "platforms:x64"
            flags { "ExcludeFromBuild" } 
				
end
