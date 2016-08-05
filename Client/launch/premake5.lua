project "Client Launcher"
	language "C++"
	kind "WindowedApp"
	targetname "Multi Theft Auto"
	targetdir(buildpath("."))
	
	includedirs { "../sdk" }
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	flags { "WinMain" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"*.rc", "*.ico", "**.xml"},
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	
	filter "system:windows"
		files {
			"NEU/GDFImp.gdf.xml",
			"GDFImp.rc",
			"launch.rc",
			"mtaicon.ico"
		}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
		
	filter "system:not windows"
		flags { "ExcludeFromBuild" } 