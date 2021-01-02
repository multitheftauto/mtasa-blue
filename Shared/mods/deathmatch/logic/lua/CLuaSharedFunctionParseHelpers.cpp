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
ADD_ENUM(ePhysicsProperty::SLEEPING_THRESHOLDS, "sleepingthresholds")
ADD_ENUM(ePhysicsProperty::SUBSTEPS, "simulationsubsteps")
ADD_ENUM(ePhysicsProperty::SIMULATION_ENABLED, "simulationenabled")
ADD_ENUM(ePhysicsProperty::RESTITUTION, "restitution")
ADD_ENUM(ePhysicsProperty::SCALE, "scale")
ADD_ENUM(ePhysicsProperty::DEBUG_COLOR, "debugcolor")
ADD_ENUM(ePhysicsProperty::FILTER_MASK, "filtermask")
ADD_ENUM(ePhysicsProperty::FILTER_GROUP, "filtergroup")
ADD_ENUM(ePhysicsProperty::STIFFNESS, "stiffness")
ADD_ENUM(ePhysicsProperty::SIZE, "size")
ADD_ENUM(ePhysicsProperty::RADIUS, "radius")
ADD_ENUM(ePhysicsProperty::HEIGHT, "height")
ADD_ENUM(ePhysicsProperty::BOUNDING_BOX, "boundingbox")
ADD_ENUM(ePhysicsProperty::BOUNDING_SPHERE, "boundingsphere")
ADD_ENUM(ePhysicsProperty::GRAVITY, "gravity")
ADD_ENUM(ePhysicsProperty::USE_CONTINOUS, "usecontinuous")
ADD_ENUM(ePhysicsProperty::MOTION_CCD_THRESHOLD, "motionthreshold")
ADD_ENUM(ePhysicsProperty::SWEPT_SPHERE_RADIUS, "sweptsphereradius")
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
ADD_ENUM(ePhysicsProperty::TRIGGERCOLLISIONEVENTS, "triggercollisionevents")
ADD_ENUM(ePhysicsProperty::TRIGGERCONSTRAINTEVENTS, "triggerconstraintevents")
ADD_ENUM(ePhysicsProperty::RIGID_BODY_A, "rigidbodya")
ADD_ENUM(ePhysicsProperty::RIGID_BODY_B, "rigidbodyb")
ADD_ENUM(ePhysicsProperty::SLEEP, "sleep")
ADD_ENUM(ePhysicsProperty::WANTS_SLEEPING, "wantssleeping")
ADD_ENUM(ePhysicsProperty::IS_COMPOUND, "iscompound")
ADD_ENUM(ePhysicsProperty::IS_CONCAVE, "iscocave")
ADD_ENUM(ePhysicsProperty::IS_CONVEX, "isconvex")
ADD_ENUM(ePhysicsProperty::IS_CONVEX2D, "isconvex2d")
ADD_ENUM(ePhysicsProperty::IS_INFINITE, "isinfinite")
ADD_ENUM(ePhysicsProperty::IS_NON_MOVING, "isnonmoving")
ADD_ENUM(ePhysicsProperty::IS_POLYHEDRAL, "ispolyhedral")
ADD_ENUM(ePhysicsProperty::IS_SOFT_BODY, "issoftbody")
IMPLEMENT_ENUM_CLASS_END("physics-shape-property")

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

SString GetSharedUserDataClassName(void* ptr, lua_State* luaVM)
{
    if (auto* pVar = UserDataCast<CLuaPhysicsRigidBody>((CLuaPhysicsRigidBody*)NULL, ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast<CLuaPhysicsStaticCollision>((CLuaPhysicsStaticCollision*)NULL, ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto* pVar = UserDataCast<CLuaPhysicsConstraint>((CLuaPhysicsConstraint*)NULL, ptr, luaVM))
        return GetClassTypeName(pVar);
    if (auto pVar = UserDataCast<CLuaPhysicsShape>((CLuaPhysicsShape*)NULL, ptr, luaVM))
        return GetClassTypeName(pVar.get());
    if (auto* pVar = UserDataCast<CLuaPhysicsElement>((CLuaPhysicsElement*)NULL, ptr, luaVM))
        return GetClassTypeName(pVar);

    return "";
}
