/*********************************************************
*
*  Multi Theft Auto: San Andreas - Deathmatch
*
*  ml_base, External lua add-on module
*  
*  Copyright © 2003-2008 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 2002-2003 Rockstar North
*
*  THE FOLLOWING SOURCES ARE PART OF THE MULTI THEFT
*  AUTO SOFTWARE DEVELOPMENT KIT AND ARE RELEASED AS
*  OPEN SOURCE FILES. THESE FILES MAY BE USED AS LONG
*  AS THE DEVELOPER AGREES TO THE LICENSE THAT IS
*  PROVIDED WITH THIS PACKAGE.
*
*********************************************************/

#include "CLuaArguments.h"
#include <assert.h>

CLuaArguments::CLuaArguments ( const CLuaArguments& Arguments )
{
    // Copy all the arguments
    vector < CLuaArgument* > ::const_iterator iter = Arguments.m_Arguments.begin ();
    for ( ; iter != Arguments.m_Arguments.end (); iter++ )
    {
        CLuaArgument* pArgument = new CLuaArgument ( **iter );
        m_Arguments.push_back ( pArgument );
    }
}


const CLuaArguments& CLuaArguments::operator = ( const CLuaArguments& Arguments )
{
    // Clear our previous list if any
    DeleteArguments ();

    // Copy all the arguments
    vector < CLuaArgument* > ::const_iterator iter = Arguments.m_Arguments.begin ();
    for ( ; iter != Arguments.m_Arguments.end (); iter++ )
    {
        CLuaArgument* pArgument = new CLuaArgument ( **iter );
        m_Arguments.push_back ( pArgument );
    }

    // Return the given reference allowing for chaining
    return Arguments;
}


void CLuaArguments::ReadArguments ( lua_State* luaVM, unsigned int uiIndexBegin )
{
    // Delete the previous arguments if any
    DeleteArguments ();

    // Start reading arguments until there are none left
    while ( lua_type ( luaVM, uiIndexBegin ) != LUA_TNONE )
    {
        // Create an argument, let it read out the argument and add it to our vector
        CLuaArgument* pArgument = new CLuaArgument ( luaVM, uiIndexBegin++ );
        m_Arguments.push_back ( pArgument );
    }
}


void CLuaArguments::PushArguments ( lua_State* luaVM ) const
{
    // Push all our arguments
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end (); iter++ )
    {
        (*iter)->Push ( luaVM );
    }
}


void CLuaArguments::PushArguments ( CLuaArguments& Arguments )
{
    vector < CLuaArgument* > ::const_iterator iter = Arguments.IterBegin ();
    for ( ; iter != Arguments.IterEnd (); iter++ )
    {
        CLuaArgument* pArgument = new CLuaArgument ( **iter );
        m_Arguments.push_back ( pArgument );
    }
}


bool CLuaArguments::Call ( lua_State* luaVM, const char* szFunction ) const
{
    assert ( szFunction );

    // Add the function name to the stack and get the event from the table
    assert ( luaVM );

    lua_pushstring ( luaVM, szFunction );
    lua_gettable ( luaVM, LUA_GLOBALSINDEX );

    // Push our arguments onto the stack
    PushArguments ( luaVM );

    int iret = lua_pcall ( luaVM, m_Arguments.size (), 0, 0 ) ;
    if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
    {
        const char* szRes = lua_tostring( luaVM, -1 );
        return false; // the function call failed
    }
        
    return true;
}


CLuaArgument* CLuaArguments::PushNil ( void )
{
    CLuaArgument* pArgument = new CLuaArgument;
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushBoolean ( bool bBool )
{
    CLuaArgument* pArgument = new CLuaArgument ( bBool );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushNumber ( double dNumber )
{
    CLuaArgument* pArgument = new CLuaArgument ( dNumber );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushString ( const char* szString )
{
    CLuaArgument* pArgument = new CLuaArgument ( szString );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushUserData ( void* pUserData )
{
    CLuaArgument* pArgument = new CLuaArgument ( pUserData );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


void CLuaArguments::DeleteArguments ( void )
{
    // Delete each item
    vector < CLuaArgument* > ::iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the vector
    m_Arguments.clear ();
}