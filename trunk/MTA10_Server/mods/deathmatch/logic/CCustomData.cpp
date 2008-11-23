/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCustomData.cpp
*  PURPOSE:     Custom entity data class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CCustomData::Copy ( CCustomData* pCustomData )
{
    list < SCustomData* > ::const_iterator iter = pCustomData->IterBegin ();
    for ( ; iter != pCustomData->IterEnd (); iter++ )
    {
        Set ( (*iter)->szName, (*iter)->Variable, (*iter)->pLuaMain );
    }
}

SCustomData* CCustomData::Get ( const char* szName )
{
    assert ( szName );

    // Try finding the name in our list
    list < SCustomData* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Names matches?
        if ( strcmp ( (*iter)->szName, szName ) == 0 )
        {
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}

// User-defined warnings
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : warning C0000 *MTA Developers*: "

void CCustomData::Set ( const char* szName, const CLuaArgument& Variable, class CLuaMain* pLuaMain )
{
	assert ( szName );

    // Grab the item with the given name
    SCustomData* pData = Get ( szName );
    if ( pData )
    {
        // Set the variable and eventually its new owner
        pData->Variable = Variable;
        pData->pLuaMain = pLuaMain;
    }
    else
    {
        // Add it and set the stuff
        pData = new SCustomData;
        m_List.push_back ( pData );
        strncpy ( pData->szName, szName, MAX_CUSTOMDATA_NAME_LENGTH );
        pData->szName [MAX_CUSTOMDATA_NAME_LENGTH] = 0;
        pData->Variable = Variable;
        pData->pLuaMain = pLuaMain;
    }
}


bool CCustomData::Delete ( const char* szName )
{
    // Find the item and delete it
    SCustomData* pData = Get ( szName );
    if ( pData )
    {
        if ( !m_List.empty() ) m_List.remove ( pData );
        delete pData;

        // Success
        return true;
    }

    // Didn't exist
    return false;
}


void CCustomData::DeleteAll ( class CLuaMain* pLuaMain )
{
    // Delete any items with matching VM's
    list < SCustomData* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Delete it if they match
        if ( (*iter)->pLuaMain == pLuaMain )
        {
			// Delete the object
            delete *iter;

			// Remove from list
			m_List.erase ( iter );

			// Continue from the beginning, unless the list is empty
			if ( m_List.empty () ) break;
			iter = m_List.begin ();
        }
    }
}


void CCustomData::DeleteAll ( void )
{
    // Delete all the items
    list < SCustomData* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
}

CXMLNode * CCustomData::OutputToXML ( CXMLNode * pNode )
{
    list < SCustomData* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        CLuaArgument * arg = &(*iter)->Variable;
        
        switch ( arg->GetType() )
        {
        case LUA_TSTRING:
            {
                CXMLAttribute * attr = pNode->GetAttributes().Create( (*iter)->szName );
                attr->SetValue ( arg->GetString ().c_str () );
                break;
            }
        case LUA_TNUMBER:
            {
                CXMLAttribute * attr = pNode->GetAttributes().Create( (*iter)->szName );
                attr->SetValue ( (float)arg->GetNumber () );
                break;
            }
        case LUA_TBOOLEAN:
            {
                CXMLAttribute * attr = pNode->GetAttributes().Create( (*iter)->szName );
                attr->SetValue ( arg->GetBoolean () );
                break;
            }
        }
    }
    return pNode;   
}

