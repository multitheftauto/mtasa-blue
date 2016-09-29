/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWaterManager.cpp
*  PURPOSE:     Water entity manager class
*  DEVELOPERS:  arc_
*
*****************************************************************************/

#include "StdInc.h"

CClientWaterManager::CClientWaterManager ( CClientManager* pManager )
{
    m_pManager = pManager;
    m_bDontRemoveFromList = false;
}


CClientWaterManager::~CClientWaterManager ( void )
{
    DeleteAll ();
}


CClientWater* CClientWaterManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTWATER )
    {
        return static_cast < CClientWater* > ( pEntity );
    }

    return nullptr;
}


void CClientWaterManager::DeleteAll ( void )
{
    // Delete each water poly
    m_bDontRemoveFromList = true;
    for (auto& pWater : m_List)
    {
        delete pWater;
    }

    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear ();
}


bool CClientWaterManager::Exists ( CClientWater* pWater )
{
    return std::find(m_List.begin(), m_List.end(), pWater) != m_List.end();
}

void CClientWaterManager::AddToList ( CClientWater* pWater )
{
    m_List.push_back ( pWater );
}

void CClientWaterManager::RemoveFromList ( CClientWater* pWater )
{
    if ( !m_bDontRemoveFromList )
    {
        if ( !m_List.empty() ) 
            m_List.remove ( pWater );
    }
}

bool CClientWaterManager::GetWaterLevel ( CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown )
{
    return g_pGame->GetWaterManager ()->GetWaterLevel ( vecPosition, pfLevel, bCheckWaves, pvecUnknown );
}

bool CClientWaterManager::SetPositionWaterLevel ( const CVector& vecPosition, float fLevel, void* pChangeSource )
{
    return g_pGame->GetWaterManager ()->SetPositionWaterLevel ( vecPosition, fLevel, pChangeSource );
}

bool CClientWaterManager::SetWorldWaterLevel ( float fLevel, void* pChangeSource, bool bIncludeWorldNonSeaLevel )
{
    return g_pGame->GetWaterManager ()->SetWorldWaterLevel ( fLevel, pChangeSource, bIncludeWorldNonSeaLevel );
}

bool CClientWaterManager::SetAllElementWaterLevel ( float fLevel, void* pChangeSource )
{
    for (auto& pWater : m_List )
    {
        pWater->SetLevel ( fLevel, pChangeSource );
    }
    return true;
}


void CClientWaterManager::ResetWorldWaterLevel ( void )
{
    g_pGame->GetWaterManager ()->ResetWorldWaterLevel ();
}

float CClientWaterManager::GetWaveLevel ()
{
    return g_pGame->GetWaterManager ()->GetWaveLevel ();
}

void CClientWaterManager::SetWaveLevel ( float fWaveLevel )
{
    g_pGame->GetWaterManager ()->SetWaveLevel ( fWaveLevel );
}

void CClientWaterManager::SetDimension(unsigned short usDimension)
{
    m_usDimension = usDimension;
    for (auto& pWater : m_List)
    {
        pWater->RelateDimension(m_usDimension);
    }
}
