/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaArguments.cpp
*  PURPOSE:     Lua arguments manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

#ifndef VERIFY_ENTITY
#define VERIFY_ENTITY(entity) (CStaticFunctionDefinitions::GetRootElement()->IsMyChild(entity,true)&&!entity->IsBeingDeleted())
#endif
 
extern CClientGame* g_pClientGame;

CLuaArguments::CLuaArguments ( NetBitStreamInterface& bitStream )
{
	ReadFromBitStream ( bitStream );
}

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

void CLuaArguments::ReadTable ( lua_State* luaVM, signed int uiIndexBegin, unsigned int depth )
{
    // Delete the previous arguments if any
    DeleteArguments ();
    lua_pushnil(luaVM);  /* first key */
    while (lua_next(luaVM, uiIndexBegin) != 0) {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        CLuaArgument* pArgument = new CLuaArgument ( luaVM, -2, depth );
        m_Arguments.push_back ( pArgument ); // push the key first

        pArgument = new CLuaArgument ( luaVM, -1, depth );
        m_Arguments.push_back ( pArgument ); // then the value
       
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(luaVM, 1);
    }
}

void CLuaArguments::ReadArgument ( lua_State* luaVM, unsigned int uiIndex )
{
    CLuaArgument* pArgument = new CLuaArgument ( luaVM, uiIndex );
    m_Arguments.push_back ( pArgument );
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

void CLuaArguments::PushAsTable ( lua_State* luaVM )
{
    lua_newtable ( luaVM );
    if ( !m_Arguments.empty () )
    {
        vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
        for ( ; iter != m_Arguments.end (); iter ++ )
        {
            (*iter)->Push ( luaVM );
            iter++;
            (*iter)->Push ( luaVM );
            lua_settable ( luaVM, -3 );
        }
    }
}

bool CLuaArguments::Call ( CLuaMain* pLuaMain, int iLuaFunction, CLuaArguments * returnValues ) const
{
    assert ( pLuaMain );

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine ();
    assert ( luaVM );
    int luaStackPointer = lua_gettop ( luaVM );
	lua_getref ( luaVM, iLuaFunction );

    // Push our arguments onto the stack
    PushArguments ( luaVM );

    // Call the function with our arguments
    pLuaMain->ResetInstructionCount ();

    int iret = lua_pcall ( luaVM, m_Arguments.size (), LUA_MULTRET, 0 );
    if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
    {
        const char* szRes = lua_tostring( luaVM, -1 );		
		g_pClientGame->GetScriptDebugging()->LogError ( luaVM, "%s", szRes );

        return false; // the function call failed
    }
    else if ( returnValues != NULL )
    {
        int iReturns = lua_gettop ( luaVM ) - luaStackPointer;
        for ( int i = - iReturns; i <= -1; i++ )
        {
            returnValues->ReadArgument ( luaVM, i );
        }

        for ( int h = 0; h < iReturns; h++)
		{
            lua_pop ( luaVM, 1 );
		}
    }
        
    return true;
}


bool CLuaArguments::CallGlobal ( CLuaMain* pLuaMain, const char* szFunction, CLuaArguments * returnValues ) const
{
    assert ( pLuaMain );
    assert ( szFunction );

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine ();
    assert ( luaVM );
    int luaStackPointer = lua_gettop ( luaVM );
    lua_pushstring ( luaVM, szFunction );
    lua_gettable ( luaVM, LUA_GLOBALSINDEX );

    // Push our arguments onto the stack
    PushArguments ( luaVM );

    // Call the function with our arguments
    pLuaMain->ResetInstructionCount ();

    int iret = lua_pcall ( luaVM, m_Arguments.size (), LUA_MULTRET, 0 );
    if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
    {
        const char* szRes = lua_tostring( luaVM, -1 );
        g_pClientGame->GetScriptDebugging()->LogError ( luaVM, "%s", szRes );

        return false; // the function call failed
    }
    else if ( returnValues != NULL )
    {
        int iReturns = lua_gettop ( luaVM ) - luaStackPointer;
        for ( int i = - iReturns; i <= -1; i++ )
        {
            returnValues->ReadArgument ( luaVM, i );
        }

        for ( int h = 0; h < iReturns; h++)
		{
            lua_pop ( luaVM, 1 );
		}
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


CLuaArgument* CLuaArguments::PushElement ( CClientEntity* pElement )
{
    CLuaArgument* pArgument = new CLuaArgument ( pElement );
    m_Arguments.push_back ( pArgument );
    return pArgument;
}


CLuaArgument* CLuaArguments::PushArgument ( CLuaArgument& Argument )
{
    CLuaArgument* pArgument = new CLuaArgument ( Argument );
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


bool CLuaArguments::ReadFromBitStream ( NetBitStreamInterface& bitStream )
{
    unsigned short usNumArgs;
    if ( bitStream.Read ( usNumArgs ) )
    {
        for ( unsigned short us = 0 ; us < usNumArgs ; us++ )
        {
		    CLuaArgument* pArgument = new CLuaArgument;
            pArgument->ReadFromBitStream ( bitStream );
            m_Arguments.push_back ( pArgument );
	    }
    }
    return true;
}


bool CLuaArguments::WriteToBitStream ( NetBitStreamInterface& bitStream )
{
    bool bSuccess = true;
    bitStream.Write ( static_cast < unsigned short > ( m_Arguments.size () ) );
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () ; iter++ )
    {
        CLuaArgument* pArgument = *iter;
        if ( !pArgument->WriteToBitStream ( bitStream ) )
        {
            bSuccess = false;
        }
    }
    return bSuccess;
}