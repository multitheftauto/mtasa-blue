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

CClientDFF::CClientDFF ( CClientManager* pManager, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pDFFManager = pManager->GetDFFManager ();

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

    // Unload our clumps
    UnloadDFF ();
}


// Get a clump which has been loaded to replace the specified model id
RpClump* CClientDFF::GetLoadedClump ( ushort usModelId )
{
    if ( usModelId == 0 )
        return NULL;

    SLoadedClumpInfo& info = MapGet ( m_LoadedClumpInfoMap, usModelId );

    if ( !info.bTriedLoad )
    {
        info.bTriedLoad = true;

        // Make sure previous model+collision is loaded
        m_pManager->GetModelRequestManager ()->RequestBlocking ( usModelId, "CClientDFF::LoadDFF" );

        // Attempt loading it
        info.pClump = g_pGame->GetRenderWare ()->ReadDFF ( m_strDffFilename, usModelId, CClientVehicleManager::IsValidModel ( usModelId ) );
    }

    return info.pClump;
}


bool CClientDFF::LoadDFF ( const char* szFile, unsigned short usModelId )
{
    // Should only be called once, directly after construction
    assert ( m_strDffFilename.empty () );

    m_strDffFilename = szFile;
    if ( m_strDffFilename.empty () )
        return false;

    if ( !FileExists ( m_strDffFilename ) )
        return false;

    if ( !g_pCore->GetNetwork ()->CheckFile ( "dff", m_strDffFilename ) )
        return false;

    // Do load now if a model id was supplied
    if ( usModelId != 0 )
    {
        if ( GetLoadedClump ( usModelId ) == NULL )
            return false;
    }

    return true;
}


void CClientDFF::UnloadDFF ( void )
{
    for ( std::map < ushort, SLoadedClumpInfo >::iterator iter = m_LoadedClumpInfoMap.begin () ; iter != m_LoadedClumpInfoMap.end () ; ++iter )
    {
        SLoadedClumpInfo& info = iter->second;
        if ( info.pClump )
            g_pGame->GetRenderWare ()->DestroyDFF ( info.pClump );
    }

    m_LoadedClumpInfoMap.clear ();
}


bool CClientDFF::ReplaceModel ( unsigned short usModel )
{
    // Get clump loaded for this model id
    RpClump* pClump = GetLoadedClump ( usModel );

    // We have a DFF?
    if ( pClump )
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
            return ReplaceObjectModel ( pClump, usModel );
        }

        // Is it a vehicle model?
        else if ( CClientVehicleManager::IsValidModel ( usModel ) )
        {
            return ReplaceVehicleModel ( pClump, usModel );
        }

        else
        {
            // Weapon model replacement - Figure out how to reload weapons here!
            return ReplaceWeaponModel ( pClump, usModel );
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
        m_pManager->GetVehicleManager ()->RestreamVehicles ( usModel );
    }

    // Is this an object ID?
    else if ( CClientObjectManager::IsValidModel ( usModel ) )
    {
        if ( CClientPedManager::IsValidWeaponModel ( usModel ) )
        {
            // Stream the weapon of that model out so we have no
            // loaded when we do the restore. The streamer will
            // eventually stream them back in with async loading.
            m_pManager->GetPedManager ()->RestreamWeapon ( usModel );

        }
        // Stream the objects of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pManager->GetObjectManager ()->RestreamObjects ( usModel );
    }
    // Is this an ped ID?
    else if ( CClientPlayerManager::IsValidModel ( usModel ) )
    {
        // Stream the ped of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pManager->GetPedManager ()->RestreamPeds ( usModel );
    }
    else
        return;

    // Restore all the models we replaced.
    g_pGame->GetModelInfo ( usModel )->RestoreOriginalModel ();
}


bool CClientDFF::ReplaceObjectModel ( RpClump* pClump, ushort usModel )
{
    // Stream out all the object models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pManager->GetObjectManager ()->RestreamObjects ( usModel );
    g_pGame->GetModelInfo ( usModel )->RestreamIPL ();

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModel );
    pModelInfo->SetCustomModel ( pClump );

    // Remember that we've replaced that object model
    m_Replaced.push_back ( usModel );

    // Success
    return true;
}

bool CClientDFF::ReplaceWeaponModel ( RpClump* pClump, ushort usModel )
{
    // Stream out all the weapon models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pManager->GetObjectManager ()->RestreamObjects ( usModel );
    g_pGame->GetModelInfo ( usModel )->RestreamIPL ();

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModel );
    pModelInfo->SetCustomModel ( pClump );

    // Remember that we've replaced that weapon model
    m_Replaced.push_back ( usModel );

    // Success
    return true;
}

bool CClientDFF::ReplacePedModel ( RpClump* pClump, ushort usModel )
{
    // Stream out all the weapon models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pManager->GetObjectManager ()->RestreamObjects ( usModel );
    g_pGame->GetModelInfo ( usModel )->RestreamIPL ();

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModel );
    pModelInfo->SetCustomModel ( pClump );

    // Remember that we've replaced that weapon model
    m_Replaced.push_back ( usModel );

    // Success
    return true;
}

bool CClientDFF::ReplaceVehicleModel ( RpClump* pClump, ushort usModel )
{
    // Make sure previous model+collision is loaded
    m_pManager->GetModelRequestManager ()->RequestBlocking ( usModel, "CClientDFF::ReplaceVehicleModel" );

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModel );
    pModelInfo->SetCustomModel ( pClump );

    // Remember that we've replaced that vehicle model
    m_Replaced.push_back ( usModel );

    // Stream out all the vehicle models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pManager->GetVehicleManager ()->RestreamVehicles ( usModel );

    // Success
    return true;
}
