
project "BulletPhysics"
language "C++"
kind "StaticLib"
targetname "bulletphysics"
characterset "MBCS"

disablewarnings {"4244", "4267"}
floatingpoint "Fast"
stringpooling "on"

defines { "SKIP_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD", "NO_OPENGL3" }

filter "system:not windows"
    links { "rt" }
    buildoptions { "-pthread", "-fPIC" }
    linkoptions { "-pthread" }
    
filter {}

includedirs { "./" }

vpaths {
    ["Bullet3Collision/Headers/*"] = "Bullet3Collision/**.h",
    ["Bullet3Collision/Sources/*"] = "Bullet3Collision/**.cpp",
    ["Bullet3Dynamics/Headers/*"] = "Bullet3Dynamics/**.h",
    ["Bullet3Dynamics/Sources/*"] = "Bullet3Dynamics/**.cpp",
    ["LinearMath/Headers/*"] = "LinearMath/**.h",
    ["LinearMath/Sources/*"] = "LinearMath/**.cpp",
    ["*"] = {"premake5.lua", "btBulletCollisionCommon.h", "btBulletDynamicsCommon.h"}
}

files {
    "premake5.lua",

    -- Collisions:
    "Bullet3Collision/BroadPhaseCollision/b3BroadphaseCallback.h",
    "Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.h",
    "Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.h",
    "Bullet3Collision/BroadPhaseCollision/b3OverlappingPair.h",
    "Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.h",
    "Bullet3Collision/BroadPhaseCollision/shared/b3Aabb.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3BvhSubtreeInfoData.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3BvhTraversal.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3ClipFaces.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3Collidable.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3Contact4Data.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3ContactConvexConvexSAT.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3ContactSphereSphere.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3ConvexPolyhedronData.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3FindConcaveSatAxis.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3FindSeparatingAxis.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3MprPenetration.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3NewContactReduction.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3QuantizedBvhNodeData.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3ReduceContacts.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3RigidBodyData.h",
    "Bullet3Collision/NarrowPhaseCollision/shared/b3UpdateAabbs.h",
    "Bullet3Collision/NarrowPhaseCollision/b3Config.h",
    "Bullet3Collision/NarrowPhaseCollision/b3Contact4.h",
    "Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.h",
    "Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.h",
    "Bullet3Collision/NarrowPhaseCollision/b3RaycastInfo.h",
    "Bullet3Collision/NarrowPhaseCollision/b3RigidBodyCL.h",

    "Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp",
    "Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp",
    "Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp",
    "Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp",
    "Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp",

    -- Dynamics:
    "Bullet3Dynamics/b3CpuRigidBodyPipeline.h",
    "Bullet3Dynamics/ConstraintSolver/b3ContactSolverInfo.h",
    "Bullet3Dynamics/ConstraintSolver/b3FixedConstraint.h",
    "Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.h",
    "Bullet3Dynamics/ConstraintSolver/b3JacobianEntry.h",
    "Bullet3Dynamics/ConstraintSolver/b3PgsJacobiSolver.h",
    "Bullet3Dynamics/ConstraintSolver/b3Point2PointConstraint.h",
    "Bullet3Dynamics/ConstraintSolver/b3SolverBody.h",
    "Bullet3Dynamics/ConstraintSolver/b3SolverConstraint.h",
    "Bullet3Dynamics/ConstraintSolver/b3TypedConstraint.h",
    "Bullet3Dynamics/shared/b3ContactConstraint4.h",
    "Bullet3Dynamics/shared/b3ConvertConstraint4.h",
    "Bullet3Dynamics/shared/b3Inertia.h",
    "Bullet3Dynamics/shared/b3IntegrateTransforms.h",

    "Bullet3Dynamics/b3CpuRigidBodyPipeline.cpp",
    "Bullet3Dynamics/ConstraintSolver/b3FixedConstraint.cpp",
    "Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.cpp",
    "Bullet3Dynamics/ConstraintSolver/b3PgsJacobiSolver.cpp",
    "Bullet3Dynamics/ConstraintSolver/b3Point2PointConstraint.cpp",
    "Bullet3Dynamics/ConstraintSolver/b3TypedConstraint.cpp",

    "LinearMath/btAabbUtil2.h",
    "LinearMath/btAlignedAllocator.h",
    "LinearMath/btAlignedObjectArray.h",
    "LinearMath/btConvexHull.h",
    "LinearMath/btConvexHullComputer.h",
    "LinearMath/btDefaultMotionState.h",
    "LinearMath/btGeometryUtil.h",
    "LinearMath/btGrahamScan2dConvexHull.h",
    "LinearMath/btHashMap.h",
    "LinearMath/btIDebugDraw.h",
    "LinearMath/btList.h",
    "LinearMath/btMatrix3x3.h",
    "LinearMath/btMinMax.h",
    "LinearMath/btMotionState.h",
    "LinearMath/btPolarDecomposition.h",
    "LinearMath/btPoolAllocator.h",
    "LinearMath/btQuadWord.h",
    "LinearMath/btQuaternion.h",
    "LinearMath/btQuickprof.h",
    "LinearMath/btRandom.h",
    "LinearMath/btScalar.h",
    "LinearMath/btSerializer.h",
    "LinearMath/btStackAlloc.h",
    "LinearMath/btThreads.h",
    "LinearMath/btTransform.h",
    "LinearMath/btTransformUtil.h",
    "LinearMath/btVector3.h",
    "LinearMath/TaskScheduler/btThreadSupportInterface.h",

    "LinearMath/btAlignedAllocator.cpp",
    "LinearMath/btConvexHull.cpp",
    "LinearMath/btConvexHullComputer.cpp",
    "LinearMath/btGeometryUtil.cpp",
    "LinearMath/btPolarDecomposition.cpp",
    "LinearMath/btQuickprof.cpp",
    "LinearMath/btSerializer.cpp",
    "LinearMath/btSerializer64.cpp",
    "LinearMath/btThreads.cpp",
    "LinearMath/btVector3.cpp",
    "LinearMath/TaskScheduler/btTaskScheduler.cpp",
    "LinearMath/TaskScheduler/btThreadSupportPosix.cpp",
    "LinearMath/TaskScheduler/btThreadSupportWin32.cpp",

    "btBulletCollisionCommon.h",
    "btBulletDynamicsCommon.h"
}