/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.cpp
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
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CTimeUsMarker < 20 > markerLatentEvent; // For timing triggerLatentClientEvent

CBlipManager*              CLuaFunctionDefs::m_pBlipManager;
CLuaManager*               CLuaFunctionDefs::m_pLuaManager;
CMarkerManager*            CLuaFunctionDefs::m_pMarkerManager;
CObjectManager*            CLuaFunctionDefs::m_pObjectManager;
CPickupManager*            CLuaFunctionDefs::m_pPickupManager;
CPlayerManager*            CLuaFunctionDefs::m_pPlayerManager;
CRadarAreaManager*         CLuaFunctionDefs::m_pRadarAreaManager;
CRegisteredCommands*       CLuaFunctionDefs::m_pRegisteredCommands;
CElement*                  CLuaFunctionDefs::m_pRootElement;
CScriptDebugging*          CLuaFunctionDefs::m_pScriptDebugging;
CVehicleManager*           CLuaFunctionDefs::m_pVehicleManager;
CTeamManager*              CLuaFunctionDefs::m_pTeamManager;
CAccountManager*           CLuaFunctionDefs::m_pAccountManager;
CColManager*               CLuaFunctionDefs::m_pColManager;
CResourceManager*          CLuaFunctionDefs::m_pResourceManager;
CAccessControlListManager* CLuaFunctionDefs::m_pACLManager;
CLuaModuleManager*         CLuaFunctionDefs::m_pLuaModuleManager;

void CLuaFunctionDefs::Initialize( CLuaManager* pLuaManager, CGame* pGame )
{
    m_pLuaManager = pLuaManager;
    m_pLuaModuleManager = pLuaManager->GetLuaModuleManager ();
    m_pBlipManager = pGame->GetBlipManager ();
    m_pMarkerManager = pGame->GetMarkerManager ();
    m_pObjectManager = pGame->GetObjectManager ();
    m_pPickupManager = pGame->GetPickupManager ();
    m_pPlayerManager = pGame->GetPlayerManager ();
    m_pRadarAreaManager = pGame->GetRadarAreaManager ();
    m_pTeamManager = pGame->GetTeamManager ();
    m_pAccountManager = pGame->GetAccountManager ();
    m_pRegisteredCommands = pGame->GetRegisteredCommands ();
    m_pRootElement = pGame->GetMapManager ()->GetRootElement ();
    m_pScriptDebugging = pGame->GetScriptDebugging ();
    m_pVehicleManager = pGame->GetVehicleManager ();
    m_pColManager = pGame->GetColManager ();
    m_pResourceManager = pGame->GetResourceManager ();
    m_pACLManager = pGame->GetACLManager ();
}

int CLuaFunctionDefs::SetTrainTrackPosition ( lua_State* luaVM )
{
    // bool setTrainTrackPosition ( int trackID, int trackNode, Vector3D vecPosition )
    // bool setTrainTrackPosition ( int trackID, int trackNode, float x, float y, float z )
    unsigned char ucTrackID = 0;
    unsigned int uiTrackNode = 0;
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucTrackID );
    argStream.ReadNumber ( uiTrackNode );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainTrackPosition ( ucTrackID, uiTrackNode, vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTrainTrackPosition ( lua_State* luaVM )
{
    unsigned char ucTrackID = 0;
    unsigned int uiTrackNode = 0;
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucTrackID );
    argStream.ReadNumber ( uiTrackNode );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackPosition ( ucTrackID, uiTrackNode, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            lua_pushnumber ( luaVM, vecPosition.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::CreateTrainTrack ( lua_State* luaVM )
{
    unsigned int uiTrackNodes = 0;
    float fLength = 0.0f;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiTrackNodes );

    if ( !argStream.HasErrors () )
    {
        // Get the VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Get the resourc
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CTrainTrack * pTrainTrack = CStaticFunctionDefinitions::CreateTrainTrack ( pResource, uiTrackNodes );
                if ( pTrainTrack )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( pTrainTrack );
                    }
                    lua_pushelement ( luaVM, pTrainTrack );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetTrainTrackLength ( lua_State* luaVM )
{
    float fLength = 0.0f;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );
    argStream.ReadNumber ( fLength );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainTrackLength ( pTrainTrack, fLength ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTrainTrackLength ( lua_State* luaVM )
{
    float fLength = 0.0f;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackLength ( pTrainTrack, fLength ) )
        {
            lua_pushnumber ( luaVM, fLength );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetTrainTrackNumberOfNodes ( lua_State* luaVM )
{
    unsigned int uiNodes = 0;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );
    argStream.ReadNumber ( uiNodes );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainTrackNumberOfNodes ( pTrainTrack, uiNodes ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTrainTrackNumberOfNodes ( lua_State* luaVM )
{
    unsigned int uiNodes = 0;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackNumberOfNodes ( pTrainTrack, uiNodes ) )
        {
            lua_pushnumber ( luaVM, uiNodes );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTrainTrackID ( lua_State* luaVM )
{
    unsigned char ucTrack = 0;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackID ( pTrainTrack, ucTrack ) )
        {
            lua_pushnumber ( luaVM, ucTrack );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
