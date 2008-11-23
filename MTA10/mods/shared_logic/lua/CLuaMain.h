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

#include "CLuaFunctionDefinitions.h"
#include "CLuaTimerManager.h"

#include "CLuaFunctionDefinitions.h"

#include <xml/CXMLFile.h>

#define MAX_SCRIPTNAME_LENGTH 64

#include <list>
using namespace std;

class CSFXSynth;

class CLuaMain //: public CClient
{
public:
    enum
    {
        OWNER_SERVER,
        OWNER_MAP,
    };

public:
                                    CLuaMain                ( class CLuaManager* pLuaManager/*,
                                                              CObjectManager* pObjectManager,
                                                              CPlayerManager* pPlayerManager,
                                                              CVehicleManager* pVehicleManager,
                                                              CBlipManager* pBlipManager,
                                                              CRadarAreaManager* pRadarAreaManager,
                                                              CMapManager* pMapManager*/ );
                                    ~CLuaMain               ( void );

    int                             GetClientType           ( void ) { /*return CClient::CLIENT_SCRIPT;*/ };
    const char*                     GetNickPointer          ( void ) { return m_szScriptName; };

    void                            SendEcho                ( const char* szEcho ) {};
    void                            SendConsole             ( const char* szEcho ) {};

    inline int                      GetOwner                ( void )                        { return m_iOwner; };
    inline void                     SetOwner                ( int iOwner )                  { m_iOwner = iOwner; };

    bool                            LoadScriptFromFile      ( const char* szLUAScript );
    bool                            LoadScript              ( const char* szLUAScript );
    void                            UnloadScript            ( void );

    void                            Start                   ( void );

    void                            DoPulse                 ( void );

	// Gets the SFXSynth instance or creates one if it's non-existant
	CSFXSynth *						GetSFXSynthInstance		( void );

    void                            GetScriptName           ( char* szLuaScript ) const     { strcpy ( szLuaScript, m_szScriptName ); };
    inline const char*              GetScriptNamePointer    ( void ) const                  { return m_szScriptName; };
    void                            SetScriptName           ( const char* szName )          { strncpy ( m_szScriptName, szName, MAX_SCRIPTNAME_LENGTH ); };

    void                            RegisterFunction        ( const char* szFunction, lua_CFunction function );

    inline lua_State*               GetVM                   ( void )                        { return m_luaVM; };
    inline CLuaTimerManager*        GetTimerManager         ( void ) const                  { return m_pLuaTimerManager; };

    bool                            BeingDeleted            ( void );
    inline lua_State *              GetVirtualMachine       ( void ) const                  { return m_luaVM; };

    void                            ResetInstructionCount   ( void );

    inline class CResource*         GetResource             ( void )                        { return m_pResource; }
    inline void                     SetResource             ( class CResource* pResource )
    {
        m_pResource = pResource;
        UpdateGlobals ();
    }

    CXMLFile *                      CreateXML               ( const char* szFilename );
    void                            DestroyXML              ( CXMLFile* pFile );
    void                            DestroyXML              ( CXMLNode* pRootNode );
    void                            SaveXML                 ( CXMLNode * pRootNode );
    bool                            XMLExists               ( CXMLFile* pFile );

    void                            UpdateGlobals           ( void );

private:
    void                            InitVM                  ( void );
	void							InitSecurity			( void );

    static void                     InstructionCountHook    ( lua_State* luaVM, lua_Debug* pDebug );

    char                            m_szScriptName [MAX_SCRIPTNAME_LENGTH + 1];
    int                             m_iOwner;

    lua_State*                      m_luaVM;
    CLuaTimerManager*               m_pLuaTimerManager;

    bool                            m_bBeingDeleted; // prevent it being deleted twice

    unsigned long                   m_ulFunctionEnterTime;

    class CResource*                m_pResource;

    list < CXMLFile* >              m_XMLFiles;

	// Sound classes
	CSFXSynth*						m_pSFXSynth;
};

#endif
