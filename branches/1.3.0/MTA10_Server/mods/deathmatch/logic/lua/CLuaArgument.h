/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaArgument.h
*  PURPOSE:     Lua argument handler class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAARGUMENT_H
#define __CLUAARGUMENT_H

#include <string>

extern "C"
{
    #include "lua.h"
}
#include "../common/CBitStream.h"
#include "json.h"

class CElement;
class CLuaArguments;

#define LUA_TTABLEREF 9
#define LUA_TSTRING_LONG 10
#define LONG_STRING_MIN_VERSION     "1.3.0-9.03772"

class CLuaArgument
{
public:
                            CLuaArgument        ( void );
                            CLuaArgument        ( bool bBool );
                            CLuaArgument        ( double dNumber );
                            CLuaArgument        ( const std::string& strString );
                            CLuaArgument        ( CElement* pElement );
                            CLuaArgument        ( const CLuaArgument& Argument, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables = NULL );
                            CLuaArgument        ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables = NULL );
                            CLuaArgument        ( lua_State* luaVM, int iArgument, std::map < const void*, CLuaArguments* > * pKnownTables = NULL );
                            ~CLuaArgument       ( void );

    const CLuaArgument&     operator =          ( const CLuaArgument& Argument );
    bool                    operator ==         ( const CLuaArgument& Argument );
    bool                    operator !=         ( const CLuaArgument& Argument );

    void                    Read                ( lua_State* luaVM, int iArgument, std::map < const void*, CLuaArguments* > * pKnownTables = NULL );
    void                    Push                ( lua_State* luaVM, std::map < CLuaArguments*, int > * pKnownTables = NULL ) const;
    
    void                    Read                ( bool bBool );
    void                    Read                ( double dNumber );
    void                    Read                ( const std::string& strString );
    void                    Read                ( CElement* pElement );

    void                    ReadUserData        ( void* pUserData );
    void                    Read                ( class CLuaArguments * table );

    inline int              GetType             ( void ) const      { return m_iType; };

    inline bool             GetBoolean          ( void ) const      { return m_bBoolean; };
    inline lua_Number       GetNumber           ( void ) const      { return m_Number; };
    inline const std::string& GetString         ( void )            { return m_strString; };
    inline void*            GetLightUserData    ( void ) const      { return m_pLightUserData; };
    CElement*               GetElement          ( void ) const;
    bool                    GetAsString         ( SString& strBuffer );

    bool                    ReadFromBitStream   ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables = NULL );
    bool                    WriteToBitStream    ( NetBitStreamInterface& bitStream, std::map < CLuaArguments*, unsigned long > * pKnownTables = NULL ) const;
    json_object*            WriteToJSONObject   ( bool bSerialize = false, std::map < CLuaArguments*, unsigned long > * pKnownTables = NULL );
    bool                    ReadFromJSONObject  ( json_object* object, std::vector < CLuaArguments* > * pKnownTables = NULL );
    char *                  WriteToString       ( char * szBuffer, int length );

private:
    void                    LogUnableToPacketize    ( const char* szMessage ) const;

    int                     m_iType;
    bool                    m_bBoolean;
    lua_Number              m_Number;
    std::string             m_strString;
    void*                   m_pLightUserData;
    CLuaArguments*          m_pTableData;
    bool                    m_bWeakTableRef;

    std::string             m_strFilename;
    int                     m_iLine;

    void                    CopyRecursive       ( const CLuaArgument& Argument, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables = NULL );
    bool                    CompareRecursive    ( const CLuaArgument& Argument, std::set < CLuaArguments* > * pKnownTables = NULL );
    void                    DeleteTableData     ( void );
};

#endif
