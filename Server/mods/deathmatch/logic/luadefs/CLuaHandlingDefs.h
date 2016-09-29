/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaHandlingDefs.h
*  PURPOSE:     Lua vehicle handling definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaHandlingDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                           ( void );

    // Set
    LUA_DECLARE ( SetVehicleHandling );
    LUA_DECLARE ( SetModelHandling );

    // Get
    LUA_DECLARE ( GetVehicleHandling );
    LUA_DECLARE ( GetModelHandling );
    LUA_DECLARE ( GetOriginalHandling );

/*
    // Create/action
    LUA_DECLARE ( createHandling );
    LUA_DECLARE ( addDefaultHandling );

    // Get
    LUA_DECLARE ( handlingGetMass );
    LUA_DECLARE ( handlingGetTurnMass );
    LUA_DECLARE ( handlingGetDragCoefficiency );
    LUA_DECLARE ( handlingGetCenterOfMass );

    LUA_DECLARE ( handlingGetPercentSubmerged );
    LUA_DECLARE ( handlingGetTractionMultiplier );

    LUA_DECLARE ( handlingGetDriveType );
    LUA_DECLARE ( handlingGetEngineType );
    LUA_DECLARE ( handlingGetNumberOfGears );

    LUA_DECLARE ( handlingGetEngineAcceleration );
    LUA_DECLARE ( handlingGetEngineInertia );
    LUA_DECLARE ( handlingGetMaxVelocity );

    LUA_DECLARE ( handlingGetBrakeDeceleration );
    LUA_DECLARE ( handlingGetBrakeBias );
    LUA_DECLARE ( handlingGetABS );

    LUA_DECLARE ( handlingGetSteeringLock );
    LUA_DECLARE ( handlingGetTractionLoss );
    LUA_DECLARE ( handlingGetTractionBias );

    LUA_DECLARE ( handlingGetSuspensionForceLevel );
    LUA_DECLARE ( handlingGetSuspensionDamping );
    LUA_DECLARE ( handlingGetSuspensionHighSpeedDamping );
    LUA_DECLARE ( handlingGetSuspensionUpperLimit );
    LUA_DECLARE ( handlingGetSuspensionLowerLimit );
    LUA_DECLARE ( handlingGetSuspensionFrontRearBias );
    LUA_DECLARE ( handlingGetSuspensionAntiDiveMultiplier );

    LUA_DECLARE ( handlingGetCollisionDamageMultiplier );

    LUA_DECLARE ( handlingGetFlags );
    LUA_DECLARE ( handlingGetModelFlags );
    LUA_DECLARE ( handlingGetSeatOffsetDistance );

    LUA_DECLARE ( handlingGetHeadLightType );
    LUA_DECLARE ( handlingGetTailLightType );
    LUA_DECLARE ( handlingGetAnimGroup );

    // Set
    LUA_DECLARE ( handlingSetMass );
    LUA_DECLARE ( handlingSetTurnMass );
    LUA_DECLARE ( handlingSetDragCoefficiency );
    LUA_DECLARE ( handlingSetCenterOfMass );

    LUA_DECLARE ( handlingSetPercentSubmerged );
    LUA_DECLARE ( handlingSetTractionMultiplier );

    LUA_DECLARE ( handlingSetDriveType );
    LUA_DECLARE ( handlingSetEngineType );
    LUA_DECLARE ( handlingSetNumberOfGears );

    LUA_DECLARE ( handlingSetEngineAcceleration );
    LUA_DECLARE ( handlingSetEngineInertia );
    LUA_DECLARE ( handlingSetMaxVelocity );

    LUA_DECLARE ( handlingSetBrakeDeceleration );
    LUA_DECLARE ( handlingSetBrakeBias );
    LUA_DECLARE ( handlingSetABS );

    LUA_DECLARE ( handlingSetSteeringLock );
    LUA_DECLARE ( handlingSetTractionLoss );
    LUA_DECLARE ( handlingSetTractionBias );

    LUA_DECLARE ( handlingSetSuspensionForceLevel );
    LUA_DECLARE ( handlingSetSuspensionDamping );
    LUA_DECLARE ( handlingSetSuspensionHighSpeedDamping );
    LUA_DECLARE ( handlingSetSuspensionUpperLimit );
    LUA_DECLARE ( handlingSetSuspensionLowerLimit );
    LUA_DECLARE ( handlingSetSuspensionFrontRearBias );
    LUA_DECLARE ( handlingSetSuspensionAntiDiveMultiplier );

    LUA_DECLARE ( handlingSetCollisionDamageMultiplier );

    LUA_DECLARE ( handlingSetFlags );
    LUA_DECLARE ( handlingSetModelFlags );
    LUA_DECLARE ( handlingSetSeatOffsetDistance );

    LUA_DECLARE ( handlingSetHeadLightType );
    LUA_DECLARE ( handlingSetTailLightType );
    LUA_DECLARE ( handlingSetAnimGroup );*/
};