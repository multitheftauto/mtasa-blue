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

#include "StdInc.h"

using namespace std;

#ifndef VERIFY_ENTITY
#define VERIFY_ENTITY(entity) (CStaticFunctionDefinitions::GetRootElement()->IsMyChild(entity,true)&&!entity->IsBeingDeleted())
#endif
 
extern CClientGame* g_pClientGame;

CLuaArguments::CLuaArguments ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables )
{
    ReadFromBitStream ( bitStream, pKnownTables );
}


CLuaArguments::CLuaArguments ( const CLuaArguments& Arguments, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables )
{
    // Copy all the arguments
    CopyRecursive ( Arguments, pKnownTables );
}


CLuaArgument* CLuaArguments::operator [] ( const unsigned int uiPosition ) const
{
	if ( uiPosition < m_Arguments.size () )
		return m_Arguments.at ( uiPosition );
	return NULL;
}


const CLuaArguments& CLuaArguments::operator = ( const CLuaArguments& Arguments )
{
    CopyRecursive ( Arguments );

    // Return the given reference allowing for chaining
    return Arguments;
}


void CLuaArguments::CopyRecursive ( const CLuaArguments& Arguments, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables )
{
    // Clear our previous list if any
    DeleteArguments ();

    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < CLuaArguments*, CLuaArguments* > ();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert ( std::make_pair ( (CLuaArguments *)&Arguments, (CLuaArguments *)this ) );

    // Copy all the arguments
    vector < CLuaArgument* > ::const_iterator iter = Arguments.m_Arguments.begin ();
    for ( ; iter != Arguments.m_Arguments.end (); iter++ )
    {
        CLuaArgument* pArgument = new CLuaArgument ( **iter, pKnownTables );
        m_Arguments.push_back ( pArgument );
    }

    if ( bKnownTablesCreated )
        delete pKnownTables;
}

void CLuaArguments::ReadArguments ( lua_State* luaVM, signed int uiIndexBegin )
{
    // Delete the previous arguments if any
    DeleteArguments ();

    std::map < const void*, CLuaArguments* > knownTables;

    // Start reading arguments until there are none left
    while ( lua_type ( luaVM, uiIndexBegin ) != LUA_TNONE )
    {
        // Create an argument, let it read out the argument and add it to our vector
        CLuaArgument* pArgument = new CLuaArgument ( luaVM, uiIndexBegin++, &knownTables );
        m_Arguments.push_back ( pArgument );
    }
}

void CLuaArguments::ReadTable ( lua_State* luaVM, int iIndexBegin, std::map < const void*, CLuaArguments* > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < const void*, CLuaArguments* > ();
        bKnownTablesCreated = true;
    }
    pKnownTables->insert ( std::make_pair ( lua_topointer(luaVM, iIndexBegin), this ) );

    // Delete the previous arguments if any
    DeleteArguments ();

    lua_pushnil(luaVM);  /* first key */
    if ( iIndexBegin < 0 )
        iIndexBegin--;

    while (lua_next(luaVM, iIndexBegin) != 0) {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        CLuaArgument* pArgument = new CLuaArgument ( luaVM, -2, pKnownTables );
        m_Arguments.push_back ( pArgument ); // push the key first

        pArgument = new CLuaArgument ( luaVM, -1, pKnownTables );
        m_Arguments.push_back ( pArgument ); // then the value
       
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(luaVM, 1);
    }

    if ( bKnownTablesCreated )
        delete pKnownTables;
}

void CLuaArguments::ReadArgument ( lua_State* luaVM, int iIndex )
{
    CLuaArgument* pArgument = new CLuaArgument ( luaVM, iIndex );
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

void CLuaArguments::PushAsTable ( lua_State* luaVM, std::map < CLuaArguments*, int > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < CLuaArguments*, int > ();
        bKnownTablesCreated = true;
    }

    lua_newtable ( luaVM );
    pKnownTables->insert ( std::make_pair ( (CLuaArguments *)this, lua_gettop(luaVM) ) );
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () && (iter+1) != m_Arguments.end (); iter ++ )
    {
        (*iter)->Push ( luaVM, pKnownTables ); // index
        iter++;
        (*iter)->Push ( luaVM, pKnownTables ); // value
        lua_settable ( luaVM, -3 );
    }

    if ( bKnownTablesCreated )
        delete pKnownTables;
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


CLuaArgument* CLuaArguments::PushTable ( CLuaArguments * table )
{
    CLuaArgument* pArgument = new CLuaArgument (  );
    pArgument->Read(table);
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


bool CLuaArguments::ReadFromBitStream ( NetBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables )
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::vector < CLuaArguments* > ();
        bKnownTablesCreated = true;
    }

    unsigned short usNumArgs;
    if ( bitStream.Read ( usNumArgs ) )
    {
        pKnownTables->push_back ( this );
        for ( unsigned short us = 0 ; us < usNumArgs ; us++ )
        {
		    CLuaArgument* pArgument = new CLuaArgument ( bitStream, pKnownTables );
            m_Arguments.push_back ( pArgument );
        }
	}

    if ( bKnownTablesCreated )
        delete pKnownTables;

    return true;
}


bool CLuaArguments::WriteToBitStream ( NetBitStreamInterface& bitStream, std::map < CLuaArguments*, unsigned long > * pKnownTables ) const
{
    bool bKnownTablesCreated = false;
    if ( !pKnownTables )
    {
        pKnownTables = new std::map < CLuaArguments*, unsigned long > ();
        bKnownTablesCreated = true;
    }

    bool bSuccess = true;
    pKnownTables->insert ( make_pair ( (CLuaArguments *)this, pKnownTables->size () ) );
    bitStream.Write ( static_cast < unsigned short > ( m_Arguments.size () ) );
    vector < CLuaArgument* > ::const_iterator iter = m_Arguments.begin ();
    for ( ; iter != m_Arguments.end () ; iter++ )
    {
        CLuaArgument* pArgument = *iter;
        if ( !pArgument->WriteToBitStream ( bitStream, pKnownTables ) )
        {
            bSuccess = false;
        }
    }

    if ( bKnownTablesCreated )
        delete pKnownTables;

	return bSuccess;
}
