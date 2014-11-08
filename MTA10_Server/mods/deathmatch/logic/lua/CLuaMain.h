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
#include "CLuaVector2.h"
#include "CLuaVector3.h"
#include "CLuaVector4.h"
#include "CLuaMatrix.h"
#include "CLuaModuleManager.h"
#include "../CTextDisplay.h"

#include "CLuaFunctionDefinitions.h"

#define MAX_SCRIPTNAME_LENGTH 64

class CBlipManager;
class CObjectManager;
class CPlayerManager;
class CRadarAreaManager;
class CVehicleManager;
class CMapManager;

struct CRefInfo
{
    unsigned long int ulUseCount;
    int iFunction;
};

class CLuaMain //: public CClient
{
public:
    ZERO_ON_NEW
                                    CLuaMain                ( class CLuaManager* pLuaManager,
                                                              CObjectManager* pObjectManager,
                                                              CPlayerManager* pPlayerManager,
                                                              CVehicleManager* pVehicleManager,
                                                              CBlipManager* pBlipManager,
                                                              CRadarAreaManager* pRadarAreaManager,
                                                              CMapManager* pMapManager,
                                                              CResource* pResourceOwner, 
                                                              bool bEnableOOP );

                                    ~CLuaMain               ( void );

    bool                            LoadScriptFromBuffer    ( const char* cpBuffer, unsigned int uiSize, const char* szFileName );
    bool                            LoadScript              ( const char* szLUAScript );
    void                            UnloadScript            ( void );

    void                            Start                   ( void );

    void                            DoPulse                 ( void );

    inline const char*              GetScriptName           ( void ) const                  { return m_strScriptName; }
    void                            SetScriptName           ( const char* szName )          { m_strScriptName.AssignLeft ( szName, MAX_SCRIPTNAME_LENGTH ); }

    void                            RegisterFunction        ( const char* szFunction, lua_CFunction function );

    inline lua_State*               GetVM                   ( void )                        { return m_luaVM; };
    inline CLuaTimerManager*        GetTimerManager         ( void ) const                  { return m_pLuaTimerManager; };

    inline CBlipManager*            GetBlipManager          ( void ) const                  { return m_pBlipManager; };
    inline CObjectManager*          GetObjectManager        ( void ) const                  { return m_pObjectManager; };
    inline CPlayerManager*          GetPlayerManager        ( void ) const                  { return m_pPlayerManager; };
    inline CVehicleManager*         GetVehicleManager       ( void ) const                  { return m_pVehicleManager; };
    inline CMapManager*             GetMapManager           ( void ) const                  { return m_pMapManager; };
    
    CXMLFile *                      CreateXML               ( const char* szFilename );
    void                            DestroyXML              ( CXMLFile* pFile );
    void                            DestroyXML              ( CXMLNode* pRootNode );
    void                            SaveXML                 ( CXMLNode * pRootNode );
    bool                            XMLExists               ( CXMLFile* pFile );
    unsigned long                   GetXMLFileCount         ( void ) const                  { return m_XMLFiles.size (); };
    unsigned long                   GetOpenFileCount        ( void ) const                  { return m_OpenFilenameList.size(); };
    unsigned long                   GetTimerCount           ( void ) const                  { return m_pLuaTimerManager ? m_pLuaTimerManager->GetTimerCount () : 0; };
    unsigned long                   GetElementCount         ( void ) const                  { return m_pResource && m_pResource->GetElementGroup () ? m_pResource->GetElementGroup ()->GetCount () : 0; };
    unsigned long                   GetTextDisplayCount     ( void ) const                  { return m_Displays.size (); };
    unsigned long                   GetTextItemCount        ( void ) const                  { return m_TextItems.size (); };
    void                            OnOpenFile              ( const SString& strFilename );
    void                            OnCloseFile             ( const SString& strFilename );

    CTextDisplay *                  CreateDisplay           ( void );
    void                            DestroyDisplay          ( CTextDisplay * pDisplay );
    CTextItem *                     CreateTextItem          ( const char* szText, float fX, float fY, eTextPriority priority = PRIORITY_LOW, const SColor color = -1, float fScale = 1.0f, unsigned char format = 0, unsigned char ucShadowAlpha = 0 );
    void                            DestroyTextItem         ( CTextItem * pTextItem );

