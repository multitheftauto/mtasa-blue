/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaDefs.h
*  PURPOSE:     Lua definitions header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#pragma once
#define LUA_DECLARE(x) static int x ( lua_State * luaVM ); 
#define LUA_DECLARE_OOP(x) LUA_DECLARE(x) LUA_DECLARE(OOP_##x)

class CClientColManager;
class CClientColModelManager;
class CClientDFFManager;
class CClientEntity;
class CClientGame;
class CClientGUIManager;
class CClientManager;
class CClientMarkerManager;
class CClientObjectManager;
class CClientPickupManager;
class CClientPlayerManager;
class CClientRadarMarkerManager;
class CClientTeamManager;
class CClientVehicleManager;
class CElementDeleter;
class CLuaManager;
class CRegisteredCommands;
class CRenderWare;
class CResourceManager;
class CScriptDebugging;

class CLuaDefs
{
public:
    static void                         Initialize      ( class CClientGame* pClientGame,
                                                          CLuaManager* pLuaManager,
                                                          CScriptDebugging* pScriptDebugging );

    static int                          CanUseFunction ( lua_CFunction f, lua_State* luaVM );
    static void                         DidUseFunction ( lua_CFunction f, lua_State* luaVM );

// This is just for the Lua funcs. Please don't public this and use it other
// places in the client.
public:
    static CElementDeleter*             m_pElementDeleter;
    static CLuaManager*                 m_pLuaManager;
    static CScriptDebugging*            m_pScriptDebugging;
    static class CClientGame*           m_pClientGame;
    static CClientManager*              m_pManager;
    static CClientEntity*               m_pRootEntity;
    static CClientGUIManager*           m_pGUIManager;
    static CClientPlayerManager*        m_pPlayerManager;
    static CClientRadarMarkerManager*   m_pRadarMarkerManager;
    static CResourceManager*            m_pResourceManager;
    static CClientVehicleManager*       m_pVehicleManager;
    static CClientColManager*           m_pColManager;
    static CClientObjectManager*        m_pObjectManager;
    static CClientTeamManager*          m_pTeamManager;
    static CRenderWare*                 m_pRenderWare;
    static CClientMarkerManager*        m_pMarkerManager;
    static CClientPickupManager*        m_pPickupManager;
    static CClientDFFManager*           m_pDFFManager;
    static CClientColModelManager*      m_pColModelManager;
    static CRegisteredCommands*         m_pRegisteredCommands;  
};