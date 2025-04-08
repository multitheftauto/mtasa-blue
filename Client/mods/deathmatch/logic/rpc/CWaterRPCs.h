/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CWaterRPCs.h
 *  PURPOSE:     Header for water RPC calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CWaterRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(SetWorldWaterLevel);
    DECLARE_RPC(ResetWorldWaterLevel);
    DECLARE_RPC(SetAllElementWaterLevel);
    DECLARE_ELEMENT_RPC(SetElementWaterLevel);
    DECLARE_ELEMENT_RPC(SetWaterVertexPosition);
    DECLARE_RPC(SetWaterColor);
    DECLARE_RPC(ResetWaterColor);
};
