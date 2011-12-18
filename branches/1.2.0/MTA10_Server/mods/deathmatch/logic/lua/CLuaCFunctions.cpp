/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaCFunctions.cpp
*  PURPOSE:     Lua C-function extension class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Alberto Alonso <rydencillo@gmail.com>
*               arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace google;

dense_hash_map < lua_CFunction, CLuaCFunction* > CLuaCFunctions::ms_Functions;
dense_hash_map < std::string, CLuaCFunction* > CLuaCFunctions::ms_FunctionsByName;
bool CLuaCFunctions::ms_bMapsInitialized = false;

CLuaCFunction::CLuaCFunction ( const char* szName, lua_CFunction f, bool bRestricted )
{
    m_strName = szName ? szName : "";
    m_Function = f;
    m_bRestricted = bRestricted;
}

CLuaCFunctions::CLuaCFunctions ()
{
}

CLuaCFunctions::~CLuaCFunctions ()
{
    RemoveAllFunctions ();
}

void CLuaCFunctions::InitializeHashMaps ( )
{
    if ( !ms_bMapsInitialized )
    {
        ms_bMapsInitialized = true;
        ms_Functions.set_empty_key ( (lua_CFunction)0x00000000 );
        ms_Functions.set_deleted_key ( (lua_CFunction)0xFFFFFFFF );
        ms_FunctionsByName.set_empty_key ( std::string ( "" ) );
        ms_FunctionsByName.set_deleted_key ( std::string ( "\xFF" ) );
    }
}

CLuaCFunction* CLuaCFunctions::AddFunction ( const char* szName, lua_CFunction f, bool bRestricted )
{
    // Already have a function by this name?
    CLuaCFunction* pFunction = GetFunction ( szName );
    if ( pFunction )
        return pFunction;

    // Already have a function by this address?
    pFunction = GetFunction ( f );
    if ( !pFunction )
    {
        pFunction = new CLuaCFunction ( szName, f, bRestricted );
        ms_Functions [ f ] = pFunction;
    }
    ms_FunctionsByName [ szName ] = pFunction;
    return pFunction;
}


CLuaCFunction* CLuaCFunctions::GetFunction ( lua_CFunction f )
{
    dense_hash_map < lua_CFunction, CLuaCFunction* >::iterator it;
    it = ms_Functions.find ( f );
    if ( it == ms_Functions.end () )
        return NULL;

    return it->second;
}


CLuaCFunction* CLuaCFunctions::GetFunction ( const char* szName )
{
    dense_hash_map < std::string, CLuaCFunction* >::iterator it;
    it = ms_FunctionsByName.find ( szName );
    if ( it == ms_FunctionsByName.end () )
        return NULL;

    return it->second;
}


void CLuaCFunctions::RegisterFunctionsWithVM ( lua_State* luaVM )
{
    // Register all our functions to a lua VM
    dense_hash_map < std::string, CLuaCFunction* >::iterator it;
    for ( it = ms_FunctionsByName.begin (); it != ms_FunctionsByName.end (); it++ )
    {
        lua_register ( luaVM, it->first.c_str (), it->second->GetAddress () );
    }
}


void CLuaCFunctions::RemoveAllFunctions ( void )
{
    // Delete all functions
    dense_hash_map < lua_CFunction, CLuaCFunction* >::iterator it;
    for ( it = ms_Functions.begin (); it != ms_Functions.end (); it++ )
    {
        delete it->second;
    }
    ms_Functions.clear ();
    ms_FunctionsByName.clear ();
}
