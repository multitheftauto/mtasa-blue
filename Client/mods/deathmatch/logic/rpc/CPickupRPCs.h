/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CPickupRPCs.h
 *  PURPOSE:     Header for pickups RPC class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CPickupRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(DestroyAllPickups);
    DECLARE_ELEMENT_RPC(SetPickupType);
    DECLARE_ELEMENT_RPC(SetPickupVisible);
};
