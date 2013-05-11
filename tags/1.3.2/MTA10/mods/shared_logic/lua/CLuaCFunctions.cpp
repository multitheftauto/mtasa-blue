/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctions.cpp
*  PURPOSE:     Lua functions class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

static std::vector < CLuaCFunction* > m_sFunctions;
static std::map < lua_CFunction, CLuaCFunction* > ms_Functions;

CLuaCFunction::CLuaCFunction ( const char* szName, lua_CFunction f, bool bRestricted )
{
    m_strName = szName;
    m_Function = f;
    m_bRestricted = bRestricted;
}


CLuaCFunction* CLuaCFunctions::AddFunction ( const char* szName, lua_CFunction f, bool bRestricted )
{
    // See if we already have it added. Eventually just return it instead of adding it twice
    CLuaCFunction* pFunction = GetFunction ( szName, f );
    if ( pFunction ) return pFunction;

    // Create it, add to list and return
    pFunction = new CLuaCFunction ( szName, f, bRestricted );
    m_sFunctions.push_back ( pFunction );
    MapSet ( ms_Functions, f, pFunction );
    return pFunction;
}


CLuaCFunction* CLuaCFunctions::GetFunction ( const char* szName, lua_CFunction f )
{
    // Grab a function of the given name and type
    std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions.begin ();
    for ( ; iter != m_sFunctions.end (); iter++ )
    {
        if ( (*iter)->GetFunctionAddress () == f &&
             strcmp ( (*iter)->GetFunctionName (), szName ) == 0 )
        {
            return *iter;
        }
    }

    return NULL;
}


CLuaCFunction* CLuaCFunctions::GetFunction ( lua_CFunction f )
{
    return MapFindRef ( ms_Functions, f );
}


const char* CLuaCFunctions::GetFunctionName ( lua_CFunction f, bool& bRestricted )
{
    // Return the function name of the given C address
    std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions.begin ();
    for ( ; iter != m_sFunctions.end (); iter++ )
    {
        if ( (*iter)->GetFunctionAddress () == f )
        {
            bRestricted = (*iter)->IsRestricted ();
            return (*iter)->GetFunctionName ();
        }
    }

    bRestricted = false;
    return NULL;
}


bool CLuaCFunctions::IsRestricted ( const char* szName )
{
    // Grab a function of the given name and type
    std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions.begin ();
    for ( ; iter != m_sFunctions.end (); iter++ )
    {
        if ( strcmp ( (*iter)->GetFunctionName (), szName ) == 0 )
        {
            return (*iter)->IsRestricted ();
        }
    }

    // Not restricted by MTA atleast
    return false;
}


void CLuaCFunctions::RegisterFunctionsWithVM ( lua_State* luaVM )
{
    // Register all our functions to a lua VM
    std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions.begin ();
    for ( ; iter != m_sFunctions.end (); iter++ )
    {
        lua_pushstring ( luaVM, (*iter)->GetFunctionName () );
        lua_pushcclosure ( luaVM, (*iter)->GetFunctionAddress (), 1 );
        lua_setglobal ( luaVM, (*iter)->GetFunctionName () );
    }
}


void CLuaCFunctions::RemoveAllFunctions ( void )
{
    // Delete all functions
    std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions.begin ();
    for ( ; iter != m_sFunctions.end (); iter++ )
    {
        delete *iter;
    }

    m_sFunctions.clear ();
}
