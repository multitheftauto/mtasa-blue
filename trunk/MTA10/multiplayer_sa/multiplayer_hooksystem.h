/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/multiplayer_hooksystem.h
*  PURPOSE:     Multiplayer module hook system methods
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "multiplayersa_init.h"

#ifndef __MULTIPLAYER_HOOKSYSTEM_INIT
#define __MULTIPLAYER_HOOKSYSTEM_INIT

VOID HookInstallMethod( DWORD dwInstallAddress, DWORD dwHookFunction );
VOID HookInstallCall( DWORD dwInstallAddress, DWORD dwHookFunction );
BOOL HookInstall( DWORD dwInstallAddress, DWORD dwHookHandler, int iJmpCodeSize );
BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray );

#endif
