/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaMain.h
*  PURPOSE:     Lua virtual machine container class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaMain;

#ifndef __CLUAMAIN_H
#define __CLUAMAIN_H

#include "CLuaTimerManager.h"
#include "CLuaVector.h"
#include "CLuaMatrix.h"

#include "CLuaFunctionDefs.h"

#include <xml/CXMLFile.h>

#define MAX_SCRIPTNAME_LENGTH 64

#include <list>

struct CRefInfo
{
    unsigned long int ulUseCount;
    int iFunction;
};

class CLuaMain //: public CClient
{
public:
    ZERO_ON_NEW
                                    CLuaMain                ( class CLuaManager* pLuaManager, CResource* pResourceOwner, bool bEnableOOP );
                                    ~CLuaMain               ( void );

    bool                            LoadScriptFromBuffer    ( const char* cpBuffer, unsigned int uiSize, const char* szFileName );
    bool                            LoadScript              ( const char* szLUAScript );
    void                            UnloadScript            ( void );

    void                            Start                   ( void );

    void                            DoPulse                 ( void );

    const char*                     GetScriptName           ( void ) const                  { return m_strScriptName; }
    void                            SetScriptName           ( const char* szName )          { m_strScriptName.AssignLeft ( szName, MAX_SCRIPTNAME_LENGTH ); }

    void                            RegisterFunction        ( const char* szFunction, lua_CFunction function );

    inline lua_State*               GetVM                   ( void )                        { return m_luaVM; };
    inline CLuaTimerManager*        GetTimerManager         ( void ) const                  { return m_pLuaTimerManager; };

    bool                            BeingDeleted            ( void );
    inline lua_State *              GetVirtualMachine       ( void ) const                  { return m_luaVM; };

    void                            ResetInstructionCount   ( void );

    inline class CResource*         GetResource             ( void )                        { return m_pResource; }

    CXMLFile *                      CreateXML               ( const char* szFilename );
    void                            DestroyXML              ( CXMLFile* pFile );
    void                            DestroyXML              ( CXMLNode* pRootNode );
    void                            SaveXML                 ( CXMLNode * pRootNode );
    bool                            XMLExists               ( CXMLFile* pFile );
    unsigned long                   GetXMLFileCount         ( void ) const                  { return m_XMLFiles.size (); };
    unsigned long                   GetTimerCount           ( void ) const                  { return m_pLuaTimerManager ? m_pLuaTimerManager->GetTimerCount () : 0; };
    unsigned long                   GetElementCount         ( void ) const;

    void                            AddElementClass         ( lua_State* luaVM );
    void                            AddVehicleClass         ( lua_State* luaVM );
    void                            AddPedClass             ( lua_State* luaVM );
    void                            AddPlayerClass          ( lua_State* luaVM );
    void                            AddObjectClass          ( lua_State* luaVM );
    void                            AddMarkerClass          ( lua_State* luaVM );
    void                            AddBlipClass            ( lua_State* luaVM );
    void                            AddPickupClass          ( lua_State* luaVM );
    void                            AddColShapeClass        ( lua_State* luaVM );
    void                            AddProjectileClass      ( lua_State* luaVM );
    void                            AddRadarAreaClass       ( lua_State* luaVM );
    void                            AddTeamClass            ( lua_State* luaVM );
    void                            AddWaterClass           ( lua_State* luaVM );
    void                            AddSoundClass           ( lua_State* luaVM );
    void                            AddWeaponClass          ( lua_State* luaVM );
    void                            AddEffectClass          ( lua_State* luaVM );

    void                            AddGuiElementClass      ( lua_State* luaVM );
    void                            AddGuiFontClass       ( lua_State* luaVM );
    void                            AddGuiWindowClass       ( lua_State* luaVM );
    void                            AddGuiButtonClass       ( lua_State* luaVM );
    void                            AddGuiEditClass         ( lua_State* luaVM );
    void                            AddGuiLabelClass        ( lua_State* luaVM );
    void                            AddGuiMemoClass         ( lua_State* luaVM );
    void                            AddGuiImageClass        ( lua_State* luaVM );
    void                            AddGuiComboBoxClass     ( lua_State* luaVM );
    void                            AddGuiCheckBoxClass     ( lua_State* luaVM );
    void                            AddGuiRadioButtonClass  ( lua_State* luaVM );
    void                            AddGuiScrollPaneClass   ( lua_State* luaVM );
    void                            AddGuiScrollBarClass    ( lua_State* luaVM );
    void                            AddGuiProgressBarClass  ( lua_State* luaVM );
    void                            AddGuiGridlistClass     ( lua_State* luaVM );
    void                            AddGuiTabPanelClass     ( lua_State* luaVM );
    void                            AddGuiTabClass          ( lua_State* luaVM );

    void                            AddResourceClass        ( lua_State* luaVM );
    void                            AddTimerClass           ( lua_State* luaVM );
    void                            AddFileClass            ( lua_State* luaVM );
    void                            AddXmlNodeClass         ( lua_State* luaVM );

    void                            AddCameraClass          ( lua_State* luaVM );

    void                            AddVector3DClass        ( lua_State* luaVM );
    void                            AddVector2DClass        ( lua_State* luaVM );
    void                            AddMatrixClass          ( lua_State* luaVM );

    void                            InitClasses             ( lua_State* luaVM );
    void                            InitVM                  ( void );
    const SString&                  GetFunctionTag          ( int iLuaFunction );
    int                             PCall                   ( lua_State *L, int nargs, int nresults, int errfunc );

    bool                            IsOOPEnabled            ( void )                        { return m_bEnableOOP; }
private:
    void                            InitSecurity            ( void );

    static void                     InstructionCountHook    ( lua_State* luaVM, lua_Debug* pDebug );

    SString                         m_strScriptName;

    lua_State*                      m_luaVM;
    CLuaTimerManager*               m_pLuaTimerManager;

    bool                            m_bBeingDeleted; // prevent it being deleted twice

    CElapsedTime                    m_FunctionEnterTimer;

    class CResource*                m_pResource;

    std::list < CXMLFile* >         m_XMLFiles;

    bool                            m_bEnableOOP;
public:
    std::map < const void*, CRefInfo >      m_CallbackTable;
    std::map < int, SString >               m_FunctionTagMap;
};

#endif
