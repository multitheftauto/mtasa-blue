/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/HookSystem.cpp
 *  PURPOSE:     Function hook installation system
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HookSystem.h"

BYTE* CreateJump(DWORD dwFrom, DWORD dwTo, BYTE* ByteArray)
{
    ByteArray[0] = 0xE9;
    MemPutFast<DWORD>(&ByteArray[1], dwTo - (dwFrom + 5));
    return ByteArray;
}
