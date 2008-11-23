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

class CLuaArgument
{
public:
                            CLuaArgument        ( void );
                            CLuaArgument        ( bool bBool );
                            CLuaArgument        ( double dNumber );
                            CLuaArgument        ( const char* szString );
                            CLuaArgument        ( CElement* pElement );
                            CLuaArgument        ( const CLuaArgument& Argument );
                            CLuaArgument        ( lua_State* luaVM, signed int uiArgument, unsigned int depth = 0 );
                            ~CLuaArgument       ( void );

    const CLuaArgument&     operator =          ( const CLuaArgument& Argument );
    bool                    operator ==         ( const CLuaArgument& Argument );
    bool                    operator !=         ( const CLuaArgument& Argument );

    void                    Read                ( lua_State* luaVM, signed int uiArgument, unsigned int depth = 0 );
    void                    Push                ( lua_State* luaVM ) const;
    
    void                    Read                ( bool bBool );
    void                    Read                ( double dNumber );
    void                    Read                ( const char* szString );
    void                    Read                ( CElement* pElement );

    void                    ReadUserData        ( void* pUserData );
    void                    Read                ( class CLuaArguments * table );

    inline int              GetType             ( void ) const      { return m_iType; };

    inline bool             GetBoolean          ( void ) const      { return m_bBoolean; };
    inline lua_Number       GetNumber           ( void ) const      { return m_Number; };
    inline std::string&     GetString           ( void )            { return m_strString; };
    inline void*            GetLightUserData    ( void ) const      { return m_pLightUserData; };
    CElement*               GetElement          ( void ) const;
    bool                    GetAsString         ( char * szBuffer, unsigned int uiLength );

    bool                    ReadFromBitStream   ( NetServerBitStreamInterface& bitStream );
    bool                    WriteToBitStream    ( NetServerBitStreamInterface& bitStream ) const;
    json_object*            WriteToJSONObject   ( bool bSerialize = false );
    bool                    ReadFromJSONObject  ( json_object* object );
    char *                  WriteToString       ( char * szBuffer, int length );

private:
    void                    LogUnableToPacketize    ( const char* szMessage ) const;

    int                     m_iType;
    bool                    m_bBoolean;
    lua_Number              m_Number;
    std::string             m_strString;
    void*                   m_pLightUserData;
    class CLuaArguments*    m_pTableData;

    std::string             m_strFilename;
    int                     m_iLine;
};

#endif
