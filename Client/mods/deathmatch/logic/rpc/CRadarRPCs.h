/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CRadarRPCs.h
 *  PURPOSE:     Header for radar rpc class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CRadarRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(DestroyAllRadarAreas);
    DECLARE_ELEMENT_RPC(SetRadarAreaSize);
    DECLARE_ELEMENT_RPC(SetRadarAreaColor);
    DECLARE_ELEMENT_RPC(SetRadarAreaFlashing);
};
