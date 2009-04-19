/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.cpp
*  PURPOSE:     TXD manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               arc_
*
*****************************************************************************/

#include <StdInc.h>

std::map < unsigned short, std::list < RwTexture* > > CClientTXD::ms_ReplacedTXDTextures;
std::map < unsigned short, std::list < RwTexture* > > CClientTXD::ms_AddedTXDTextures;

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

bool CClientTXD::Import ( unsigned short usModelID )
{
    // Have we got textures and haven't already imported into this model?
    if ( !m_Textures.empty () && !IsImported ( usModelID ) )
    {
        // Grab the lists of replaced and added textures for the model's TXD
        unsigned short usTxdID = g_pGame->GetModelInfo ( usModelID )->GetTextureDictionaryID ();
        std::list < RwTexture* >& replacedTextures = ms_ReplacedTXDTextures.insert (
            make_pair ( usTxdID, std::list < RwTexture* > () )
        ).first->second;
        std::list < RwTexture* >& addedTextures = ms_AddedTXDTextures.insert (
            make_pair ( usTxdID, std::list < RwTexture* > () )
        ).first->second;

        // If we already used our main textures in a model, we need to make copies now
        // for this new model.
        bool bMakeCopy = ( m_usMainModel != 0xFFFF );

        // Add the textures.
        g_pGame->GetRenderWare ()->ModelInfoTXDAddTextures ( m_Textures, usModelID, bMakeCopy, &replacedTextures, &addedTextures );

        // Add the id to the list over which models we've imported ourselves into.
        m_ImportedModels.push_back ( usModelID );
        if ( m_usMainModel == 0xFFFF )
            m_usMainModel = usModelID;

        return true;
    }

    return false;
}


bool CClientTXD::IsImported ( unsigned short usModelID )
{
    return std::find ( m_ImportedModels.begin (), m_ImportedModels.end (), usModelID ) != m_ImportedModels.end ();
}


void CClientTXD::Remove ( unsigned short usModel )
{
    if ( IsImported ( usModel ) )
    {
        // Remove the model
        InternalRemove ( usModel );

        // Remove it from the list
        m_ImportedModels.remove ( usModel );

        // If we just removed ourselves from our main model, we are free again to use our main
        // textures in a model directly
        if ( usModel == m_usMainModel )
            m_usMainModel = 0xFFFF;
    }
}


void CClientTXD::RemoveAll ( void )
{
    std::list < unsigned short > ::iterator it;
    for ( it = m_ImportedModels.begin (); it != m_ImportedModels.end (); it++ )
    {
        InternalRemove ( *it );
    }

    m_ImportedModels.clear ();
    m_usMainModel = 0xFFFF;
}


bool CClientTXD::IsImportableModel ( unsigned short usModelID )
{
    // Currently we work on vehicles and objects
    return CClientObjectManager::IsValidModel ( usModelID ) ||
           CClientVehicleManager::IsValidModel ( usModelID );
}


void CClientTXD::InternalRemove ( unsigned short usModelID )
{
    // If this is not our main model, we added copies in there and we need to destroy them now
    bool bDestroy = ( usModelID != m_usMainModel );

    // Remove the textures
    g_pGame->GetRenderWare ()->ModelInfoTXDRemoveTextures ( m_Textures, usModelID, bDestroy, true );

    // Re-add any replaced textures
    unsigned short usTxdID = g_pGame->GetModelInfo ( usModelID )->GetTextureDictionaryID ();
    std::map < unsigned short, std::list < RwTexture* > >::iterator it;
    it = ms_ReplacedTXDTextures.find ( usTxdID );
    if ( it != ms_ReplacedTXDTextures.end () )
    {
        std::list < RwTexture* >::iterator myIt;
        std::list < RwTexture* > replacedTextures;
        for ( myIt = m_Textures.begin (); myIt != m_Textures.end (); myIt++ )
        {
            RwTexture* pReplacedTexture = FindNamedTextureInList ( it->second, (*myIt)->name );
            if ( pReplacedTexture )
            {
                replacedTextures.push_back ( pReplacedTexture );
                it->second.remove ( pReplacedTexture );
            }
        }
        g_pGame->GetRenderWare ()->ModelInfoTXDAddTextures ( replacedTextures, usModelID, false, NULL, NULL, false );
        if ( it->second.empty () )
            ms_ReplacedTXDTextures.erase ( it );
    }

    // Remove any added textures from the list
    it = ms_AddedTXDTextures.find ( usTxdID );
    if ( it != ms_AddedTXDTextures.end () )
    {
        std::list < RwTexture* >::iterator myIt;
        for ( myIt = m_Textures.begin (); myIt != m_Textures.end (); myIt++ )
        {
            it->second.remove ( *myIt );
        }
        if ( it->second.empty () )
            ms_AddedTXDTextures.erase ( it );
    }
}

RwTexture* CClientTXD::FindNamedTextureInList ( std::list < RwTexture* >& list, const char* szTexName )
{
    std::list < RwTexture* >::iterator it;
    for ( it = list.begin (); it != list.end (); it++ )
    {
        if ( !stricmp ( szTexName, (*it)->name ) )
            return *it;
    }
    return NULL;
}
