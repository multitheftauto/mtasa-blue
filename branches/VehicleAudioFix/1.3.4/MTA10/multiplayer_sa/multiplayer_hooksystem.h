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

// Auto detect requirement of US/EU hook installation
#define EZHookInstall(type) \
        __if_not_exists( RETURN_##type##_US ) \
        { \
            HookInstall( HOOKPOS_##type, (DWORD)HOOK_##type, HOOKSIZE_##type ) \
        } \
        __if_exists( RETURN_##type##_US ) \
        { \
            if ( pGameInterface->GetGameVersion () == VERSION_US_10 ) \
            { \
                EZHookInstall_HERE( type, US ) \
            } \
            else \
            { \
                EZHookInstall_HERE( type, EU ) \
            } \
        }


// US/EU hook installation
// Includes additional return pointer copies if required
#define EZHookInstall_HERE(type,CO) \
        __if_exists( RESTORE_Bytes_##type ) \
        { \
            RESTORE_Addr_##type = HOOKPOS_##type##_##CO##; \
            RESTORE_Size_##type = HOOKSIZE_##type##_##CO##; \
            assert( sizeof(RESTORE_Bytes_##type) >= RESTORE_Size_##type ); \
            MemCpyFast ( RESTORE_Bytes_##type, (PVOID)RESTORE_Addr_##type, RESTORE_Size_##type ); \
        } \
        HookInstall( HOOKPOS_##type##_##CO##, (DWORD)HOOK_##type, HOOKSIZE_##type##_##CO## ); \
        RETURN_##type##_BOTH = RETURN_##type##_##CO##; \
        __if_exists( RETURN_##type##B_##CO## ) \
        { \
            RETURN_##type##B_BOTH = RETURN_##type##B_##CO##; \
        } \
        __if_exists( RETURN_##type##C_##CO## ) \
        { \
            RETURN_##type##C_BOTH = RETURN_##type##C_##CO##; \
        }


// Structure for holding hook info
struct SHookInfo
{
    template < class T > SHookInfo ( DWORD dwAddress, T dwHook, uint uiSize ): dwAddress ( dwAddress ) , dwHook ( (DWORD)dwHook ) , uiSize ( uiSize ) {}
    DWORD dwAddress;
    DWORD dwHook;
    uint uiSize;
};

#define MAKE_HOOK_INFO(type)  SHookInfo ( HOOKPOS_##type, HOOK_##type, HOOKSIZE_##type )


// Structure for holding poke info
struct SPokeInfo
{
    DWORD dwAddress;
    BYTE ucValue;
};

#endif
