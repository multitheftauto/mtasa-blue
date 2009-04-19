/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDFF.cpp
*  PURPOSE:     .dff model handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"

CClientDFF::CClientDFF ( CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pDFFManager = pManager->GetDFFManager ();

    m_pLoadedClump = NULL;
    SetTypeName ( "dff" );

    // Add us to DFF manager list
    m_pDFFManager->AddToList ( this );
}


CClientDFF::~CClientDFF ( void )
{
    // Remove us from DFF manager list
    m_pDFFManager->RemoveFromList ( this );

    // Restore all our models
    RestoreModels ();

    // Unload our DFF
    UnloadDFF ();
}


bool CClientDFF::LoadDFF ( const char* szFile, unsigned short usCollisionModel )
{
    // We can't have two clumps loaded.
    if ( !m_pLoadedClump )
    {
        // Attempt loading it
		m_pLoadedClump = g_pGame->GetRenderWare ()->ReadDFF ( szFile, usCollisionModel );

		// Return whether we succeeded or not
        return ( m_pLoadedClump != NULL );
    }

    // We failed
    return false;
}


void CClientDFF::UnloadDFF ( void )
{
    // We have a clump loaded?
	if ( m_pLoadedClump )
    {
        // Destroy it and null it
		g_pGame->GetRenderWare ()->DestroyDFF ( m_pLoadedClump );
        m_pLoadedClump = NULL;
    }
}


bool CClientDFF::ReplaceModel ( unsigned short usModel )
{
    // We have a DFF?
    if ( m_pLoadedClump )
    {
        // Have someone already replaced that model?
        CClientDFF* pReplaced = m_pDFFManager->GetElementThatReplaced ( usModel );
        if ( pReplaced )
        {
            // Remove it from its list so it won't restore the object if
            // it's destroyed or its resource is when it's been replaced
            // again by an another resource.
            pReplaced->m_Replaced.remove ( usModel );
        }

        // Is it an object model?
        if ( CClientObjectManager::IsValidModel ( usModel ) )
        {
            return ReplaceObjectModel ( usModel );
        }

        // Is it a vehicle model?
        if ( CClientVehicleManager::IsValidModel ( usModel ) )
        {
            return ReplaceVehicleModel ( usModel );
        }
    }

    // No supported type or no loaded clump
    return false;
}


bool CClientDFF::HasReplaced ( unsigned short usModel )
{
    // See if we have a match in our list
    std::list < unsigned short > ::iterator iter = m_Replaced.begin ();
    for ( ; iter != m_Replaced.end (); iter++ )
    {
        // Compare the models
        if ( *iter == usModel )
        {
            return true;
        }
    }

    // We haven't replaced this model
    return false;
}


void CClientDFF::RestoreModel ( unsigned short usModel )
{
    // Restore the model and remove it from the list
    InternalRestoreModel ( usModel );
    m_Replaced.remove ( usModel );
}


void CClientDFF::RestoreModels ( void )
{
    // Loop through our list over replaced models
    std::list < unsigned short > ::iterator iter = m_Replaced.begin ();
    for ( ; iter != m_Replaced.end (); iter++ )
    {
        // Restore this model
        InternalRestoreModel ( *iter );
    }

    // Clear the list
    m_Replaced.clear ();
}


void CClientDFF::InternalRestoreModel ( unsigned short usModel )
{
    // Is this a vehicle ID?
    if ( CClientVehicleManager::IsValidModel ( usModel ) )
    {
        // Stream the vehicles of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pDFFManager->ReStreamVehicles ( usModel );
    }

    // Is this an object ID?
    else if ( CClientObjectManager::IsValidModel ( usModel ) )
    {
        // Stream the objects of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pDFFManager->ReStreamObjects ( usModel );
    }
    else
        return;

    // Restore all the models we replaced.
    g_pGame->GetModelInfo ( usModel )->RestoreOriginalModel ();
}


bool CClientDFF::ReplaceObjectModel ( unsigned short usModel )
{
    // Stream out all the vehicle models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pDFFManager->ReStreamObjects ( usModel );

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModel );
    pModelInfo->SetCustomModel ( m_pLoadedClump );

    // Remember that we've replaced that vehicle model
    m_Replaced.push_back ( usModel );

    // Success
    return true;
}


bool CClientDFF::ReplaceVehicleModel ( unsigned short usModel )
{
    // Stream out all the vehicle models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pDFFManager->ReStreamVehicles ( usModel );

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModel );
    pModelInfo->SetCustomModel ( m_pLoadedClump );

    // Remember that we've replaced that vehicle model
    m_Replaced.push_back ( usModel );

    // Success
    return true;
}
