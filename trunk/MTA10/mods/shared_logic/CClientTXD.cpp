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


bool CClientTXD::LoadTXD ( const SString& strFile, bool bFilteringEnabled, bool bIsRawData )
{
    // Do load here to check for errors.
    m_bFilteringEnabled = bFilteringEnabled;
    m_bIsRawData = bIsRawData;

    if( !m_bIsRawData )
    {
        m_strFilename = strFile;
        CBuffer buffer;
        if ( !LoadFileData( buffer ) )
            return false;
        return g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures( &m_ReplacementTextures, buffer, m_bFilteringEnabled );
    }
    else
    {
        m_FileData = CBuffer( strFile, strFile.length() );
        if ( !g_pCore->GetNetwork()->CheckFile( "txd", "", m_FileData ) )
            return false;

        return g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures( &m_ReplacementTextures, m_FileData, m_bFilteringEnabled );
    }
}


bool CClientTXD::Import ( unsigned short usModelID )
{
    if ( usModelID >= CLOTHES_TEX_ID_FIRST && usModelID <= CLOTHES_TEX_ID_LAST )
    {
        if ( m_FileData.IsEmpty() && m_bIsRawData )
            return false;   // Raw data has been freed already because texture was first used as non-clothes

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
        }
        m_bUsingFileDataForClothes = true;
        g_pGame->GetRenderWare ()->ClothesAddReplacementTxd( m_FileData.GetData(), usModelID - CLOTHES_MODEL_ID_FIRST );
        return true;
    }
    else
    {
        // Ensure loaded for replacing model textures
        if ( m_ReplacementTextures.textures.empty () )
        {
            if( !m_bIsRawData )
            {
                CBuffer buffer;
                if ( !LoadFileData( buffer ) )
                    return false;
                g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures( &m_ReplacementTextures, buffer, m_bFilteringEnabled );
                if ( m_ReplacementTextures.textures.empty() )
                    return false;
            }
            else
            {
                g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures( &m_ReplacementTextures, m_FileData, m_bFilteringEnabled );
                if ( m_ReplacementTextures.textures.empty() )
                    return false;
            }
        }

        // If raw data and not used as clothes textures yet, then free raw data buffer to save RAM
        if ( m_bIsRawData && !m_bUsingFileDataForClothes )
        {
            // This means the texture can't be used for clothes now
            m_FileData.Clear();
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
    return g_pCore->GetNetwork()->CheckFile( "txd", m_strFilename, buffer );
}

// Return true if data looks like TXD file contents
bool CClientTXD::IsTXDData ( const SString& strData )
{
    return strData.length() > 32 && memcmp( strData, "\x16\x00\x00\x00", 4 ) == 0;
}
