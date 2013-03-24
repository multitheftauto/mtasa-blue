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
    m_WorldWaterLevelInfo.bNonSeaLevelSet = false;
    m_WorldWaterLevelInfo.fSeaLevel = 0;
    m_WorldWaterLevelInfo.fNonSeaLevel = 0;
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


void CWaterManager::SetElementWaterLevel ( CWater* pWater, float fLevel )
{
    pWater->SetLevel ( fLevel );
}


void CWaterManager::SetAllElementWaterLevel ( float fLevel )
{
    std::list < CWater* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        SetElementWaterLevel ( *iter, fLevel );
    }
}


void CWaterManager::SetWorldWaterLevel ( float fLevel, bool bIncludeWorldNonSeaLevel )
{
    m_WorldWaterLevelInfo.fSeaLevel = fLevel;
    if ( bIncludeWorldNonSeaLevel )
    {
        m_WorldWaterLevelInfo.bNonSeaLevelSet = true;
        m_WorldWaterLevelInfo.fNonSeaLevel = fLevel;
    }
}


void CWaterManager::ResetWorldWaterLevel ( void )
{
    m_WorldWaterLevelInfo.bNonSeaLevelSet = false;
    m_WorldWaterLevelInfo.fSeaLevel = 0;
    m_WorldWaterLevelInfo.fNonSeaLevel = 0;
}


void CWaterManager::DeleteAll ()
{
    // Delete all items
    DeletePointersAndClearList ( m_List );
}


void CWaterManager::RemoveFromList ( CWater* pWater )
{
    m_List.remove ( pWater );
}


bool CWaterManager::Exists ( CWater* pWater )
{
    return ListContains ( m_List, pWater );
}
