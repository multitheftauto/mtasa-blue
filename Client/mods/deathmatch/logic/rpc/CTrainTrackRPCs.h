/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CTrainTrackRPCs.h
 *  PURPOSE:     Header for train track RPC class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CTrainTrackRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_ELEMENT_RPC(SetTrainTrackNodePosition);
};
