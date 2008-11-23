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

#include <net/net_bitstream.h>
#include "CLuaArgument.h"
#include <vector>


using namespace std;

class CLuaArguments
{
public:
    inline                                              CLuaArguments       ( void )                {};
                                                        CLuaArguments       ( const CLuaArguments& Arguments );
														CLuaArguments		( NetBitStreamInterface& bitStream );
    inline                                              ~CLuaArguments      ( void )                { DeleteArguments (); };

    const CLuaArguments&                                operator =          ( const CLuaArguments& Arguments );

    void                                                ReadArgument        ( lua_State* luaVM, unsigned int uiIndex );
    void                                                ReadArguments       ( lua_State* luaVM, unsigned int uiIndexBegin = 1 );
    void                                                PushArguments       ( lua_State* luaVM ) const;
    void                                                PushArguments       ( CLuaArguments& Arguments );
    bool                                                Call                ( class CLuaMain* pLuaMain, int iLuaFunction, CLuaArguments * returnValues = NULL ) const;
    bool                                                CallGlobal          ( class CLuaMain* pLuaMain, const char* szFunction, CLuaArguments * returnValues = NULL ) const;

    void                                                ReadTable           ( lua_State* luaVM, signed int uiIndexBegin, unsigned int depth = 0 );
    void                                                PushAsTable         ( lua_State* luaVM );

    CLuaArgument*                                       PushNil             ( void );
    CLuaArgument*                                       PushBoolean         ( bool bBool );
    CLuaArgument*                                       PushNumber          ( double dNumber );
    CLuaArgument*                                       PushString          ( const char* szString );
    CLuaArgument*                                       PushUserData        ( void* pUserData );
    CLuaArgument*                                       PushElement         ( CClientEntity* pElement );
    CLuaArgument*                                       PushArgument        ( CLuaArgument & Argument );

    void                                                DeleteArguments     ( void );
	
    bool                                                ReadFromBitStream   ( NetBitStreamInterface& bitStream );
    bool												WriteToBitStream	( NetBitStreamInterface& bitStream );

    inline unsigned int                                 Count               ( void ) const          { return static_cast < unsigned int > ( m_Arguments.size () ); };
    inline vector < CLuaArgument* > ::const_iterator    IterBegin           ( void )                { return m_Arguments.begin (); };
    inline vector < CLuaArgument* > ::const_iterator    IterEnd             ( void )                { return m_Arguments.end (); };

private:
    vector < CLuaArgument* >                            m_Arguments;
};

#endif
