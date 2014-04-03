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

VOID HookInstallCall ( DWORD dwInstallAddress,
                        DWORD dwHookFunction )
{
    DWORD dwOffset = dwHookFunction - (dwInstallAddress + 5);
    MemPut < BYTE > ( dwInstallAddress, 0xE8 );
    MemPut < DWORD > ( dwInstallAddress+1, dwOffset );
}

//
////////////////////////////////////////////////////////////////////

inline DWORD GetProcedureOffset( DWORD dwFrom, DWORD dwTo )
{
    return dwTo - (dwFrom + 5);
}

BOOL HookInstall( DWORD dwInstallAddress,
                  DWORD dwHookHandler,
                  int iJmpCodeSize )
{
    BYTE JumpBytes[MAX_JUMPCODE_SIZE];
    MemSetFast ( JumpBytes, 0x90, MAX_JUMPCODE_SIZE );
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

BOOL PatchCall( DWORD dwInstallAddress, DWORD dwHookHandler )
{
    MemPut <DWORD> ( dwInstallAddress + 1, GetProcedureOffset( dwInstallAddress, dwHookHandler ) );
    return TRUE;
}

////////////////////////////////////////////////////////////////////

BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray )
{
    ByteArray[0] = 0xE9;
    MemPutFast < DWORD > ( &ByteArray[1], dwTo - (dwFrom + 5) );
    return ByteArray;
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

VOID HookCheckOriginalByte( DWORD dwInstallAddress, uchar ucExpectedValue )
{
    uchar ucValue = *(uchar*)dwInstallAddress;
    dassert( ucValue == ucExpectedValue );
    if ( ucValue != ucExpectedValue )
        AddReportLog( 8423, SString( "HookCheckOriginalByte failed at %08x - Got %02x - expected %02x", dwInstallAddress, ucValue, ucExpectedValue ) );
}
