/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CPlayerRPCs.h
 *  PURPOSE:     Header for player RPC class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CPlayerRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(SetPlayerMoney);
    DECLARE_RPC(ShowPlayerHudComponent);
    DECLARE_RPC(ForcePlayerMap);
    DECLARE_ELEMENT_RPC(SetPlayerNametagText);
    DECLARE_ELEMENT_RPC(SetPlayerNametagColor);
    DECLARE_ELEMENT_RPC(RemovePlayerNametagColor);
    DECLARE_ELEMENT_RPC(SetPlayerNametagShowing);
    DECLARE_ELEMENT_RPC(SetPlayerTeam);
    DECLARE_RPC(TakePlayerScreenShot);
    DECLARE_RPC(SetPlayerScriptDebugLevel);
};
