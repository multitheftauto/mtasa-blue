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
    DECLARE_RPC ( SetSkyGradient );
    DECLARE_RPC ( ResetSkyGradient );
    DECLARE_RPC ( SetBlurLevel );
    DECLARE_RPC ( SetWantedLevel );
    DECLARE_RPC ( ResetMapInfo );
    DECLARE_RPC ( SetFPSLimit );
    DECLARE_RPC ( SetGarageOpen );
};

#endif