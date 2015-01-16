/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaArguments.h
*  PURPOSE:     Lua arguments manager class header
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#ifndef __CLUAARGUMENTS_H
#define __CLUAARGUMENTS_H

extern "C"
{
    #include "lua.h"
}

#include <net/bitstream.h>
#include "CLuaArgument.h"
#include "json.h"
#include <vector>
#include "CLuaFunctionRef.h"

inline void LUA_CHECKSTACK( lua_State *L, int size )
{
    if ( lua_getstackgap( L ) < size + 5 )
        lua_checkstack( L, ( size + 2 ) * 3 + 4 );
}

class CLuaArguments;

class CLuaArguments
{
public:
                                                        CLuaArguments       ( void )                { }
                                                        CLuaArguments       ( const CLuaArguments& Arguments, CFastHashMap < CLuaArguments*, CLuaArguments* > * pKnownTables = NULL );
                                                        CLuaArguments       ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables = NULL );
                                                        ~CLuaArguments      ( void )                { DeleteArguments (); };

    void                                                CopyRecursive       ( const CLuaArguments& Arguments, CFastHashMap < CLuaArguments*, CLuaArguments* > * pKnownTables = NULL );

    const CLuaArguments&                                operator =          ( const CLuaArguments& Arguments );
    CLuaArgument*                                       operator []         ( const unsigned int uiPosition ) const;

    void                                                ReadArgument        ( lua_State* luaVM, signed int uiIndex );
    void                                                ReadArguments       ( lua_State* luaVM, signed int uiIndexBegin = 1 );
    void                                                PushArguments       ( lua_State* luaVM ) const;
    void                                                PushArguments       ( const CLuaArguments& Arguments );
    bool                                                Call                ( class CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments * returnValues = NULL ) const;
    bool                                                CallGlobal          ( class CLuaMain* pLuaMain, const char* szFunction, CLuaArguments * returnValues = NULL ) const;

    void                                                ReadTable           ( lua_State* luaVM, int iIndexBegin, CFastHashMap < const void*, CLuaArguments* > * pKnownTables = NULL );
    void                                                PushAsTable         ( lua_State* luaVM, CFastHashMap < CLuaArguments*, int > * pKnownTables = NULL );

    CLuaArgument*                                       PushNil             ( void );
    CLuaArgument*                                       PushBoolean         ( bool bBool );
    CLuaArgument*                                       PushNumber          ( double dNumber );
    CLuaArgument*                                       PushString          ( const std::string& strString );
    CLuaArgument*                                       PushElement         ( CClientEntity* pElement );
    CLuaArgument*                                       PushArgument        ( const CLuaArgument& argument );
    CLuaArgument*                                       PushResource        ( CResource* pResource );

    void                                                DeleteArguments     ( void );

    bool                                                ReadFromBitStream   ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables = NULL );
    bool                                                WriteToBitStream    ( NetBitStreamInterface& bitStream, CFastHashMap < CLuaArguments*, unsigned long > * pKnownTables = NULL ) const;
    void                                                ValidateTableKeys   ( void );
    bool                                                ReadFromJSONString  ( const char* szJSON );
    bool                                                WriteToJSONString   ( std::string& strJSON, bool bSerialize = false );
    json_object *                                       WriteTableToJSONObject ( bool bSerialize = false, CFastHashMap < CLuaArguments*, unsigned long > * pKnownTables = NULL );
    json_object *                                       WriteToJSONArray    ( bool bSerialize );
    bool                                                ReadFromJSONObject  ( json_object * object, std::vector < CLuaArguments* > * pKnownTables = NULL );
    bool                                                ReadFromJSONArray   ( json_object * object, std::vector < CLuaArguments* > * pKnownTables = NULL );

    unsigned int                                        Count               ( void ) const          { return static_cast < unsigned int > ( m_Arguments.size () ); };
    std::vector < CLuaArgument* > ::const_iterator      IterBegin           ( void ) const          { return m_Arguments.begin (); };
    std::vector < CLuaArgument* > ::const_iterator      IterEnd             ( void ) const          { return m_Arguments.end (); };

private:
    std::vector < CLuaArgument* >                       m_Arguments;
};

#endif
