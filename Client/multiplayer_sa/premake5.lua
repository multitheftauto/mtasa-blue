project "Multiplayer SA"
	language "C++"
	kind "SharedLib"
	targetname "multiplayer_sa"
	targetdir(buildpath("mta"))

	cppdialect "C++20" 

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }

	filter {}
		includedirs {
			"../../Shared/sdk",
			"../sdk",
			"../../vendor/sparsehash/src/",
		}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

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

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }

	filter { "configurations:Release or configurations:Nightly",
        "files:CMultiplayerSA.cpp" .. " or " ..
        "files:CMultiplayerSA_1.3.cpp" .. " or " ..
        "files:CMultiplayerSA_ClothesCache.cpp" .. " or " ..
        "files:CMultiplayerSA_ClothesMemFix.cpp" .. " or " ..
        "files:CMultiplayerSA_ClothesSpeedUp.cpp" .. " or " ..
        "files:CMultiplayerSA_CrashFixHacks.cpp" .. " or " ..
        "files:CMultiplayerSA_Direct3D.cpp" .. " or " ..
        "files:CMultiplayerSA_Files.cpp" .. " or " ..
        "files:CMultiplayerSA_FixBadAnimId.cpp" .. " or " ..
        "files:CMultiplayerSA_FixLineOfSightArgs.cpp" .. " or " ..
        "files:CMultiplayerSA_HookDestructors.cpp" .. " or " ..
        "files:CMultiplayerSA_LicensePlate.cpp" .. " or " ..
        "files:CMultiplayerSA_ObjectLODSystem.cpp" .. " or " ..
        "files:CMultiplayerSA_Rendering.cpp" .. " or " ..
        "files:CMultiplayerSA_RwResources.cpp" .. " or " ..
        "files:CMultiplayerSA_VehicleDamage.cpp" .. " or " ..
        "files:CMultiplayerSA_VehicleLights.cpp" .. " or " ..
        "files:CMultiplayerSA_Weapons.cpp" .. " or " ..
        "files:CPopulationSA.cpp" .. " or " ..
        "files:multiplayer_keysync.cpp" .. " or " ..
        "files:multiplayer_shotsync.cpp" }
        buildoptions { "/Ob1" }   -- Expand only functions marked as inline
