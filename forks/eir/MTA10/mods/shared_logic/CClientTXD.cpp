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

    // RenderWare texture dictionary that holds texture replacement information.
    m_texDict = NULL;
}


CClientTXD::~CClientTXD ( void )
{
    // Remove us from all the models
    if ( m_texDict )
    {
        CTexDictionary::importList_t importedModels;

        importedModels = m_texDict->GetImportedList();

        delete m_texDict;

        // Restream affected models
        for ( CTexDictionary::importList_t::const_iterator iter = importedModels.begin(); iter != importedModels.end(); iter++ )
        {
            Restream ( *iter );
        }
    }

    // Remove us from all the clothes replacement doo dah
    g_pGame->GetRenderWare ()->ClothesRemoveReplacementTxd( m_FileData.GetData() );
}


bool CClientTXD::LoadTXD ( const char* szFile, bool bFilteringEnabled )
{
    // Unload previously loaded tex dictionary.
    if ( m_texDict )
    {
        delete m_texDict;

        m_texDict = NULL;
    }

    // Do load here to check for errors.
    m_strFilename = szFile;
    m_bFilteringEnabled = bFilteringEnabled;

    CFile *fileStream = g_pCore->GetModRoot()->Open( szFile, "rb" );

    CTexDictionary *txd = NULL;
    
    if ( fileStream )
    {
        txd = g_pGame->GetTextureManager()->CreateTxd( fileStream );

        if ( txd )
        {
            m_texDict = txd;

            // Apply filtering
            if ( bFilteringEnabled )
            {
                std::list <CTexture*> texList = txd->GetTextures();

                for ( std::list <CTexture*>::const_iterator iter = texList.begin(); iter != texList.end(); iter++ )
                {
                    CTexture *tex = *iter;

                    tex->SetFiltering( true );
                }
            }
        }

        delete fileStream;
    }

    return ( txd != NULL );
}

bool CClientTXD::Import ( unsigned short usModelID )
{
    if ( usModelID >= CLOTHES_TEX_ID_FIRST && usModelID <= CLOTHES_TEX_ID_LAST )
    {
        // If using for clothes only, unload 'replacing model textures' stuff to save memory
        if ( m_texDict )
        {
            delete m_texDict;

            m_texDict = NULL;
        }
        // Load txd file data if not already done
        if ( m_FileData.IsEmpty() )
        {
            if ( !g_pCore->GetNetwork ()->CheckFile ( "txd", m_strFilename ) )
                return false;
            m_FileData.LoadFromFile( m_strFilename );
        }
        g_pGame->GetRenderWare ()->ClothesAddReplacementTxd( m_FileData.GetData(), usModelID - CLOTHES_MODEL_ID_FIRST );
        return true;
    }
    else
    {
        // Ensure loaded for replacing model textures
        if ( !m_texDict )
        {
            LoadTXD( m_strFilename.c_str(), m_bFilteringEnabled );
        }

        if ( m_texDict )
        {
            // Fail if the TXD contains no textures.
            if ( m_texDict->GetTextures().empty() )
                return false;

            // Have we got textures and haven't already imported into this model?
            if ( m_texDict->Import( usModelID ) )
            {
                Restream ( usModelID );
                return true;
            }
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
