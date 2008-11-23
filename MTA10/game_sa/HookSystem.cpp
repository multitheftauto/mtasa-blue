/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/HookSystem.cpp
*  PURPOSE:		Function hook installation system
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

//
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
