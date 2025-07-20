/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/COutputRPCs.cpp
 *  PURPOSE:     Output remote procedure calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "COutputRPCs.h"

void COutputRPCs::LoadFunctions()
{
    AddHandler(TOGGLE_DEBUGGER, ToggleDebugger, "ToggleDebugger");
    AddHandler(SET_TRANSFER_BOX_VISIBILITY, SetTransferBoxVisibility, "SetTransferBoxVisibility");
}

void COutputRPCs::ToggleDebugger(NetBitStreamInterface& bitStream)
{
    unsigned char ucEnable;
    bitStream.Read(ucEnable);

    g_pCore->SetDebugVisible((ucEnable == 1));
}

void COutputRPCs::SetTransferBoxVisibility(NetBitStreamInterface& bitStream)
{
    g_pClientGame->GetTransferBox()->SetServerVisibility(bitStream.ReadBit());
}
