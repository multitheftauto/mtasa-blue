/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaHandlingDefs.h
*  PURPOSE:     Lua vehicle handling definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAHANDLINGDEFS_H
#define __CLUAHANDLINGDEFS_H

#include "CLuaDefs.h"

class CLuaHandlingDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                           ( void );

    // Create/action
    static int      createHandling                          ( lua_State* luaVM );
    static int      addDefaultHandling                      ( lua_State* luaVM );

    // Get
    static int      handlingGetMass                         ( lua_State* luaVM );
    static int      handlingGetTurnMass                     ( lua_State* luaVM );
    static int      handlingGetDragCoefficiency             ( lua_State* luaVM );
    static int      handlingGetCenterOfMass                 ( lua_State* luaVM );

    static int      handlingGetPercentSubmerged             ( lua_State* luaVM );
    static int      handlingGetTractionMultiplier           ( lua_State* luaVM );

    static int      handlingGetDriveType                    ( lua_State* luaVM );
    static int      handlingGetEngineType                   ( lua_State* luaVM );
    static int      handlingGetNumberOfGears                ( lua_State* luaVM );

    static int      handlingGetEngineAccelleration          ( lua_State* luaVM );
    static int      handlingGetEngineInertia                ( lua_State* luaVM );
    static int      handlingGetMaxVelocity                  ( lua_State* luaVM );

    static int      handlingGetBrakeDecelleration           ( lua_State* luaVM );
    static int      handlingGetBrakeBias                    ( lua_State* luaVM );
    static int      handlingGetABS                          ( lua_State* luaVM );

    static int      handlingGetSteeringLock                 ( lua_State* luaVM );
    static int      handlingGetTractionLoss                 ( lua_State* luaVM );
    static int      handlingGetTractionBias                 ( lua_State* luaVM );

    static int      handlingGetSuspensionForceLevel         ( lua_State* luaVM );
    static int      handlingGetSuspensionDamping            ( lua_State* luaVM );
    static int      handlingGetSuspensionHighSpeedDamping   ( lua_State* luaVM );
    static int      handlingGetSuspensionUpperLimit         ( lua_State* luaVM );
    static int      handlingGetSuspensionLowerLimit         ( lua_State* luaVM );
    static int      handlingGetSuspensionFrontRearBias      ( lua_State* luaVM );
    static int      handlingGetSuspensionAntidiveMultiplier ( lua_State* luaVM );

    static int      handlingGetCollisionDamageMultiplier    ( lua_State* luaVM );

    static int      handlingGetFlags                        ( lua_State* luaVM );
    static int      handlingGetModelFlags                   ( lua_State* luaVM );
    static int      handlingGetSeatOffsetDistance           ( lua_State* luaVM );

    static int      handlingGetHeadLightType                ( lua_State* luaVM );
    static int      handlingGetTailLightType                ( lua_State* luaVM );
    static int      handlingGetAnimGroup                    ( lua_State* luaVM );

    // Set
    static int      handlingSetMass                         ( lua_State* luaVM );
    static int      handlingSetTurnMass                     ( lua_State* luaVM );
    static int      handlingSetDragCoefficiency             ( lua_State* luaVM );
    static int      handlingSetCenterOfMass                 ( lua_State* luaVM );

    static int      handlingSetPercentSubmerged             ( lua_State* luaVM );
    static int      handlingSetTractionMultiplier           ( lua_State* luaVM );

    static int      handlingSetDriveType                    ( lua_State* luaVM );
    static int      handlingSetEngineType                   ( lua_State* luaVM );
    static int      handlingSetNumberOfGears                ( lua_State* luaVM );

    static int      handlingSetEngineAccelleration          ( lua_State* luaVM );
    static int      handlingSetEngineInertia                ( lua_State* luaVM );
    static int      handlingSetMaxVelocity                  ( lua_State* luaVM );

    static int      handlingSetBrakeDecelleration           ( lua_State* luaVM );
    static int      handlingSetBrakeBias                    ( lua_State* luaVM );
    static int      handlingSetABS                          ( lua_State* luaVM );

    static int      handlingSetSteeringLock                 ( lua_State* luaVM );
    static int      handlingSetTractionLoss                 ( lua_State* luaVM );
    static int      handlingSetTractionBias                 ( lua_State* luaVM );

    static int      handlingSetSuspensionForceLevel         ( lua_State* luaVM );
    static int      handlingSetSuspensionDamping            ( lua_State* luaVM );
    static int      handlingSetSuspensionHighSpeedDamping   ( lua_State* luaVM );
    static int      handlingSetSuspensionUpperLimit         ( lua_State* luaVM );
    static int      handlingSetSuspensionLowerLimit         ( lua_State* luaVM );
    static int      handlingSetSuspensionFrontRearBias      ( lua_State* luaVM );
    static int      handlingSetSuspensionAntidiveMultiplier ( lua_State* luaVM );

    static int      handlingSetCollisionDamageMultiplier    ( lua_State* luaVM );

    static int      handlingSetFlags                        ( lua_State* luaVM );
    static int      handlingSetModelFlags                   ( lua_State* luaVM );
    static int      handlingSetSeatOffsetDistance           ( lua_State* luaVM );

    static int      handlingSetHeadLightType                ( lua_State* luaVM );
    static int      handlingSetTailLightType                ( lua_State* luaVM );
    static int      handlingSetAnimGroup                    ( lua_State* luaVM );
};

#endif
