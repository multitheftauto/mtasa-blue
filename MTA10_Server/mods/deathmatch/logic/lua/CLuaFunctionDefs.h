/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.h
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaFunctionDefs;
class CBlipManager;
class CMarkerManager;
class CPickupManager;
class CRadarAreaManager;
class CRegisteredCommands;
class CAccountManager;
class CColManager;
class CAccessControlListManager;

#pragma once
#include "LuaCommon.h"
#include "CLuaMain.h"
#include "CLuaTimerManager.h"

#define LUA_ERROR() lua_pushboolean ( luaVM, false ); return 0;
#define LUA_DECLARE(x) static int x ( lua_State * luaVM ); 

extern CTimeUsMarker < 20 > markerLatentEvent;  // For timing triggerLatentClientEvent

class CLuaFunctionDefs
{
public:
    static void     Initialize ( class CLuaManager* pLuaManager, class CGame* pClientGame );

    LUA_DECLARE ( DisabledFunction );
    LUA_DECLARE ( CallRemote );
    LUA_DECLARE ( FetchRemote );

    // Event functions
    LUA_DECLARE ( AddEvent );
    LUA_DECLARE ( AddEventHandler );
    LUA_DECLARE ( RemoveEventHandler );
    LUA_DECLARE ( GetEventHandlers );
    LUA_DECLARE ( TriggerEvent );
    LUA_DECLARE ( TriggerClientEvent );
    LUA_DECLARE ( CancelEvent );
    LUA_DECLARE ( GetCancelReason );
    LUA_DECLARE ( WasEventCancelled );
    LUA_DECLARE ( TriggerLatentClientEvent );
    LUA_DECLARE ( GetLatentEventHandles );
    LUA_DECLARE ( GetLatentEventStatus );
    LUA_DECLARE ( CancelLatentEvent );
    LUA_DECLARE ( AddDebugHook );
    LUA_DECLARE ( RemoveDebugHook );

    // Explosion funcs
    LUA_DECLARE ( CreateExplosion );

    // Fire funcs
    LUA_DECLARE ( CreateFire );

    // Ped body funcs?
    LUA_DECLARE ( GetBodyPartName );
    LUA_DECLARE ( GetClothesByTypeIndex );
    LUA_DECLARE ( GetTypeIndexFromClothes );
    LUA_DECLARE ( GetClothesTypeName );

    // Weapon funcs
    LUA_DECLARE ( GetWeaponProperty );
    LUA_DECLARE ( GetOriginalWeaponProperty );
    LUA_DECLARE ( SetWeaponProperty );
    LUA_DECLARE ( GetSlotFromWeapon );

    LUA_DECLARE ( CreateWeapon );
    LUA_DECLARE ( GetWeaponNameFromID );
    LUA_DECLARE ( GetWeaponIDFromName );
    LUA_DECLARE ( FireWeapon );
    LUA_DECLARE ( SetWeaponState );
    LUA_DECLARE ( GetWeaponState );
    LUA_DECLARE ( SetWeaponTarget );
    LUA_DECLARE ( GetWeaponTarget );
    LUA_DECLARE ( SetWeaponOwner );
    LUA_DECLARE ( GetWeaponOwner );
    LUA_DECLARE ( SetWeaponFlags );
    LUA_DECLARE ( GetWeaponFlags );
    LUA_DECLARE ( SetWeaponFiringRate );
    LUA_DECLARE ( GetWeaponFiringRate );
    LUA_DECLARE ( ResetWeaponFiringRate );
    LUA_DECLARE ( GetWeaponAmmo );
    LUA_DECLARE ( SetWeaponAmmo );
    LUA_DECLARE ( GetWeaponClipAmmo );
    LUA_DECLARE ( SetWeaponClipAmmo );

    // Console functions
    LUA_DECLARE ( AddCommandHandler );
    LUA_DECLARE ( RemoveCommandHandler );
    LUA_DECLARE ( ExecuteCommandHandler );

    // JSON functions
    LUA_DECLARE ( toJSON );
    LUA_DECLARE ( fromJSON );

