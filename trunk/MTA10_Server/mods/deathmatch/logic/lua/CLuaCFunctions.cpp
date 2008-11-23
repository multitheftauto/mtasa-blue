/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		mods/deathmatch/logic/lua/CLuaCFunctions.cpp
*  PURPOSE:		Lua C-function extension class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Alberto Alonso <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define FUNCTIONVECTOR_BUCKETS      256
#define FUNCTIONVECTOR_BUCKETMASK   255

#define GET_LUACFUNCTION_ADDR(f) ((unsigned long)f)

static std::vector < CLuaCFunction* > m_sFunctions[FUNCTIONVECTOR_BUCKETS];

CLuaCFunction::CLuaCFunction ( const char* szName, lua_CFunction f, bool bRestricted )
{
    m_strName = szName ? szName : "";
    m_Function = f;
    m_bRestricted = bRestricted;
}


CLuaCFunction* CLuaCFunctions::AddFunction ( const char* szName, lua_CFunction f, bool bRestricted )
{
    // See if we already have it added. Eventually just return it instead of adding it twice
    CLuaCFunction* pFunction = GetFunction ( szName, f );
    if ( pFunction ) return pFunction;

    // Create it, add to list and return
    unsigned long ulBucket = GET_LUACFUNCTION_ADDR(f) & FUNCTIONVECTOR_BUCKETMASK;
    pFunction = new CLuaCFunction ( szName, f, bRestricted );
    m_sFunctions[ ulBucket ].push_back ( pFunction );
    return pFunction;
}


CLuaCFunction* CLuaCFunctions::GetFunction ( const char* szName, lua_CFunction f )
{
    // Grab a function of the given name and type
    unsigned long ulBucket = GET_LUACFUNCTION_ADDR(f) & FUNCTIONVECTOR_BUCKETMASK;
    std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions[ ulBucket ].begin();
    std::vector < CLuaCFunction* > ::const_iterator & iterEnd = m_sFunctions[ ulBucket ].end();
    for ( ; iter != iterEnd; iter++ )
    {
        if ( (*iter)->GetFunctionAddress () == f &&
             strcmp ( (*iter)->GetFunctionName ().c_str (), szName ) == 0 )
        {
            return *iter;
        }
    }

    return NULL;
}


bool CLuaCFunctions::GetFunctionName ( lua_CFunction f, bool& bRestricted, std::string &strName )
{
    // Return the function name of the given C address
    unsigned long ulBucket = GET_LUACFUNCTION_ADDR(f) & FUNCTIONVECTOR_BUCKETMASK;
    std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions[ ulBucket ].begin();
    std::vector < CLuaCFunction* > ::const_iterator & iterEnd = m_sFunctions[ ulBucket ].end();

    for ( ; iter != iterEnd; iter++ )
    {
        if ( (*iter)->GetFunctionAddress () == f )
        {
            bRestricted = (*iter)->IsRestricted ();
            strName = (*iter)->GetFunctionName ();
            return true;
        }
    }

    bRestricted = false;
    return false;
}


bool CLuaCFunctions::IsRestricted ( const char* szName )
{
    // Grab a function of the given name and type
    for ( unsigned long i = 0 ; i < FUNCTIONVECTOR_BUCKETS ; i++ )
    {
        std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions[i].begin ();
        std::vector < CLuaCFunction* > ::const_iterator & iterEnd = m_sFunctions[i].end ();
        for ( ; iter != iterEnd; iter++ )
        {
            if ( strcmp ( (*iter)->GetFunctionName ().c_str (), szName ) == 0 )
            {
                return (*iter)->IsRestricted ();
            }
        }
    }

    // Not restricted by MTA atleast
    return false;
}


void CLuaCFunctions::RegisterFunctionsWithVM ( lua_State* luaVM )
{
    // Register all our functions to a lua VM
    for ( unsigned long i = 0 ; i < FUNCTIONVECTOR_BUCKETS ; i++ )
    {
        std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions[i].begin ();
        std::vector < CLuaCFunction* > ::const_iterator & iterEnd = m_sFunctions[i].end ();
        for ( ; iter != iterEnd; iter++ )
        {
            lua_register ( luaVM, (*iter)->GetFunctionName ().c_str (), (*iter)->GetFunctionAddress () );
        }
    }
}


void CLuaCFunctions::RemoveAllFunctions ( void )
{
    // Delete all functions
    for ( unsigned long i = 0 ; i < FUNCTIONVECTOR_BUCKETS ; i++ )
    {
        std::vector < CLuaCFunction* > ::const_iterator iter = m_sFunctions[i].begin ();
        for ( ; iter != m_sFunctions[i].end (); iter++ )
        {
            delete *iter;
        }

        m_sFunctions[i].clear ();
    }
}
