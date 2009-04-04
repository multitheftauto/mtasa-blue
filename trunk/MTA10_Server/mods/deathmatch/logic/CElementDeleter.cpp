/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementDeleter.cpp
*  PURPOSE:     Element lazy deletion manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;

void CElementDeleter::Delete ( class CElement* pElement, bool bUnlink )
{
	if ( pElement && !IsBeingDeleted ( pElement ) )
    {
		// Before we do anything, fire the on-destroy event
        CLuaArguments Arguments;
        pElement->CallEvent ( "onElementDestroy", Arguments );

		// Add it to our list
		if ( !pElement->IsBeingDeleted () )
		{
			m_List.push_back ( pElement );
		}

		// Flag it as being deleted and unlink it from the tree/managers
		pElement->SetIsBeingDeleted ( true );
        pElement->ClearChildren ();
        pElement->SetParentObject ( NULL );

        if ( bUnlink )
		    pElement->Unlink ();
	}
}


void CElementDeleter::DoDeleteAll ( void )
{
    // Delete all the elements
    list < CElement* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
}


bool CElementDeleter::IsBeingDeleted ( CElement* pElement )
{
    list < CElement* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( pElement == *iter )
        {
            return true;
        }
    }

    return false;
}
