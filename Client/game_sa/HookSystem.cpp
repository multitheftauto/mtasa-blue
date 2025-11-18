/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/HookSystem.cpp
 *  PURPOSE:     Function hook installation system
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

void HookInstallCall(DWORD dwInstallAddress, DWORD dwHookFunction)
{
    DWORD dwOffset = dwHookFunction - (dwInstallAddress + 5);
    MemPut<BYTE>(dwInstallAddress, 0xE8);
    MemPut<DWORD>(dwInstallAddress + 1, dwOffset);
}

void HookInstallVTBLCall(void* vtblMethodAddress, std::uintptr_t hookFunction)
{
    // We need to change the protection of the memory page to be able to write to it as it's in the .rdata section
    DWORD op;
    VirtualProtect(vtblMethodAddress, 4, PAGE_EXECUTE_READWRITE, &op);
    *static_cast<std::uintptr_t*>(vtblMethodAddress) = hookFunction;
    VirtualProtect(vtblMethodAddress, 4, op, &op);
}
