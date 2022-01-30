/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CMarkerRPCs.h
 *  PURPOSE:     Header for marker RPC class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CModelRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(AllocateModelFromParent);
};
