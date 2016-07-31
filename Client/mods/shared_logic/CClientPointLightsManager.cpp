/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPointLightsManager.cpp
*  PURPOSE:     PointLights entity manager class
*  DEVELOPERS:  Alexander Romanov <lexr128@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientPointLightsManager::CClientPointLightsManager ( CClientManager* pManager )
{
    // Init
    m_pManager = pManager;
    m_bDontRemoveFromList = false;
    m_usDimension = 0;
}


CClientPointLightsManager::~CClientPointLightsManager ( void )
{
    // Make sure all the lights are deleted
    DeleteAll ();
}


CClientPointLights* CClientPointLightsManager::Create ( ElementID EntityID )
{
    if ( !Get ( EntityID ) )
    {
        return new CClientPointLights ( m_pManager, EntityID );
    }

    return NULL;
}


void CClientPointLightsManager::Delete ( CClientPointLights* pLight )
{
    delete pLight;
}


void CClientPointLightsManager::DeleteAll ( void )
{
    // Delete all the lights
    m_bDontRemoveFromList = true;
    list < CClientPointLights* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    for ( auto pLight : m_SearchLightList )
    {
        delete pLight;
    }
    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear ();
    m_SearchLightList.clear ();
}

CClientPointLights* CClientPointLightsManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTPOINTLIGHTS )
    {
        return static_cast < CClientPointLights* > ( pEntity );
    }

    return NULL;
}


void CClientPointLightsManager::SetDimension ( unsigned short usDimension )
{
    list < CClientPointLights* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        (*iter)->RelateDimension ( usDimension );
    }

    m_usDimension = usDimension;
}


void CClientPointLightsManager::DoPulse ( void )
{
    // Pulse each light
    list < CClientPointLights* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); ++iter )
    {
        (*iter)->DoPulse ();
    }
}

void CClientPointLightsManager::RenderHeliLightHandler ()
{
    // Set render states
    auto pLights = g_pGame->GetPointLights ();
    pLights->PreRenderHeliLights ();

    // Render our lights
    for ( auto pLight : m_SearchLightList )
    {
        pLight->Render ();
    }

    // Reset render states
    pLights->PostRenderHeliLights ();
}

bool CClientPointLightsManager::IsLightsLimitReached ()
{
    auto pThis = g_pClientGame->GetManager ()->GetPointLightsManager ();
    return pThis->m_SearchLightList.size () >= 1000;
}

void CClientPointLightsManager::RemoveFromList ( CClientPointLights* pLight )
{
    if ( !m_bDontRemoveFromList )
    {
        if ( !m_List.empty() ) m_List.remove ( pLight );
    }
}

void CClientPointLightsManager::RemoveSearchLightFromList ( CClientSearchLight* pLight )
{
    if ( !m_bDontRemoveFromList )
    {
        m_SearchLightList.remove ( pLight );
    }
}
