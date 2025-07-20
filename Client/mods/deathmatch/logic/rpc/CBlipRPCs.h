/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CBlipRPCs.h
 *  PURPOSE:     Header for blip RPC class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CBlipRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(DestroyAllBlips);
    DECLARE_ELEMENT_RPC(SetBlipIcon);
    DECLARE_ELEMENT_RPC(SetBlipSize);
    DECLARE_ELEMENT_RPC(SetBlipColor);
    DECLARE_ELEMENT_RPC(SetBlipOrdering);
    DECLARE_ELEMENT_RPC(SetBlipVisibleDistance);
};
