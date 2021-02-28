/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        shared/mods/deathmatch/logic/lua/CLuaSharedFunctionParseHelpers.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"

IMPLEMENT_ENUM_CLASS_BEGIN(ePhysicsProperty)
ADD_ENUM(ePhysicsProperty::MASS, "mass")
ADD_ENUM(ePhysicsProperty::SLEEPING_LINEAR_THRESHOLDS, "sleepinglinearthresholds")
ADD_ENUM(ePhysicsProperty::SLEEPING_ANGULAR_THRESHOLDS, "sleepingangularthresholds")
ADD_ENUM(ePhysicsProperty::RESTITUTION, "restitution")
ADD_ENUM(ePhysicsProperty::SCALE, "scale")
ADD_ENUM(ePhysicsProperty::DEBUG_COLOR, "debugcolor")
ADD_ENUM(ePhysicsProperty::FILTER_MASK, "filtermask")
ADD_ENUM(ePhysicsProperty::FILTER_GROUP, "filtergroup")
ADD_ENUM(ePhysicsProperty::SIZE, "size")
ADD_ENUM(ePhysicsProperty::RADIUS, "radius")
ADD_ENUM(ePhysicsProperty::HEIGHT, "height")
ADD_ENUM(ePhysicsProperty::BOUNDING_BOX, "boundingbox")
ADD_ENUM(ePhysicsProperty::BOUNDING_SPHERE, "boundingsphere")
ADD_ENUM(ePhysicsProperty::SWEPT_SPHERE_RADIUS, "sweptsphereradius")
ADD_ENUM(ePhysicsProperty::SLEEP, "sleep")
ADD_ENUM(ePhysicsProperty::WANTS_SLEEPING, "wantssleeping")
ADD_ENUM(ePhysicsProperty::MOTION_CCD_THRESHOLD, "motionthreshold")
ADD_ENUM(ePhysicsProperty::GRAVITY, "gravity")
ADD_ENUM(ePhysicsProperty::STIFFNESS, "stiffness")
ADD_ENUM(ePhysicsProperty::PIVOT_A, "pivota")
ADD_ENUM(ePhysicsProperty::PIVOT_B, "pivotb")
ADD_ENUM(ePhysicsProperty::LOWER_LIN_LIMIT, "lowerlinlimit")
ADD_ENUM(ePhysicsProperty::UPPER_LIN_LIMIT, "upperlinlimit")
ADD_ENUM(ePhysicsProperty::LOWER_ANG_LIMIT, "loweranglimit")
ADD_ENUM(ePhysicsProperty::UPPER_ANG_LIMIT, "upperanglimit")
ADD_ENUM(ePhysicsProperty::BREAKING_IMPULSE_THRESHOLD, "breakingimpulsethreshold")
ADD_ENUM(ePhysicsProperty::APPLIED_IMPULSE, "appliedimpulse")
ADD_ENUM(ePhysicsProperty::JOINTS_FEEDBACK, "jointfeedback")
ADD_ENUM(ePhysicsProperty::ENABLED, "enabled")
ADD_ENUM(ePhysicsProperty::RIGID_BODY_A, "rigidbodya")
ADD_ENUM(ePhysicsProperty::RIGID_BODY_B, "rigidbodyb")
IMPLEMENT_ENUM_CLASS_END("physics-element-property")

IMPLEMENT_ENUM_CLASS_BEGIN(ePhysicsWorldProperty)
ADD_ENUM(ePhysicsWorldProperty::SUBSTEPS, "simulationsubsteps")
ADD_ENUM(ePhysicsWorldProperty::SIMULATION_ENABLED, "simulationenabled")
ADD_ENUM(ePhysicsWorldProperty::GRAVITY, "gravity")
ADD_ENUM(ePhysicsWorldProperty::USE_CONTINOUS, "usecontinuous")
ADD_ENUM(ePhysicsWorldProperty::TRIGGERCOLLISIONEVENTS, "triggercollisionevents")
ADD_ENUM(ePhysicsWorldProperty::TRIGGERCONSTRAINTEVENTS, "triggerconstraintevents")
IMPLEMENT_ENUM_CLASS_END("physics-world-property")

