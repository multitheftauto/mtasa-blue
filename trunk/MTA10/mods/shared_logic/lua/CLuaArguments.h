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
#include <vector>

#if MTA_DEBUG
    // Tight allocation in debug to find trouble.
    #define LUA_CHECKSTACK(vm,space) lua_checkstack(vm, (space) )
#else
    // Extra room in release to avoid trouble.
    #define LUA_CHECKSTACK(vm,space) lua_checkstack(vm, (space)*2 )
#endif

class CLuaArguments;

class CLuaArguments
{
public:
                                                        CLuaArguments       ( void )                { }
                                                        CLuaArguments       ( const CLuaArguments& Arguments, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables = NULL );
														CLuaArguments		( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables = NULL );
                                                        ~CLuaArguments      ( void )                { DeleteArguments (); };

    void                                                CopyRecursive       ( const CLuaArguments& Arguments, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables = NULL );

    const CLuaArguments&                                operator =          ( const CLuaArguments& Arguments );
	CLuaArgument*										operator []			( const unsigned int uiPosition ) const;

    void                                                ReadArgument        ( lua_State* luaVM, signed int uiIndex );
    void                                                ReadArguments       ( lua_State* luaVM, signed int uiIndexBegin = 1 );
    void                                                PushArguments       ( lua_State* luaVM ) const;
    void                                                PushArguments       ( CLuaArguments& Arguments );
    bool                                                Call                ( class CLuaMain* pLuaMain, int iLuaFunction, CLuaArguments * returnValues = NULL ) const;
	bool                                                CallGlobal          ( class CLuaMain* pLuaMain, const char* szFunction, CLuaArguments * returnValues = NULL ) const;

    void                                                ReadTable           ( lua_State* luaVM, int iIndexBegin, std::map < const void*, CLuaArguments* > * pKnownTables = NULL );
    void                                                PushAsTable         ( lua_State* luaVM, std::map < CLuaArguments*, int > * pKnownTables = NULL );

    CLuaArgument*                                       PushNil             ( void );
    CLuaArgument*                                       PushBoolean         ( bool bBool );
    CLuaArgument*                                       PushNumber          ( double dNumber );
    CLuaArgument*                                       PushString          ( const char* szString );
    CLuaArgument*                                       PushUserData        ( void* pUserData );
    CLuaArgument*                                       PushElement         ( CClientEntity* pElement );
    CLuaArgument*                                       PushArgument        ( const CLuaArgument& argument );
    CLuaArgument*                                       PushTable           ( CLuaArguments * table );

    void                                                DeleteArguments     ( void );

    bool                                                ReadFromBitStream   ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables = NULL );
    bool                                                WriteToBitStream    ( NetBitStreamInterface& bitStream, std::map < CLuaArguments*, unsigned long > * pKnownTables = NULL ) const;

    unsigned int                                        Count               ( void ) const          { return static_cast < unsigned int > ( m_Arguments.size () ); };
    std::vector < CLuaArgument* > ::const_iterator      IterBegin           ( void )                { return m_Arguments.begin (); };
    std::vector < CLuaArgument* > ::const_iterator      IterEnd             ( void )                { return m_Arguments.end (); };

private:
    std::vector < CLuaArgument* >                       m_Arguments;
};

#endif


