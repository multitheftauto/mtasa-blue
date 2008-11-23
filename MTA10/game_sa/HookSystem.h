/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/HookSystem.h
*  PURPOSE:		Function hook installation system
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __HOOKSYSTEM_H
#define __HOOKSYSTEM_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define		MAX_JUMPCODE_SIZE			20

BYTE * CreateJump ( DWORD dwJumpAddress, BYTE * ByteArray );
BOOL HookInstall( DWORD dwInstallAddress,
				  DWORD dwHookHandler,
				  DWORD * dwHookStorage,
				  int iJmpCodeSize );

#endif
