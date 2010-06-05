/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGUIManager.cpp
*  PURPOSE:     GUI entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientGUIManager::CClientGUIManager ( void )
{
    m_bCanRemoveFromList = true;
}


CClientGUIManager::~CClientGUIManager ( void )
{
    // Destroy all objects
    DeleteAll ();
}


void CClientGUIManager::DeleteAll ( void )
{
    // We must make sure the gui elements don't interrupt our list
    m_bCanRemoveFromList = false;

    // Delete all the gui elements
    list < CClientGUIElement* > ::const_iterator iter = m_Elements.begin ();
    for ( ; iter != m_Elements.end (); iter++ )
    {
		delete *iter;
    }

    // Clear the list
    m_Elements.clear ();
    m_bCanRemoveFromList = true;
}


bool CClientGUIManager::Exists ( CClientGUIElement* pGUIElement )
{
	if ( pGUIElement ) {
		// Find the object in the list
		list < CClientGUIElement* > ::const_iterator iter = m_Elements.begin ();
		for ( ; iter != m_Elements.end (); iter++ )
		{
			if ( *iter == pGUIElement )
			{
				return true;
			}
		}
	}

    // Doesn't exist
    return false;
}


bool CClientGUIManager::Exists ( CGUIElement* pCGUIElement )
{
	if ( pCGUIElement ) {
		// Find the object in the list
		list < CClientGUIElement* > ::const_iterator iter = m_Elements.begin ();
		for ( ; iter != m_Elements.end (); iter++ )
		{
			if ( (*iter)->GetCGUIElement () == pCGUIElement )
			{
				return true;
			}
		}
	}

    // Doesn't exist
    return false;
}


CClientGUIElement* CClientGUIManager::Get ( CGUIElement* pCGUIElement )
{
	if ( pCGUIElement ) {
		// Find the object in the list
		list < CClientGUIElement* > ::const_iterator iter = m_Elements.begin ();
		for ( ; iter != m_Elements.end (); iter++ )
		{
			if ( (*iter)->GetCGUIElement () == pCGUIElement )
			{
				return *iter;
			}
		}
	}

    // Doesn't exist
    return NULL;
}


void CClientGUIManager::Add ( CClientGUIElement* pElement )
{
	m_Elements.push_back ( pElement );
}


void CClientGUIManager::Remove ( CClientGUIElement* pGUIElement )
{
    if ( m_bCanRemoveFromList )
    {
        m_Elements.remove ( pGUIElement );
    }
}
