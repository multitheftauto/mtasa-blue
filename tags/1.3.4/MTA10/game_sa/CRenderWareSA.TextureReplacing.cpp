/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.TextureReplacing.cpp
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

//
// Info about the current state of a model's txd textures
//
class CModelTexturesInfo
{
public:
    std::vector < RwTexture* >              originalTextures;
    std::vector < SReplacementTextures* >   usedByReplacements;
    ushort                                  usTxdId;
    RwTexDictionary*                        pTxd;
};

std::map < ushort, CModelTexturesInfo > ms_ModelTexturesInfoMap;


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTexturesInfo
//
// Find/create texture info for a modelid
//
////////////////////////////////////////////////////////////////
CModelTexturesInfo* CRenderWareSA::GetModelTexturesInfo ( ushort usModelId )
{
    if ( !pGame->GetModelInfo ( usModelId ) )
        return NULL;

    ushort usTxdId = pGame->GetModelInfo ( usModelId )->GetTextureDictionaryID ();

    CModelTexturesInfo* pInfo = MapFind ( ms_ModelTexturesInfoMap, usTxdId );
    if ( !pInfo )
    {
        // Get txd
        RwTexDictionary* pTxd = CTxdStore_GetTxd ( usTxdId );

        if ( !pTxd )
        {
            pGame->GetModelInfo ( usModelId )->Request ( BLOCKING, "CRenderWareSA::GetModelTexturesInfo" );
            CTxdStore_AddRef ( usTxdId );
            ( (void (__cdecl *)(unsigned short))FUNC_RemoveModel )( usModelId );
            pTxd = CTxdStore_GetTxd ( usTxdId );
        }
        else
        {
            CTxdStore_AddRef ( usTxdId );
        }

        if ( !pTxd )
            return NULL;

        // Add new info
        MapSet ( ms_ModelTexturesInfoMap, usTxdId, CModelTexturesInfo () );
        pInfo = MapFind ( ms_ModelTexturesInfoMap, usTxdId );
        pInfo->usTxdId = usTxdId;
        pInfo->pTxd = pTxd;

        // Save original textures
        GetTxdTextures ( pInfo->originalTextures, pInfo->pTxd );
    }

    return pInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDLoadTextures
//
// Load textures from a TXD file
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::ModelInfoTXDLoadTextures ( SReplacementTextures* pReplacementTextures, const SString& strFilename, bool bFilteringEnabled )
{
    // Are we already loaded?
    if ( !pReplacementTextures->textures.empty () )
        return false;

    if ( !g_pCore->GetNetwork ()->CheckFile ( "txd", strFilename ) )
        return false;

    // Try to load it
    RwTexDictionary* pTxd = ReadTXD ( strFilename );
    if ( pTxd )
    {
        // Get the list of textures into our own list
        GetTxdTextures ( pReplacementTextures->textures, pTxd );

        for ( uint i = 0 ; i < pReplacementTextures->textures.size () ; i++ )
        {
            pReplacementTextures->textures[i]->txd = NULL;
            if ( bFilteringEnabled )
                pReplacementTextures->textures[i]->flags = 0x1102;       // Enable filtering (otherwise textures are pixely)
        }

        // Make the txd forget it has any textures and destroy it
        pTxd->textures.root.next = &pTxd->textures.root;
        pTxd->textures.root.prev = &pTxd->textures.root;
        RwTexDictionaryDestroy ( pTxd );
        pTxd = NULL;

        // We succeeded if we got any textures
        return pReplacementTextures->textures.size () > 0;
    }

    return false;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDAddTextures
//
// Adds texture into the TXD of a model.
// Returns true if model was affected.
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::ModelInfoTXDAddTextures ( SReplacementTextures* pReplacementTextures, ushort usModelId )
{
    // Already done for this modelid?
    if ( ListContains ( pReplacementTextures->usedInModelIds, usModelId ) )
        return false;

    // Get valid textures info for this model
    CModelTexturesInfo* pInfo = GetModelTexturesInfo ( usModelId );
    if ( !pInfo )
        return false;

    // Remember which models this set has been applied to
    pReplacementTextures->usedInModelIds.push_back ( usModelId );

    // Already done for this txd?
    if ( ListContains ( pReplacementTextures->usedInTxdIds, pInfo->usTxdId ) )
        return true;    // Return true as model may need restreaming

    //
    // Add section for this txd
    //
    pReplacementTextures->perTxdList.push_back ( SReplacementTextures::SPerTxd () );
    SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList.back ();

    perTxdInfo.usTxdId = pInfo->usTxdId;
    perTxdInfo.bTexturesAreCopies = ( pReplacementTextures->usedInTxdIds.size () > 0 );

    // Copy / clone textures
    for ( std::vector < RwTexture* >::iterator iter = pReplacementTextures->textures.begin () ; iter != pReplacementTextures->textures.end () ; iter++ )
    {
        RwTexture* pNewTexture = *iter;

        // Use a copy if not first txd
        if ( perTxdInfo.bTexturesAreCopies )
        {
            // Reuse the given texture's raster
            RwTexture* pCopyTex = RwTextureCreate ( pNewTexture->raster );

            // Copy over additional properties
            MemCpyFast ( &pCopyTex->name, &pNewTexture->name, RW_TEXTURE_NAME_LENGTH );
            MemCpyFast ( &pCopyTex->mask, &pNewTexture->mask, RW_TEXTURE_NAME_LENGTH );
            pCopyTex->flags = pNewTexture->flags;
            
            pNewTexture = pCopyTex;
        }
        perTxdInfo.usingTextures.push_back ( pNewTexture );
    }

    //
    // Add each texture to the target txd
    //
    for ( std::vector < RwTexture* >::iterator iter = perTxdInfo.usingTextures.begin () ; iter != perTxdInfo.usingTextures.end () ; iter++ )
    {
        RwTexture* pNewTexture = *iter;

        // If there is a name clash with an existing texture, replace it
        RwTexture* pExistingTexture = RwTexDictionaryFindNamedTexture ( pInfo->pTxd, pNewTexture->name );
        if ( pExistingTexture )
        {
            RwTexDictionaryRemoveTexture ( pInfo->pTxd, pExistingTexture );
        }

        // Add the texture
        dassert ( !RwTexDictionaryContainsTexture ( pInfo->pTxd, pNewTexture ) );
        RwTexDictionaryAddTexture ( pInfo->pTxd, pNewTexture );
    }

    // Remember which txds this set has been applied to
    pReplacementTextures->usedInTxdIds.push_back ( pInfo->usTxdId );
    dassert ( !ListContains ( pInfo->usedByReplacements, pReplacementTextures ) );
    pInfo->usedByReplacements.push_back ( pReplacementTextures );
    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDRemoveTextures
//
// Remove the textures from the txds that are using them.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ModelInfoTXDRemoveTextures ( SReplacementTextures* pReplacementTextures )
{
    // For each using txd
    for ( uint i = 0 ; i < pReplacementTextures->perTxdList.size () ; i++ )
    {
        SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList[i];

        // Get textures info
        ushort usTxdId = perTxdInfo.usTxdId;
        CModelTexturesInfo* pInfo = MapFind ( ms_ModelTexturesInfoMap, usTxdId );

        // Validate
        dassert ( MapFind ( ms_ModelTexturesInfoMap, usTxdId ) );
        dassert ( ListContains ( pInfo->usedByReplacements, pReplacementTextures ) );

        // Remove replacement textures
        for ( uint i = 0 ; i < perTxdInfo.usingTextures.size () ; i++ )
        {
            RwTexture* pOldTexture = perTxdInfo.usingTextures[i];
            RwTexDictionaryRemoveTexture ( pInfo->pTxd, pOldTexture );
            dassert ( !RwTexDictionaryContainsTexture ( pInfo->pTxd, pOldTexture ) );
            if ( perTxdInfo.bTexturesAreCopies )
            {
                // Destroy the copy (but not the raster as that was not copied)
                pOldTexture->raster = NULL;
                RwTextureDestroy ( pOldTexture );
            }
        }

        // Ensure there are original named textures in the txd
        for ( uint i = 0 ; i < pInfo->originalTextures.size () ; i++ )
        {
            RwTexture* pOriginalTexture = pInfo->originalTextures[i];
            if ( !RwTexDictionaryFindNamedTexture ( pInfo->pTxd, pOriginalTexture->name ) )
                RwTexDictionaryAddTexture ( pInfo->pTxd, pOriginalTexture );
        }

        // Remove refs
        ListRemove ( pInfo->usedByReplacements, pReplacementTextures );

        // If no refs left, check original state and then remove info
        if ( pInfo->usedByReplacements.empty () )
        {
            // txd should now contain the same textures as 'originalTextures'
        #ifdef MTA_DEBUG
            std::vector < RwTexture* > currentTextures;
            GetTxdTextures ( currentTextures, pInfo->pTxd );
            assert ( currentTextures.size () == pInfo->originalTextures.size () );
            for ( uint i = 0 ; i < pInfo->originalTextures.size () ; i++ )
            {
                RwTexture* pOriginalTexture = pInfo->originalTextures[i];
                assert ( ListContains ( currentTextures, pOriginalTexture ) );
                ListRemove ( currentTextures, pOriginalTexture );
            }
            assert ( currentTextures.empty () );
        #endif

            // Remove info
            CTxdStore_RemoveRef ( pInfo->usTxdId );
            MapRemove ( ms_ModelTexturesInfoMap, usTxdId );
        }
    }

    // Destroy replacement textures
    for ( uint i = 0 ; i < pReplacementTextures->textures.size () ; i++ )
    {
        RwTexture* pOldTexture = pReplacementTextures->textures[i];
        DestroyTexture ( pOldTexture );
    }
    pReplacementTextures->textures.clear ();
}
