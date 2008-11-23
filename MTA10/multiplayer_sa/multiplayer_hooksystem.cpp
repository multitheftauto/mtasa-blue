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

VOID HookInstallMethod(	DWORD dwInstallAddress,
						DWORD dwHookFunction )
{
	DWORD oldProt, oldProt2;

	VirtualProtect((LPVOID)dwInstallAddress,4,PAGE_EXECUTE_READWRITE,&oldProt);
	*(PDWORD)dwInstallAddress = (DWORD)dwHookFunction;
	VirtualProtect((LPVOID)dwInstallAddress,4,oldProt,&oldProt2);
}

VOID HookInstallCall ( DWORD dwInstallAddress,
						DWORD dwHookFunction )
{
    DWORD oldProt, oldProt2;
    DWORD dwOffset = dwHookFunction - (dwInstallAddress + 5);
	VirtualProtect((LPVOID)dwInstallAddress,5,PAGE_EXECUTE_READWRITE,&oldProt);
	*(BYTE*)(dwInstallAddress) = 0xE8;
    *(DWORD*)(dwInstallAddress+1) = dwOffset;
	VirtualProtect((LPVOID)dwInstallAddress,5,oldProt,&oldProt2);
}

////////////////////////////////////////////////////////////////////

BOOL HookInstall( DWORD dwInstallAddress,
				  DWORD dwHookHandler,
				  DWORD * dwHookStorage,
				  int iJmpCodeSize )
{
	BYTE	JumpBytes[MAX_JUMPCODE_SIZE];
	memset(JumpBytes,0x90,MAX_JUMPCODE_SIZE);
	if (CreateJump((DWORD)dwHookStorage, JumpBytes))
	{	
		*dwHookStorage = dwHookHandler;

		DWORD oldProt, oldProt2;

		VirtualProtect((LPVOID)dwInstallAddress,iJmpCodeSize,PAGE_EXECUTE_READWRITE,&oldProt);		
		memcpy((PVOID)dwInstallAddress,JumpBytes,iJmpCodeSize);
		VirtualProtect((LPVOID)dwInstallAddress,iJmpCodeSize,oldProt,&oldProt2);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////

BYTE * CreateJump ( DWORD dwJumpAddress, BYTE * ByteArray )
{
	ByteArray[0] = 0xFF;
	ByteArray[1] = 0x25;
	ByteArray[2] = *((BYTE *)(&dwJumpAddress) );
	ByteArray[3] = *((BYTE *)(&dwJumpAddress) + 1);
	ByteArray[4] = *((BYTE *)(&dwJumpAddress) + 2);
	ByteArray[5] = *((BYTE *)(&dwJumpAddress) + 3);
	return ByteArray;
}

