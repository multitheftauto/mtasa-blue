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

    // Remove us from all the clothes replacement doo dah
    g_pGame->GetRenderWare ()->ClothesRemoveReplacementTxd( m_FileData.GetData() );
}


bool CClientTXD::LoadTXD ( const char* szFile, bool bFilteringEnabled )
{
    // Do load here to check for errors.
    m_strFilename = szFile;
    m_bFilteringEnabled = bFilteringEnabled;
    CBuffer buffer;
    if ( !LoadFileData( buffer ) )
        return false;
    return g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures( &m_ReplacementTextures, buffer, m_bFilteringEnabled );
}

bool CClientTXD::Import ( unsigned short usModelID )
{
    if ( usModelID >= CLOTHES_TEX_ID_FIRST && usModelID <= CLOTHES_TEX_ID_LAST )
    {
        // If using for clothes only, unload 'replacing model textures' stuff to save memory
        if ( !m_ReplacementTextures.textures.empty() && m_ReplacementTextures.usedInModelIds.empty() )
        {
            g_pGame->GetRenderWare ()->ModelInfoTXDRemoveTextures ( &m_ReplacementTextures );
            m_ReplacementTextures = SReplacementTextures();
        }
        // Load txd file data if not already done
        if ( m_FileData.IsEmpty() )
        {
            if ( !LoadFileData( m_FileData ) )
                return false;
            g_pClientGame->GetResourceManager()->ValidateResourceFile( m_strFilename, m_FileData );
        }
        g_pGame->GetRenderWare ()->ClothesAddReplacementTxd( m_FileData.GetData(), usModelID - CLOTHES_MODEL_ID_FIRST );
        return true;
    }
    else
    {
        // Ensure loaded for replacing model textures
        if ( m_ReplacementTextures.textures.empty () )
        {
            CBuffer buffer;
            if ( !LoadFileData( buffer ) )
                return false;
            g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures( &m_ReplacementTextures, buffer, m_bFilteringEnabled );
            if ( m_ReplacementTextures.textures.empty() )
                return false;
        }

        // Have we got textures and haven't already imported into this model?
        if ( g_pGame->GetRenderWare ()->ModelInfoTXDAddTextures ( &m_ReplacementTextures, usModelID ) )
        {
            Restream ( usModelID );
            return true;
        }
    }

    return false;
}

bool CClientTXD::IsImportableModel ( unsigned short usModelID )
{
    // Currently we work on vehicles and objects
    return CClientObjectManager::IsValidModel ( usModelID ) ||
           CClientVehicleManager::IsValidModel ( usModelID ) ||
           CClientPlayerManager::IsValidModel( usModelID ) ||
            ( usModelID >= CLOTHES_TEX_ID_FIRST && usModelID <= CLOTHES_TEX_ID_LAST );
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

// Load file contents into supplied buffer
bool CClientTXD::LoadFileData( CBuffer& buffer )
{
    buffer.LoadFromFile( m_strFilename );
    g_pClientGame->GetResourceManager()->ValidateResourceFile( m_strFilename, buffer );
    return g_pCore->GetNetwork()->CheckFile( "txd", m_strFilename );
}
