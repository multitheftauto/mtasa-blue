/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaArgument.h
*  PURPOSE:     Lua argument class header
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CLUAARGUMENT_H
#define __CLUAARGUMENT_H

extern "C"
{
    #include "lua.h"
}
#include <net/net_bitstream.h>
#include <string>

class CClientEntity;

class CLuaArgument
{
public:
                            CLuaArgument        ( void );
                            CLuaArgument        ( bool bBool );
                            CLuaArgument        ( double dNumber );
                            CLuaArgument        ( const char* szString );
                            CLuaArgument        ( void* pUserData );
                            CLuaArgument        ( CClientEntity* pElement );
                            CLuaArgument        ( const CLuaArgument& Argument );
                            CLuaArgument        ( lua_State* luaVM, signed int uiArgument, unsigned int depth = 0 );
                            ~CLuaArgument       ( void );

    const CLuaArgument&     operator =          ( const CLuaArgument& Argument );
    bool                    operator ==         ( const CLuaArgument& Argument );
    bool                    operator !=         ( const CLuaArgument& Argument );

    void                    Read                ( lua_State* luaVM, signed int uiArgument, unsigned int depth = 0 );
    void                    Read                ( bool bBool );
    void                    Read                ( double dNumber );
    void                    Read                ( const char* szString );
    void                    Read                ( void* pUserData );
    void                    Read                ( CClientEntity* pElement );
    void                    ReadElementID       ( ElementID ID );

    void                    Push                ( lua_State* luaVM ) const;

    inline int              GetType             ( void ) const      { return m_iType; };

    inline bool             GetBoolean          ( void ) const      { return m_bBoolean; };
    inline lua_Number       GetNumber           ( void ) const      { return m_Number; };
    inline const char*      GetString           ( void ) const      { return m_strString.c_str (); };
    inline void*            GetLightUserData    ( void ) const      { return m_pLightUserData; };
    CClientEntity*          GetElement          ( void ) const;

    bool                    ReadFromBitStream   ( NetBitStreamInterface& bitStream );
    bool                    WriteToBitStream    ( NetBitStreamInterface& bitStream );

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
