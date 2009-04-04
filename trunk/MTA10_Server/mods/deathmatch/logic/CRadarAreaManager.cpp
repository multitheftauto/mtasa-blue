/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRadarAreaManager.cpp
*  PURPOSE:     Radar area entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CRadarAreaManager::CRadarAreaManager ( void )
{
    m_bDontRemoveFromList = false;
}


CRadarAreaManager::~CRadarAreaManager ( void )
{
    // Delete all the radar areas
    DeleteAll ();
}


CRadarArea* CRadarAreaManager::Create ( CElement* pParent, CXMLNode* pNode )
{
    // Create the radar area
    CRadarArea* pArea = new CRadarArea ( this, pParent, pNode );

    // If the id was invalid, delete it and return NULL
    if ( pArea->GetID () == INVALID_ELEMENT_ID )
    {
        delete pArea;
        return NULL;
    }

    // Otherwize return it
    return pArea;
}


CRadarArea* CRadarAreaManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    // Create the radar area
    CRadarArea* pArea = new CRadarArea ( this, pParent, &Node );

    // If the id was invalid, delete it and return NULL
    if ( pArea->GetID () == INVALID_ELEMENT_ID ||
         !pArea->LoadFromCustomData ( pLuaMain, pEvents ) )
    {
        delete pArea;
        return NULL;
    }

    // Otherwize return it
    return pArea;
}


void CRadarAreaManager::DeleteAll ( void )
{
    // Delete all the radar areas
    m_bDontRemoveFromList = true;
    list < CRadarArea* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear ();
}


bool CRadarAreaManager::Exists ( CRadarArea* pArea )
{
    // See if it exists. If, return true
    list < CRadarArea* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pArea )
        {
            return true;
        }
    }

    // Doesn't exist
    return false;
}


void CRadarAreaManager::RemoveFromList ( CRadarArea* pArea )
{
    if ( !m_bDontRemoveFromList && !m_List.empty() )
    {
        m_List.remove ( pArea );
    }
}
