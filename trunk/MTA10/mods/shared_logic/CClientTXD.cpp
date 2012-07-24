/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.cpp
*  PURPOSE:     TXD manager class
*
*****************************************************************************/

#include <StdInc.h>

CClientTXD::CClientTXD ( class CClientManager* pManager, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    SetTypeName ( "txd" );
}


CClientTXD::~CClientTXD ( void )
{
    // Remove us from all the models
    g_pGame->GetRenderWare ()->ModelInfoTXDRemoveTextures ( &m_ReplacementTextures );

    // Restream affected models
    for ( uint i = 0 ; i < m_ReplacementTextures.usedInModelIds.size () ; i++ )
    {
        Restream ( m_ReplacementTextures.usedInModelIds[i] );
    }
}


bool CClientTXD::LoadTXD ( const char* szFile, bool bFilteringEnabled )
{
    return g_pGame->GetRenderWare ()->ModelInfoTXDLoadTextures ( &m_ReplacementTextures, szFile, bFilteringEnabled );
}

bool CClientTXD::Import ( unsigned short usModelID )
{
    // Have we got textures and haven't already imported into this model?
    if ( g_pGame->GetRenderWare ()->ModelInfoTXDAddTextures ( &m_ReplacementTextures, usModelID ) )
    {
        Restream ( usModelID );
        return true;
    }

    return false;
}

bool CClientTXD::IsImportableModel ( unsigned short usModelID )
{
    // Currently we work on vehicles and objects
    return CClientObjectManager::IsValidModel ( usModelID ) ||
           CClientVehicleManager::IsValidModel ( usModelID ) ||
           CClientPlayerManager::IsValidModel( usModelID );
}

void CClientTXD::Restream ( unsigned short usModelID )
{
    if ( CClientVehicleManager::IsValidModel ( usModelID ) )
    {
        m_pManager->GetVehicleManager ()->RestreamVehicles ( usModelID );
    }
    else if ( CClientObjectManager::IsValidModel ( usModelID ) )
    {
        if ( CClientPedManager::IsValidWeaponModel ( usModelID ) )
        {
            m_pManager->GetPedManager ()->RestreamWeapon ( usModelID );
            m_pManager->GetPickupManager ()->RestreamPickups ( usModelID );
        }
        m_pManager->GetObjectManager ()->RestreamObjects ( usModelID );
        g_pGame->GetModelInfo ( usModelID )->RestreamIPL ();
    }
    else if ( CClientPlayerManager::IsValidModel ( usModelID ) )
    {
        m_pManager->GetPedManager ()->RestreamPeds ( usModelID );
    }

}
