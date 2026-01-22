/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/multiplayer_hooksystem.h
 *  PURPOSE:     Multiplayer module hook system methods
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "multiplayersa_init.h"

#pragma once

VOID  HookInstallMethod(DWORD dwInstallAddress, DWORD dwHookFunction);
VOID  HookInstallCall(DWORD dwInstallAddress, DWORD dwHookFunction);
BOOL  HookInstall(DWORD dwInstallAddress, DWORD dwHookHandler, int iJmpCodeSize);
BYTE* CreateJump(DWORD dwFrom, DWORD dwTo, BYTE* ByteArray);
VOID  HookCheckOriginalByte(DWORD dwInstallAddress, uchar ucExpectedValue);

#define EZHookInstall(type) \
    HookInstall( HOOKPOS_##type, (DWORD)HOOK_##type, HOOKSIZE_##type );

// Check original byte before hooking
#define EZHookInstallChecked(type) \
    HookCheckOriginalByte( HOOKPOS_##type, HOOKCHECK_##type ); \
    EZHookInstall( type );

#define EZHookInstallRestore(type) \
    __if_exists(RESTORE_Bytes_##type) \
    { \
        RESTORE_Addr_##type = HOOKPOS_##type; \
        RESTORE_Size_##type = HOOKSIZE_##type; \
        assert(sizeof(RESTORE_Bytes_##type) >= RESTORE_Size_##type); \
        MemCpyFast(RESTORE_Bytes_##type, (PVOID)RESTORE_Addr_##type, RESTORE_Size_##type); \
    } \
    EZHookInstall( type );

// Structure for holding hook info
struct SHookInfo
{
    template <class T>
    SHookInfo(DWORD dwAddress, T dwHook, uint uiSize) : dwAddress(dwAddress), dwHook((DWORD)dwHook), uiSize(uiSize)
    {
    }
    DWORD dwAddress;
    DWORD dwHook;
    uint  uiSize;
};

#define MAKE_HOOK_INFO(type)  SHookInfo ( HOOKPOS_##type, HOOK_##type, HOOKSIZE_##type )

// Structure for holding poke info
struct SPokeInfo
{
    DWORD dwAddress;
    BYTE  ucValue;
};
