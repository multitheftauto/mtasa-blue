/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CRadarRPCs.h
*  PURPOSE:     Header for radar rpc class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRadarRPCs_H
#define __CRadarRPCs_H

#include "CRPCFunctions.h"

class CRadarRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( DestroyAllRadarAreas );
    DECLARE_RPC ( SetRadarAreaSize );
    DECLARE_RPC ( SetRadarAreaColor );
    DECLARE_RPC ( SetRadarAreaFlashing );
};

#endif