	project "Bullet3Dynamics"

	language "C++"
				
	kind "StaticLib"

	includedirs {
		".."
	}		
	
    if os.istarget("Linux") then
        buildoptions{"-fPIC"}
    end

	files {
		"**.cpp",
		"**.h"
	}