IMPLEMENT_ENUM_CLASS_BEGIN(ePhysicsDebugMode)
ADD_ENUM(ePhysicsDebugMode::NoDebug, "nodebug")
ADD_ENUM(ePhysicsDebugMode::DrawWireframe, "drawwireframe")
ADD_ENUM(ePhysicsDebugMode::DrawAabb, "drawaabb")
ADD_ENUM(ePhysicsDebugMode::DrawFeaturesText, "drawfeaturestext")
ADD_ENUM(ePhysicsDebugMode::DrawContactPoints, "drawcontactpoints")
ADD_ENUM(ePhysicsDebugMode::NoDeactivation, "nodeactivation")
ADD_ENUM(ePhysicsDebugMode::NoHelpText, "nohelptext")
ADD_ENUM(ePhysicsDebugMode::DrawText, "drawtext")
ADD_ENUM(ePhysicsDebugMode::ProfileTimings, "profiletimings")
ADD_ENUM(ePhysicsDebugMode::EnableSatComparison, "enablesatcomparison")
ADD_ENUM(ePhysicsDebugMode::DisableBulletLCP, "disablebulletlcp")
ADD_ENUM(ePhysicsDebugMode::EnableCCD, "enableccd")
ADD_ENUM(ePhysicsDebugMode::DrawConstraints, "drawconstraints")
ADD_ENUM(ePhysicsDebugMode::DrawConstraintLimits, "drawconstraintlimits")
ADD_ENUM(ePhysicsDebugMode::FastWireframe, "fastwireframe")
ADD_ENUM(ePhysicsDebugMode::DrawNormals, "drawnormals")
ADD_ENUM(ePhysicsDebugMode::DrawFrames, "drawframes")
ADD_ENUM(ePhysicsDebugMode::LINE_WIDTH, "linewidth")
ADD_ENUM(ePhysicsDebugMode::DRAW_DISTANCE, "drawdistance")
IMPLEMENT_ENUM_CLASS_END("physics-debug-mode")

IMPLEMENT_ENUM_CLASS_BEGIN(ePhysicsElementType)
ADD_ENUM(ePhysicsElementType::Unknown, "physics-unknown")
ADD_ENUM(ePhysicsElementType::WorldElement, "physics-world-element")
ADD_ENUM(ePhysicsElementType::RigidBody, "physics-rigidbody")
ADD_ENUM(ePhysicsElementType::StaticCollision, "physics-rigidbody")
ADD_ENUM(ePhysicsElementType::Shape, "physics-shape")
ADD_ENUM(ePhysicsElementType::ConvexShape, "physics-convex-shape")
ADD_ENUM(ePhysicsElementType::ConcaveShape, "physics-concave-shape")
ADD_ENUM(ePhysicsElementType::BoxShape, "physics-box-shape")
ADD_ENUM(ePhysicsElementType::SphereShape, "physics-sphere-shape")
ADD_ENUM(ePhysicsElementType::ConeShape, "physics-cone-shape")
ADD_ENUM(ePhysicsElementType::BvhTriangleMeshShape, "physics-triangle-mesh-shape")
ADD_ENUM(ePhysicsElementType::CompoundShape, "physics-compound-shape")
ADD_ENUM(ePhysicsElementType::ConvexHullShape, "physics-convex-hull-shape")
ADD_ENUM(ePhysicsElementType::CylinderShape, "physics-dylinder-shape")
ADD_ENUM(ePhysicsElementType::GimpactTriangleMeshShape, "physics-gimpact-triangle-mesh-shape")
ADD_ENUM(ePhysicsElementType::HeightfieldTerrainShape, "physics-heightfield-terrain-shape")
ADD_ENUM(ePhysicsElementType::Constraint, "physics-constraint")
ADD_ENUM(ePhysicsElementType::PointToPointConstraint, "physics-pointtopoint-constraint")
IMPLEMENT_ENUM_CLASS_END("physics-element-type")

SString GetSharedUserDataClassName(void* ptr, lua_State* luaVM)
{
    if (auto* pVar = UserDataCast<CLuaPhysicsRigidBody>((CLuaPhysicsRigidBody*)nullptr, ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast<CLuaPhysicsStaticCollision>((CLuaPhysicsStaticCollision*)nullptr, ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast<CLuaPhysicsConstraint>((CLuaPhysicsConstraint*)nullptr, ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast<CLuaPhysicsShape>((CLuaPhysicsShape*)nullptr, ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast<CLuaPhysicsElement>((CLuaPhysicsElement*)nullptr, ptr, luaVM))
        return GetClassTypeName(pVar);

    return "";
}
