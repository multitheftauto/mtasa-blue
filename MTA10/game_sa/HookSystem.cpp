/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/HookSystem.cpp
*  PURPOSE:     Function hook installation system
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

//
////////////////////////////////////////////////////////////////////

BOOL HookInstall( DWORD dwInstallAddress,
                  DWORD dwHookHandler,
                  int iJmpCodeSize )
{
    BYTE JumpBytes[MAX_JUMPCODE_SIZE];
    MemSet ( JumpBytes, 0x90, MAX_JUMPCODE_SIZE );
    if ( CreateJump ( dwInstallAddress, dwHookHandler, JumpBytes ) )
    {
        MemCpy ( (PVOID)dwInstallAddress, JumpBytes, iJmpCodeSize );
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
    MemPut < DWORD > ( &ByteArray[1], dwTo - (dwFrom + 5) );  //     *(DWORD *)(&ByteArray[1]) = dwTo - (dwFrom + 5);
    return ByteArray;
}