    CTextDisplay*                   GetTextDisplayFromScriptID    ( uint uiScriptID );
    CTextItem*                      GetTextItemFromScriptID       ( uint uiScriptID );

    bool                            BeingDeleted            ( void );
    inline lua_State *              GetVirtualMachine       ( void ) const                  { return m_luaVM; };

    void                            ResetInstructionCount   ( void );

    inline CResource *              GetResource             ( void ) { return m_pResource; }

    inline void                     SetResourceFile         ( class CResourceFile * resourceFile ) { m_pResourceFile = resourceFile; }
    inline CResourceFile *          GetResourceFile         ( void ) { return m_pResourceFile; }

    void                            RegisterHTMLDFunctions  ( void );

    void                            InitVM                  ( void );
    const SString&                  GetFunctionTag          ( int iFunctionNumber );
    int                             PCall                   ( lua_State *L, int nargs, int nresults, int errfunc );
    void                            CheckExecutionTime      ( void );
    static int                      LuaLoadBuffer           ( lua_State *L, const char *buff, size_t sz, const char *name );
    static int                      OnUndump                ( const char* p, size_t n );

private:
    void                            InitSecurity            ( void );
    void                            InitClasses             ( lua_State* luaVM );

public:

    void                            AddVector4DClass        ( lua_State* luaVM );
    void                            AddVector3DClass        ( lua_State* luaVM );
    void                            AddVector2DClass        ( lua_State* luaVM );
    void                            AddMatrixClass          ( lua_State* luaVM );

    void                            AddElementClass         ( lua_State* luaVM );
    void                            AddACLClass             ( lua_State* luaVM );
    void                            AddACLGroupClass        ( lua_State* luaVM );
    void                            AddAccountClass         ( lua_State* luaVM );
    void                            AddBanClass             ( lua_State* luaVM );
    void                            AddBlipClass            ( lua_State* luaVM );
    void                            AddColShapeClass        ( lua_State* luaVM );
    void                            AddFileClass            ( lua_State* luaVM );
    void                            AddMarkerClass          ( lua_State* luaVM );
    void                            AddObjectClass          ( lua_State* luaVM );
    void                            AddPedClass             ( lua_State* luaVM );
    void                            AddPickupClass          ( lua_State* luaVM );
    void                            AddPlayerClass          ( lua_State* luaVM );
    void                            AddRadarAreaClass       ( lua_State* luaVM );
    void                            AddResourceClass        ( lua_State* luaVM );
    void                            AddConnectionClass      ( lua_State* luaVM );
    void                            AddQueryHandleClass     ( lua_State* luaVM );
    void                            AddTeamClass            ( lua_State* luaVM );
    void                            AddTextDisplayClass     ( lua_State* luaVM );
    void                            AddTextItemClass        ( lua_State* luaVM );
    void                            AddVehicleClass         ( lua_State* luaVM );
    void                            AddWaterClass           ( lua_State* luaVM );
    void                            AddXMLClass             ( lua_State* luaVM );
    void                            AddTimerClass           ( lua_State* luaVM );
    
    bool                            IsOOPEnabled            ( void )                        { return m_bEnableOOP; }
private:

    static void                     InstructionCountHook    ( lua_State* luaVM, lua_Debug* pDebug );

    SString                         m_strScriptName;

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

    bool                            m_bEnableOOP;

    bool                            m_bBeingDeleted; // prevent it being deleted twice

    CElapsedTime                    m_FunctionEnterTimer;
    CElapsedTimeApprox              m_WarningTimer;
    uint                            m_uiPCallDepth;
    std::vector < SString >         m_OpenFilenameList;
    uint                            m_uiOpenFileCountWarnThresh;
    uint                            m_uiOpenXMLFileCountWarnThresh;
    static SString                  ms_strExpectedUndumpHash;

public:
    CFastHashMap < const void*, CRefInfo >  m_CallbackTable;
    std::map < int, SString >       m_FunctionTagMap;
};

#endif
