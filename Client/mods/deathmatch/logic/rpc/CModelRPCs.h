/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CModelRPCs.h
 *  PURPOSE:     Header for model RPC class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CModelRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(AllocateModelFromParent);
    DECLARE_RPC(UnloadModel);
};
