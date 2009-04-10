/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.cpp
*  PURPOSE:     TXD manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

CClientTXD::CClientTXD ( class CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_usMainModel = 0xFFFF;
}


CClientTXD::~CClientTXD ( void )
{
    // Remove us from all the models
    RemoveAll ();

    // Destroy all our textures
    std::list < RwTexture* >::iterator it;
    for ( it = m_Textures.begin (); it != m_Textures.end (); it++ )
    {
        g_pGame->GetRenderWare ()->DestroyTexture ( *it );
    }
    m_Textures.clear ();
}


bool CClientTXD::LoadTXD ( const char* szFile )
{
    // Are we not already loaded?
    if ( m_Textures.empty () )
    {
        // Try to load it
        RwTexDictionary* pTXD = g_pGame->GetRenderWare ()->ReadTXD ( szFile );
        if ( pTXD )
        {
            // Get the list of textures into our own list
            RwListEntry* pSentinel = &pTXD->textures.root;
            RwListEntry* pTexLink = pSentinel->next;
            while ( pTexLink != pSentinel )
            {
                RwTexture* pTex = (RwTexture *)( (BYTE *)pTexLink - offsetof(RwTexture, TXDList) );
                m_Textures.push_back ( pTex );
                pTex->txd = NULL;
                pTexLink = pTexLink->next;
            }
            // Make the txd forget it has any textures and destroy it
            pTXD->textures.root.next = &pTXD->textures.root;
            pTXD->textures.root.prev = &pTXD->textures.root;
            g_pGame->GetRenderWare ()->DestroyTXD ( pTXD );
            pTXD = NULL;
            // We succeeded if we got any textures
            return m_Textures.size () > 0;
        }
	}

    // Failed
    return false;
}

bool CClientTXD::Import ( unsigned short usModel )
{
    // Have we got textures and haven't already imported into this model?
    if ( !m_Textures.empty () && !IsImported ( usModel ) )
    {
        bool bMakeCopy = ( m_usMainModel != 0xFFFF );
        g_pGame->GetRenderWare ()->ModelInfoTXDAddTextures ( m_Textures, usModel, bMakeCopy );

        // Add the id to the list over which models we've imported ourselves into.
        m_ImportedModels.push_back ( usModel );
        if ( m_usMainModel == 0xFFFF )
            m_usMainModel = usModel;

        // Succeeded
        return true;
    }

    // Failed
    return false;
}


bool CClientTXD::IsImported ( unsigned short usModel )
{
    // Loop through our imported ids
    std::list < unsigned short >::iterator iter = m_ImportedModels.begin ();
    for ( ; iter != m_ImportedModels.end (); iter++ )
    {
        // Same id as given?
        if ( *iter == usModel )
        {
            // We've imported us into this model
            return true;
        }
    }

    // We haven't
    return false;
}


void CClientTXD::Remove ( unsigned short usModel )
{
    if ( IsImported ( usModel ) )
    {
        // Remove the model
        InternalRemove ( usModel );

        // Remove it from the list
        m_ImportedModels.remove ( usModel );
        if ( usModel == m_usMainModel )
            m_usMainModel = 0xFFFF;
    }
}


void CClientTXD::RemoveAll ( void )
{
    // Loop through our imported ids
    std::list < unsigned short > ::iterator iter = m_ImportedModels.begin ();
    for ( ; iter != m_ImportedModels.end (); iter++ )
    {
        // Remove each
        InternalRemove ( *iter );
    }

    // Clear the list
    m_ImportedModels.clear ();
    m_usMainModel = 0xFFFF;
}


bool CClientTXD::IsImportableModel ( unsigned short usModel )
{
    // Currently we work on vehicles and objects
    return CClientObjectManager::IsValidModel ( usModel ) ||
           CClientVehicleManager::IsValidModel ( usModel );
}


void CClientTXD::InternalRemove ( unsigned short usModel )
{
    bool bDestroy = ( usModel != m_usMainModel );
    g_pGame->GetRenderWare ()->ModelInfoTXDRemoveTextures ( m_Textures, usModel, bDestroy, true );
}
