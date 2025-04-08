/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CInputRPCs.h
 *  PURPOSE:     Header for input RPC class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CInputRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(BindKey);
    DECLARE_RPC(UnbindKey);
    DECLARE_RPC(BindCommand);
    DECLARE_RPC(UnbindCommand);
    DECLARE_RPC(ToggleControl);
    DECLARE_RPC(ToggleAllControls);
    DECLARE_RPC(SetControlState);
    DECLARE_RPC(ForceReconnect);
    DECLARE_RPC(ShowCursor);
    DECLARE_RPC(ShowChat);
};
