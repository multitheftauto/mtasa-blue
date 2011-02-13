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

struct CRefInfo
{
    unsigned long int ulUseCount;
    int iFunction;
};

class CLuaMain //: public CClient
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
    unsigned long                   GetXMLFileCount         ( void ) const                  { return m_XMLFiles.size (); };
    unsigned long                   GetTimerCount           ( void ) const                  { return m_pLuaTimerManager ? m_pLuaTimerManager->GetTimerCount () : 0; };
    unsigned long                   GetElementCount         ( void ) const                  { return m_pResource && m_pResource->GetElementGroup () ? m_pResource->GetElementGroup ()->GetCount () : 0; };
    unsigned long                   GetTextDisplayCount     ( void ) const                  { return m_Displays.size (); };
    unsigned long                   GetTextItemCount        ( void ) const                  { return m_TextItems.size (); };

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

    void                            InitVM                  ( void );
    const SString&                  GetFunctionTag          ( int iFunctionNumber );
private:
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
public:
    std::map < const void*, CRefInfo >      m_CallbackTable;
    std::map < int, SString >       m_FunctionTagMap;
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
