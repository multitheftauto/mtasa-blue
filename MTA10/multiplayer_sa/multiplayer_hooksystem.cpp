/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/multiplayer_hooksystem.cpp
*  PURPOSE:     Multiplayer module hook system methods
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define MAX_JUMPCODE_SIZE 50

VOID HookInstallMethod( DWORD dwInstallAddress,
                        DWORD dwHookFunction )
{
    *(PDWORD)dwInstallAddress = (DWORD)dwHookFunction;
}

VOID HookInstallCall ( DWORD dwInstallAddress,
                        DWORD dwHookFunction )
{
    DWORD dwOffset = dwHookFunction - (dwInstallAddress + 5);
    *(BYTE*)(dwInstallAddress) = 0xE8;
    *(DWORD*)(dwInstallAddress+1) = dwOffset;
}

////////////////////////////////////////////////////////////////////

BOOL HookInstall( DWORD dwInstallAddress,
                  DWORD dwHookHandler,
                  int iJmpCodeSize )
{
    BYTE JumpBytes[MAX_JUMPCODE_SIZE];
    memset ( JumpBytes, 0x90, MAX_JUMPCODE_SIZE );
    if ( CreateJump ( dwInstallAddress, dwHookHandler, JumpBytes ) )
    {
        memcpy ( (PVOID)dwInstallAddress, JumpBytes, iJmpCodeSize );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

////////////////////////////////////////////////////////////////////

BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray )
{
    ByteArray[0] = 0xE9;
    *(DWORD *)(&ByteArray[1]) = dwTo - (dwFrom + 5);
    return ByteArray;
}