    // Standard server functions
    LUA_DECLARE ( GetMaxPlayers );
    LUA_DECLARE ( SetMaxPlayers );
    LUA_DECLARE ( OutputChatBox ); LUA_DECLARE ( OOP_OutputChatBox );
    LUA_DECLARE ( OutputConsole );
    LUA_DECLARE ( OutputDebugString );
    LUA_DECLARE ( OutputServerLog );
    LUA_DECLARE ( GetServerName );
    LUA_DECLARE ( GetServerHttpPort );
    LUA_DECLARE ( GetServerIP );
    LUA_DECLARE ( GetServerPassword );
    LUA_DECLARE ( SetServerPassword );
    LUA_DECLARE ( GetServerConfigSetting );
    LUA_DECLARE ( SetServerConfigSetting );

    LUA_DECLARE ( shutdown );

    // Utility vector math functions
    LUA_DECLARE ( GetDistanceBetweenPoints2D );
    LUA_DECLARE ( GetDistanceBetweenPoints3D );
    LUA_DECLARE ( GetEasingValue );
    LUA_DECLARE ( InterpolateBetween );

    // Util functions to make scripting easier for the end user
    // Some of these are based on standard mIRC script funcs as a lot of people will be used to them
    LUA_DECLARE ( GetTickCount_ );
    LUA_DECLARE ( GetCTime );
    LUA_DECLARE ( Split );
    LUA_DECLARE ( GetTok );
    LUA_DECLARE ( GetColorFromString );
    LUA_DECLARE ( Reference );
    LUA_DECLARE ( Dereference );
    LUA_DECLARE ( UtfLen );
    LUA_DECLARE ( UtfSeek );
    LUA_DECLARE ( UtfSub );
    LUA_DECLARE ( UtfChar );
    LUA_DECLARE ( UtfCode );

    // PCRE functions
    LUA_DECLARE ( PregFind );
    LUA_DECLARE ( PregReplace );
    LUA_DECLARE ( PregMatch );

    // Loaded Map Functions
    LUA_DECLARE ( GetRootElement );
    LUA_DECLARE ( LoadMapData );
    LUA_DECLARE ( SaveMapData );

    // Mesh Functions
    LUA_DECLARE ( CreateMesh );

    // All-Seeing Eye Functions
    LUA_DECLARE ( GetGameType );
    LUA_DECLARE ( GetMapName );
    LUA_DECLARE ( SetGameType );
    LUA_DECLARE ( SetMapName );
    LUA_DECLARE ( GetRuleValue );
    LUA_DECLARE ( SetRuleValue );
    LUA_DECLARE ( RemoveRuleValue );

    // Registry funcs
    LUA_DECLARE ( GetPerformanceStats );

    // Misc funcs
    LUA_DECLARE ( ResetMapInfo );
    LUA_DECLARE ( GetServerPort );

    // Settings registry funcs
    LUA_DECLARE ( Get );
    LUA_DECLARE ( Set );

    // Utility
    LUA_DECLARE ( Md5 );
    LUA_DECLARE ( Sha256 );
    LUA_DECLARE ( Hash );
    LUA_DECLARE ( TeaEncode );
    LUA_DECLARE ( TeaDecode );
    LUA_DECLARE ( Base64encode );
    LUA_DECLARE ( Base64decode );
    LUA_DECLARE ( GetNetworkUsageData );
    LUA_DECLARE ( GetNetworkStats );
    LUA_DECLARE ( GetVersion );
    LUA_DECLARE ( GetModules );
    LUA_DECLARE ( GetModuleInfo );
    LUA_DECLARE ( IsOOPEnabled );

private:
    // Static references to objects
    static CBlipManager*                                m_pBlipManager;
    static CLuaManager*                                 m_pLuaManager;
    static CMarkerManager*                              m_pMarkerManager;
    static CObjectManager*                              m_pObjectManager;
    static CPickupManager*                              m_pPickupManager;
    static CPlayerManager*                              m_pPlayerManager;
    static CRadarAreaManager*                           m_pRadarAreaManager;
    static CRegisteredCommands*                         m_pRegisteredCommands;
    static CElement*                                    m_pRootElement;
    static CScriptDebugging*                            m_pScriptDebugging;
    static CVehicleManager*                             m_pVehicleManager;
    static CTeamManager*                                m_pTeamManager;
    static CAccountManager*                             m_pAccountManager;
    static CColManager*                                 m_pColManager;
    static CResourceManager*                            m_pResourceManager;
    static CAccessControlListManager*                   m_pACLManager;
    static CLuaModuleManager*                           m_pLuaModuleManager;
};