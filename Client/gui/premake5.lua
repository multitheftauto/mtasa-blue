project "GUI"
	language "C++"
	kind "SharedLib"
	targetname "gui"
	
	includedirs { 
		"../sdk",
		"../../vendor/cegui-0.4.0-custom/include",
		"../../vendor/sparsehash/current/src/",
		"../../vendor/sparsehash/current/src/windows"
	}
	 
	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	links {
		"CEGUI", "DirectX9GUIRenderer", "Falagard",
		"../../vendor/cegui-0.4.0-custom/lib/freetype2412ST.lib", -- Todo: Select Debug variant, also move freetype to vendor
		"d3dx9.lib", "dxerr.lib"
	}
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources"] = "*.c",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	