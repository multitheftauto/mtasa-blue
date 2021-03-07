/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Enums.h
 *  PURPOSE:     Client/server shared enum definitions
 *
 *****************************************************************************/
#pragma once

namespace EEventPriority
{
    enum EEventPriorityType
    {
        LOW,
        NORMAL,
        HIGH,
    };
}
using EEventPriority::EEventPriorityType;

DECLARE_ENUM(EEventPriority::EEventPriorityType)

namespace EPlayerScreenShotResult
{
    enum EPlayerScreenShotResultType
    {
        NONE,
        SUCCESS,
        MINIMIZED,
        DISABLED,
        ERROR_,
    };
}
using EPlayerScreenShotResult::EPlayerScreenShotResultType;

DECLARE_ENUM(EPlayerScreenShotResult::EPlayerScreenShotResultType)

namespace EDebugHook
{
    enum EDebugHookType
    {
        PRE_EVENT,
        POST_EVENT,
        PRE_FUNCTION,
        POST_FUNCTION,
        PRE_EVENT_FUNCTION,
        POST_EVENT_FUNCTION,
        MAX_DEBUG_HOOK_TYPE
    };
}
using EDebugHook::EDebugHookType;

DECLARE_ENUM(EDebugHook::EDebugHookType);

enum eEulerRotationOrder
{
    EULER_DEFAULT,
    EULER_ZXY,
    EULER_ZYX,
    EULER_MINUS_ZYX,
};

DECLARE_ENUM(eEulerRotationOrder);

DECLARE_ENUM(EHashFunction::EHashFunctionType);
DECLARE_ENUM_CLASS(PasswordHashFunction);
DECLARE_ENUM_CLASS(StringEncryptFunction);


enum class ePhysicsDebugMode
{
    // Bullet physics specific
    NoDebug = -1,
    DrawWireframe,
    DrawAabb,
    DrawFeaturesText,
    DrawContactPoints,
    NoDeactivation,
    NoHelpText,
    DrawText,
    ProfileTimings,
    EnableSatComparison,
    DisableBulletLCP,
    EnableCCD,
    DrawConstraints,
    DrawConstraintLimits,
    FastWireframe,
    DrawNormals,
    DrawFrames,
    // Custom
    LINE_WIDTH,
    DRAW_DISTANCE,
    COUNT,
};
DECLARE_ENUM_CLASS(ePhysicsDebugMode);

enum class ePhysicsProperty
{
    MASS,
    SLEEPING_LINEAR_THRESHOLDS,
    SLEEPING_ANGULAR_THRESHOLDS,
    RESTITUTION,
    SCALE,
    DEBUG_COLOR,
    FILTER_MASK,
    FILTER_GROUP,
    SIZE,
    RADIUS,
    HEIGHT,
    MOTION_CCD_THRESHOLD,
    GRAVITY,
    SWEPT_SPHERE_RADIUS,
    SLEEP,
    WANTS_SLEEPING,
    // constraints properties
    STIFFNESS,
    PIVOT_A,
    PIVOT_B,
    LOWER_LIN_LIMIT,
    UPPER_LIN_LIMIT,
    LOWER_ANG_LIMIT,
    UPPER_ANG_LIMIT,
    BREAKING_IMPULSE_THRESHOLD,
    APPLIED_IMPULSE,
    RIGID_BODY_A,
    RIGID_BODY_B,
    JOINTS_FEEDBACK,
    ENABLED,
    COUNT,
};
DECLARE_ENUM_CLASS(ePhysicsProperty);

enum class ePhysicsWorldProperty
{
    GRAVITY,
    USE_CONTINOUS,
    SIMULATION_ENABLED,
    SUBSTEPS,
    WORLDSIZE,
    TRIGGEREVENTS,
    TRIGGERCOLLISIONEVENTS,
    TRIGGERCONSTRAINTEVENTS,
    COUNT,
};
DECLARE_ENUM_CLASS(ePhysicsWorldProperty);

enum class ePhysicsElementType
{
    Unknown,
    WorldElement,
    RigidBody,
    StaticCollision,

    // Shapes
    Shape,
    ConvexShape,
    ConcaveShape,
    BoxShape,
    SphereShape,
    ConeShape,
    BvhTriangleMeshShape,
    CompoundShape,
    ConvexHullShape,
    CylinderShape,
    GimpactTriangleMeshShape,
    HeightfieldTerrainShape,

    // Constraints
    Constraint,
    PointToPointConstraint,
};
DECLARE_ENUM_CLASS(ePhysicsElementType);

// For internal usage for physics
enum class ePhysicsWorld
{
    DiscreteDynamicsWorld,
    DiscreteDynamicsWorldMt,
    Count,
};


DECLARE_ENUM(ePacketID);
