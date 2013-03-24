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
    DeletePointersAndClearList ( m_List );
}


bool CRadarAreaManager::Exists ( CRadarArea* pArea )
{
    return ListContains ( m_List, pArea );
}


void CRadarAreaManager::RemoveFromList ( CRadarArea* pArea )
{
    m_List.remove ( pArea );
}
