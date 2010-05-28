/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWaterManager.cpp
*  PURPOSE:     Water entity manager class
*  DEVELOPERS:  arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CWaterManager::CWaterManager ()
{
    m_bDontRemoveFromList = false;

    m_fGlobalWaterLevel = 0.0f;
    m_fGlobalWaveHeight = 0.0f;
}

CWaterManager::~CWaterManager ()
{
    DeleteAll ();
}

CWater* CWaterManager::Create ( CWater::EWaterType waterType, CElement* pParent, CXMLNode* pNode )
{
    CWater* pWater = new CWater ( this, pParent, pNode, waterType );
    if ( pWater->GetID () == INVALID_ELEMENT_ID )
    {
        delete pWater;
        return NULL;
    }
    return pWater;
}

CWater* CWaterManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    CWater* pWater = new CWater ( this, pParent, &Node );
    if ( pWater->GetID () == INVALID_ELEMENT_ID || !pWater->LoadFromCustomData ( pLuaMain, pEvents ) )
    {
        delete pWater;
        pWater = NULL;
    }
    return pWater;
}

void CWaterManager::SetGlobalWaterLevel ( float fLevel )
{
    m_fGlobalWaterLevel = fLevel;
    std::list < CWater* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        (*iter)->SetLevel ( fLevel );
    }
}

void CWaterManager::DeleteAll ()
{
    // Delete all items
    m_bDontRemoveFromList = true;
    std::list < CWater* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
    m_bDontRemoveFromList = false;
}


void CWaterManager::RemoveFromList ( CWater* pWater )
{
    if ( !m_bDontRemoveFromList && !m_List.empty() )
    {
        m_List.remove ( pWater );
    }
}


bool CWaterManager::Exists ( CWater* pWater )
{
    // Try to find the water ID in the list
    std::list < CWater* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( (*iter) == pWater )
        {
            return true;
        }
    }

    // Couldn't find it
    return false;
}
