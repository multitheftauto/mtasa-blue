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

#include "CLuaFunctionDefs.h"

#include <xml/CXMLFile.h>

#define MAX_SCRIPTNAME_LENGTH 64

#include <list>

class CLuaMain //: public CClient
{
public:
    enum
    {
        OWNER_SERVER,
        OWNER_MAP,
    };

public:
                                    CLuaMain                ( class CLuaManager* pLuaManager, CResource* pResourceOwner /*,
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
    bool                            LoadScriptFromBuffer    ( const char* cpBuffer, unsigned int uiSize, const char* szFileName, bool bUTF8 );
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

    bool                            BeingDeleted            ( void );
    inline lua_State *              GetVirtualMachine       ( void ) const                  { return m_luaVM; };

    void                            ResetInstructionCount   ( void );

    inline class CResource*         GetResource             ( void )                        { return m_pResource; }

    CXMLFile *                      CreateXML               ( const char* szFilename );
    void                            DestroyXML              ( CXMLFile* pFile );
    void                            DestroyXML              ( CXMLNode* pRootNode );
    void                            SaveXML                 ( CXMLNode * pRootNode );
    bool                            XMLExists               ( CXMLFile* pFile );

    void                            InitVM                  ( void );
private:
    void                            InitSecurity            ( void );

    static void                     InstructionCountHook    ( lua_State* luaVM, lua_Debug* pDebug );

    char                            m_szScriptName [MAX_SCRIPTNAME_LENGTH + 1];
    int                             m_iOwner;

    lua_State*                      m_luaVM;
    CLuaTimerManager*               m_pLuaTimerManager;

    bool                            m_bBeingDeleted; // prevent it being deleted twice

    unsigned long                   m_ulFunctionEnterTime;

    class CResource*                m_pResource;

    std::list < CXMLFile* >         m_XMLFiles;
};


/////////////////////////////////////////////////////////////////////////
//
// CScriptArgReader
//
//
// Attempt to simplify the reading of arguments from a script call
//
//////////////////////////////////////////////////////////////////////
class CScriptArgReader
{
public:
    CScriptArgReader ( lua_State* luaVM )
    {
        m_luaVM = luaVM;
        m_iIndex = 1;
        m_bError = false;
    }

    //
    // Read next number
    //
    template < class T >
    bool ReadNumber ( T& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex++ );
        if ( iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING )
        {
            outValue = static_cast < T > ( lua_tonumber ( m_luaVM, m_iIndex - 1 ) );
            return true;
        }

        outValue = 0;
        m_bError = true;
        return false;
    }

    //
    // Read next number, using default if required
    //
    template < class T, class U >
    bool ReadNumber ( T& outValue, const U& defaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex++ );
        if ( iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING )
        {
            outValue = static_cast < T > ( lua_tonumber ( m_luaVM, m_iIndex - 1 ) );
            return true;
        }
        else
        if ( iArgument == LUA_TNONE )
        {
            outValue = static_cast < T > ( defaultValue );
            return true;
        }

        outValue = 0;
        m_bError = true;
        return false;
    }

    //
    // Read next bool
    //
    bool ReadBool ( bool& bOutValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex++ );
        if ( iArgument == LUA_TBOOLEAN )
        {
            bOutValue = lua_toboolean ( m_luaVM, m_iIndex - 1 ) ? true : false;
            return true;
        }

        bOutValue = false;
        m_bError = true;
        return true;
    }

    //
    // Read next bool, using default if required
    //
    bool ReadBool ( bool& bOutValue, bool bDefault )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex++ );
        if ( iArgument == LUA_TBOOLEAN )
        {
            bOutValue = lua_toboolean ( m_luaVM, m_iIndex - 1 ) ? true : false;
            return true;
        }
        else
        if ( iArgument == LUA_TNONE )
        {
            bOutValue = bDefault;
            return true;
        }

        bOutValue = false;
        m_bError = true;
        return true;
    }

    //
    // Read next string, using default if required
    //
    bool ReadString ( SString& outValue, const char* defaultValue = NULL )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex++ );
        if ( iArgument == LUA_TSTRING )
        {
            outValue = lua_tostring ( m_luaVM, m_iIndex - 1 );
            return true;
        }
        else
        if ( iArgument == LUA_TNONE && defaultValue )
        {
            outValue = defaultValue;
            return true;
        }

        outValue = "";
        m_bError = true;
        return false;
    }

    bool HasErrors ( void ) { return m_bError; }

    bool        m_bError;
    int         m_iIndex;
    lua_State*  m_luaVM;
};


#endif
