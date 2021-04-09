/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CBulletPhysicsCommon.h
 *  PURPOSE:     Bullet physics common
 *
 *****************************************************************************/

#include "bulletphysics3d/btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"

#include "physics/CLuaPhysicsBaseManager.h"
#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"
#include "physics/CLuaPhysicsShapeManager.h"

#pragma once

class CLuaPhysicsElement;

#define USE_NANOSECOND_TIMING 1            // use nanoseconds for profiling?
#define BT_THREADSAFE 1
namespace BulletPhysics
{
    namespace Limits
    {
        inline constexpr CVector WorldMinimumSize(1.f, 1.f, 1.f);
        inline constexpr CVector WorldMaximumSize(8192.f, 8192.f, 8192.f);
        inline constexpr float   MinimumPrimitiveSize = 0.05f;
        inline constexpr float   MaximumPrimitiveSize = 8196.f;
        inline constexpr int     MaximumSubSteps = 256;
        inline constexpr int     RaycastAllUpperResultsLimit = 8196;
        inline constexpr float   RigidBodyMassLimit = 1000000.0f;
    }            // namespace Limits
    namespace Defaults
    {
        inline constexpr CVector Gravity(0.f, 0.f, -9.81f);
        inline constexpr float   RigidBodyMass = 1.f;
        inline constexpr float   RigidBodyLinearDumping = 0.005f;             // 0.0 - 1.0
        inline constexpr float   RigidBodyAngularDumping = 0.005f;            // 0.0 - 1.0
        inline constexpr float   RigidBodyLinearSleepingThreshold = 0.1f;
        inline constexpr float   RigidBodyAngularSleepingThreshold = 0.1f;
        inline constexpr int     FilterGroup = 1;
        inline constexpr int     FilterMask = -1;
    }            // namespace Defaults
}            // namespace BulletPhysics
