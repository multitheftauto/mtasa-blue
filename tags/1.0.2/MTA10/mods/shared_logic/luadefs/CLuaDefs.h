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

#ifndef __CLUADEFS_H
#define __CLUADEFS_H

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
class CLuaManager;
class CRegisteredCommands;
class CRenderWare;
class CResourceManager;
class CScriptDebugging;

#define argtype(number,type) (lua_type(luaVM,number) == type)
#define argtype2(number,type,type2) (lua_type(luaVM,number) == type || lua_type(luaVM,number) == type2)

class CLuaDefs
{
public:
    static void                         Initialize      ( class CClientGame* pClientGame,
                                                          CLuaManager* pLuaManager,
                                                          CScriptDebugging* pScriptDebugging );

// This is just for the LUA funcs. Please don't public this and use it other
// places in the client.
public:
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

#endif
