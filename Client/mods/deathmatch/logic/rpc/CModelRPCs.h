/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/rpc/CModelRPCs.h
 *  PURPOSE:     Server model RPC handlers
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"
#include <net/bitstream.h>

class CModelRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    static void AllocateServerModel(NetBitStreamInterface& bitStream);
    static void FreeServerModel(NetBitStreamInterface& bitStream);
};
