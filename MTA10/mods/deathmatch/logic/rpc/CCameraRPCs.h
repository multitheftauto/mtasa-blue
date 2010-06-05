/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CCameraRPCs.h
*  PURPOSE:     Header for camera RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCameraRPCs_H
#define __CCameraRPCs_H

#include "CRPCFunctions.h"

class CCameraRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( SetCameraMatrix );
    DECLARE_RPC ( SetCameraTarget );
    DECLARE_RPC ( SetCameraInterior );
    DECLARE_RPC ( FadeCamera );
};

#endif