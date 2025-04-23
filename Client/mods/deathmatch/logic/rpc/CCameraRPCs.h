/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CCameraRPCs.h
 *  PURPOSE:     Header for camera RPC class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CCameraRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(SetCameraMatrix);
    DECLARE_RPC(SetCameraTarget);
    DECLARE_RPC(SetCameraInterior);
    DECLARE_RPC(FadeCamera);
};
