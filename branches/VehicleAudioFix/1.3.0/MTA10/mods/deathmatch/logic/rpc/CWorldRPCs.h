/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CWorldRPCs.h
*  PURPOSE:     Header for world RPC calls
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWorldRPCs_H
#define __CWorldRPCs_H

#include "CRPCFunctions.h"

class CWorldRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( SetTime );
    DECLARE_RPC ( SetWeather );
    DECLARE_RPC ( SetWeatherBlended );
    DECLARE_RPC ( SetMinuteDuration );
    DECLARE_RPC ( SetGravity );
    DECLARE_RPC ( SetGameSpeed );
    DECLARE_RPC ( SetWaveHeight );
    DECLARE_RPC ( GetSkyGradient );
    DECLARE_RPC ( SetSkyGradient );
    DECLARE_RPC ( ResetSkyGradient );
    DECLARE_RPC ( GetHeatHaze );
    DECLARE_RPC ( SetHeatHaze );
    DECLARE_RPC ( ResetHeatHaze );
    DECLARE_RPC ( SetBlurLevel );
    DECLARE_RPC ( SetWantedLevel );
    DECLARE_RPC ( ResetMapInfo );
    DECLARE_RPC ( SetFPSLimit );
    DECLARE_RPC ( SetGarageOpen );
    DECLARE_RPC ( SetGlitchEnabled );
    DECLARE_RPC ( SetJetpackWeaponEnabled );
    DECLARE_RPC ( SetCloudsEnabled );
    DECLARE_RPC ( SetTrafficLightState );
    DECLARE_RPC ( SetJetpackMaxHeight );
    DECLARE_RPC ( SetInteriorSoundsEnabled );
    DECLARE_RPC ( SetRainLevel );
    DECLARE_RPC ( SetSunSize );
    DECLARE_RPC ( SetSunColor );
    DECLARE_RPC ( SetWindVelocity );
    DECLARE_RPC ( SetFarClipDistance );
    DECLARE_RPC ( SetFogDistance );
    DECLARE_RPC ( SetAircraftMaxHeight );
    DECLARE_RPC ( SetOcclusionsEnabled );
    DECLARE_RPC ( ResetRainLevel );
    DECLARE_RPC ( ResetSunSize );
    DECLARE_RPC ( ResetSunColor );
    DECLARE_RPC ( ResetWindVelocity );
    DECLARE_RPC ( ResetFarClipDistance );
    DECLARE_RPC ( ResetFogDistance );
    DECLARE_RPC ( SetWeaponProperty );
    DECLARE_RPC ( RemoveWorldModel );
    DECLARE_RPC ( RestoreWorldModel );
    DECLARE_RPC ( RestoreAllWorldModels );
    DECLARE_RPC ( SetSyncIntervals );
};

#endif
