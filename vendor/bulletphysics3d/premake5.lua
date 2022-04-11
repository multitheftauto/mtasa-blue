project "LinearMath"
    kind "StaticLib"
    if os.istarget("Linux") then
        buildoptions{"-fPIC"}
    end
    includedirs {
        ".",
    }
    files {
        "LinearMath/*.cpp",
        "LinearMath/*.h",
        "LinearMath/TaskScheduler/*.cpp",
        "LinearMath/TaskScheduler/*.h"
    }

    defines { "BT_THREADSAFE" }

project "BulletCollision"

	kind "StaticLib"
    if os.istarget("Linux") then
        buildoptions{"-fPIC"}
    end
	includedirs {
        ".",
	}
	files {
		"BulletCollision/*.cpp",
		"BulletCollision/*.h",
		"BulletCollision/BroadphaseCollision/*.cpp",
		"BulletCollision/BroadphaseCollision/*.h",
		"BulletCollision/CollisionDispatch/*.cpp",
        "BulletCollision/CollisionDispatch/*.h",
		"BulletCollision/CollisionShapes/*.cpp",
		"BulletCollision/CollisionShapes/*.h",
		"BulletCollision/Gimpact/*.cpp",
		"BulletCollision/Gimpact/*.h",
		"BulletCollision/NarrowPhaseCollision/*.cpp",
		"BulletCollision/NarrowPhaseCollision/*.h",
	}

project "BulletDynamics"
	kind "StaticLib"
	includedirs {
        ".",
	}
    if os.istarget("Linux") then
        buildoptions{"-fPIC"}
    end
	files {
		"BulletDynamics/Dynamics/*.cpp",
        "BulletDynamics/Dynamics/*.h",
        "BulletDynamics/ConstraintSolver/*.cpp",
        "BulletDynamics/ConstraintSolver/*.h",
        "BulletDynamics/Featherstone/*.cpp",
        "BulletDynamics/Featherstone/*.h",
        "BulletDynamics/MLCPSolvers/*.cpp",
        "BulletDynamics/MLCPSolvers/*.h",
        "BulletDynamics/Vehicle/*.cpp",
        "BulletDynamics/Vehicle/*.h",
        "BulletDynamics/Character/*.cpp",
        "BulletDynamics/Character/*.h"
	}