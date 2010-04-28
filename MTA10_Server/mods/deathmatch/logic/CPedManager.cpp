/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPedManager.cpp
*  PURPOSE:     Ped entity manager class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPedManager::CPedManager ( void )
{
    m_bDontRemoveFromList = false;
}


CPedManager::~CPedManager ( void )
{
    DeleteAll ();
}


CPed* CPedManager::Create ( unsigned short usModel, CElement* pParent, CXMLNode* pNode )
{
    // Create the ped
    CPed* pPed = new CPed ( this, pParent, pNode, usModel );

    // Invalid ped id?
    if ( pPed->GetID () == INVALID_ELEMENT_ID )
    {
        delete pPed;
        return NULL;
    }

    // Return the created ped
    return pPed;
}


CPed* CPedManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    // Create the Ped
    CPed* pPed = new CPed ( this, pParent, &Node, 400 );

    // Verify the Ped id and load the data from xml
    if ( pPed->GetID () == INVALID_ELEMENT_ID ||
         !pPed->LoadFromCustomData ( pLuaMain, pEvents ) )
    {
        delete pPed;
        return NULL;
    }

    // Make sure hes alive (leave to scripts?)
    pPed->SetIsDead ( false );
    pPed->SetSpawned ( true );
    pPed->SetHealth ( 100.0f );

    // Return the created Ped
    return pPed;
}


void CPedManager::DeleteAll ( void )
{
    // Delete all items
    m_bDontRemoveFromList = true;
    list < CPed* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
    m_bDontRemoveFromList = false;
}


void CPedManager::RemoveFromList ( CPed* pPed )
{
    if ( !m_bDontRemoveFromList && !m_List.empty() )
    {
        m_List.remove ( pPed );
    }
}


bool CPedManager::Exists ( CPed* pPed )
{
    // Try to find the Ped ID in the list
    list < CPed* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( (*iter) == pPed )
        {
            return true;
        }
    }

    // Couldn't find it
    return false;
}


bool CPedManager::IsValidModel ( unsigned short usModel )
{
    return true;
}
