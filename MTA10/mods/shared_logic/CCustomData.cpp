/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CCustomData.cpp
*  PURPOSE:     Custom data storage class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CCustomData::~CCustomData ( void )
{
    DeleteAll ();
}


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


void CCustomData::Set ( const char* szName, const CLuaArgument& Variable, class CLuaMain* pLuaMain )
{
    assert ( szName );
    //assert ( pLuaMain );

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
        pData->szName [ MAX_CUSTOMDATA_NAME_LENGTH ] = 0;
        strncpy ( pData->szName, szName, MAX_CUSTOMDATA_NAME_LENGTH );
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
			// Delete the item
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
    // HACK: no idea why its crashing here
    if ( m_List.size () <= 0 )
        return;

    // Delete all the items
    list < SCustomData* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
}