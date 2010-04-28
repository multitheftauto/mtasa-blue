/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaMain.h
*  PURPOSE:     Lua virtual machine container class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Cecill Etheredge <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaMain;

#ifndef __CLUAMAIN_H
#define __CLUAMAIN_H

#include "CLuaTimerManager.h"
#include "CLuaModuleManager.h"
#include "../CTextDisplay.h"

#define MAX_SCRIPTNAME_LENGTH 64

class CBlipManager;
class CObjectManager;
class CPlayerManager;
class CRadarAreaManager;
class CVehicleManager;
class CMapManager;

class CLuaMain : public CClient
{
public:
    enum
    {
        OWNER_SERVER,
        OWNER_MAP,
    };

public:
                                    CLuaMain                ( class CLuaManager* pLuaManager,
                                                              CObjectManager* pObjectManager,
                                                              CPlayerManager* pPlayerManager,
                                                              CVehicleManager* pVehicleManager,
                                                              CBlipManager* pBlipManager,
                                                              CRadarAreaManager* pRadarAreaManager,
                                                              CMapManager* pMapManager,
                                                              CResource* pResourceOwner );
                                    ~CLuaMain               ( void );

    int                             GetClientType           ( void ) { return CClient::CLIENT_SCRIPT; };
    const char*                     GetNick                 ( void ) { return m_szScriptName; };

    void                            SendEcho                ( const char* szEcho ) {};
    void                            SendConsole             ( const char* szEcho ) {};

    inline int                      GetOwner                ( void )                        { return m_iOwner; };
    inline void                     SetOwner                ( int iOwner )                  { m_iOwner = iOwner; };

    bool                            LoadScriptFromFile      ( const char* szLUAScript );
    bool                            LoadScript              ( const char* szLUAScript );
    void                            UnloadScript            ( void );

    void                            Start                   ( void );

    void                            DoPulse                 ( void );

    void                            GetScriptName           ( char* szLuaScript ) const     { strcpy ( szLuaScript, m_szScriptName ); };
    inline const char*              GetScriptNamePointer    ( void ) const                  { return m_szScriptName; };
    void                            SetScriptName           ( const char* szName )          { strncpy ( m_szScriptName, szName, MAX_SCRIPTNAME_LENGTH ); };

    void                            RegisterFunction        ( const char* szFunction, lua_CFunction function );

    inline lua_State*               GetVM                   ( void )                        { return m_luaVM; };
    inline CLuaTimerManager*        GetTimerManager         ( void ) const                  { return m_pLuaTimerManager; };

    inline CBlipManager*            GetBlipManager          ( void ) const                  { return m_pBlipManager; };
    inline CObjectManager*          GetObjectManager        ( void ) const                  { return m_pObjectManager; };
    inline CPlayerManager*          GetPlayerManager        ( void ) const                  { return m_pPlayerManager; };
    inline CVehicleManager*         GetVehicleManager       ( void ) const                  { return m_pVehicleManager; };
    inline CMapManager*             GetMapManager           ( void ) const                  { return m_pMapManager; };
    
    void                            AddXML                  ( CXMLFile* pFile );
    CXMLFile *                      CreateXML               ( const char* szFilename );
    void                            DestroyXML              ( CXMLFile* pFile );
    void                            DestroyXML              ( CXMLNode* pRootNode );
    void                            SaveXML                 ( CXMLNode * pRootNode );
    bool                            XMLExists               ( CXMLFile* pFile );

    CTextDisplay *                  CreateDisplay           ( void );
    void                            DestroyDisplay          ( CTextDisplay * pDisplay );
    CTextItem *                     CreateTextItem          ( const char* szText, float fX, float fY, eTextPriority priority = PRIORITY_LOW, const SColor color = -1, float fScale = 1.0f, unsigned char format = 0, unsigned char ucShadowAlpha = 0 );
    void                            DestroyTextItem         ( CTextItem * pTextItem );

    bool                            TextDisplayExists       ( CTextDisplay* pDisplay );
    bool                            TextItemExists          ( CTextItem* pTextItem );

    bool                            BeingDeleted            ( void );
    inline lua_State *              GetVirtualMachine       ( void ) const                  { return m_luaVM; };

    void                            ResetInstructionCount   ( void );

    inline CResource *              GetResource             ( void ) { return m_pResource; }

    inline void                     SetResourceFile         ( class CResourceFile * resourceFile ) { m_pResourceFile = resourceFile; }
    inline CResourceFile *          GetResourceFile         ( void ) { return m_pResourceFile; }

    void                            RegisterHTMLDFunctions  ( void );

private:
    void                            InitVM                  ( void );
    void                            InitSecurity            ( void );

    static void                     InstructionCountHook    ( lua_State* luaVM, lua_Debug* pDebug );

    char                            m_szScriptName [MAX_SCRIPTNAME_LENGTH + 1];
    int                             m_iOwner;

    lua_State*                      m_luaVM;
    CLuaTimerManager*               m_pLuaTimerManager;

    class CResource*                m_pResource;
    class CResourceFile*            m_pResourceFile;
    CBlipManager*                   m_pBlipManager;
    CObjectManager*                 m_pObjectManager;
    CPlayerManager*                 m_pPlayerManager;
    CRadarAreaManager*              m_pRadarAreaManager;
    CVehicleManager*                m_pVehicleManager;
    CMapManager*                    m_pMapManager;

    list < CXMLFile* >              m_XMLFiles;
    list < CTextDisplay* >          m_Displays;
    list < CTextItem* >             m_TextItems;

    bool                            m_bBeingDeleted; // prevent it being deleted twice

    unsigned long                   m_ulFunctionEnterTime;
};

#endif
