/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/COutputRPCs.cpp
*  PURPOSE:     Output remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "COutputRPCs.h"

void COutputRPCs::LoadFunctions ( void )
{
    AddHandler ( TOGGLE_DEBUGGER, ToggleDebugger, "ToggleDebugger" );
}


void COutputRPCs::ToggleDebugger ( NetBitStreamInterface& bitStream )
{
    unsigned char ucEnable;
    bitStream.Read ( ucEnable );

    g_pCore->SetDebugVisible ( ( ucEnable == 1 ) );
}