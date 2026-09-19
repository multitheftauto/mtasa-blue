/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.TextureReplacing.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "gamesa_renderware.h"

extern CGameSA* pGame;

//
// Info about the current state of a model's txd textures
//
class CModelTexturesInfo
{
public:
    std::vector<RwTexture*>            originalTextures;
    std::vector<SReplacementTextures*> usedByReplacements;
    ushort                             usTxdId;
    RwTexDictionary*                   pTxd;
};

std::map<ushort, CModelTexturesInfo> ms_ModelTexturesInfoMap;

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::NotifyTxdDestroyed
//
// Motivation: Fix #4028 (Hard-hitting crash in RwTexDictionaryRemoveTexture).
// When a TXD dictionary is destroyed by GTA streaming or TxdForceUnload, RenderWare
// automatically frees the dictionary and all textures attached to it.
// We must notify the model texture replacement system so ms_ModelTexturesInfoMap
// doesn't retain dangling pointers to the deallocated RwTexDictionary or its textures.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::NotifyTxdDestroyed(ushort usTxdId)
{
    CModelTexturesInfo* pInfo = MapFind(ms_ModelTexturesInfoMap, usTxdId);
    if (pInfo)
    {
        pInfo->pTxd = nullptr;
        pInfo->originalTextures.clear();
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTexturesInfo
//
// Find/create texture info for a modelid
//
////////////////////////////////////////////////////////////////
CModelTexturesInfo* CRenderWareSA::GetModelTexturesInfo(ushort usModelId)
{
    CModelInfoSA* pModelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
    if (!pModelInfo)
        return NULL;

    ushort usTxdId = pModelInfo->GetTextureDictionaryID();

    CModelTexturesInfo* pInfo = MapFind(ms_ModelTexturesInfoMap, usTxdId);
    // Fix #4028: If pInfo exists but pInfo->pTxd was destroyed/unloaded, we must re-acquire
    // a valid TXD from CTxdStore or request it from the streaming engine rather than using
    // a stale nullptr or dangling pointer.
    if (!pInfo || !pInfo->pTxd)
    {
        // Get txd
        RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);

        if (!pTxd)
        {
            pModelInfo->Request(BLOCKING, "CRenderWareSA::GetModelTexturesInfo");
            CTxdStore_AddRef(usTxdId);
            ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
            pTxd = CTxdStore_GetTxd(usTxdId);
        }
        else
        {
            CTxdStore_AddRef(usTxdId);
            if (pModelInfo->GetModelType() == eModelInfoType::PED)
            {
                // Mystery fix for #9336: (MTA sometimes fails at loading custom textures)
                // Possibly forces the ped model to be reloaded in some way
                ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
            }
        }

        if (!pTxd)
            return NULL;

        // Add new info or reuse existing info slot if it was previously invalidated
        if (!pInfo)
        {
            MapSet(ms_ModelTexturesInfoMap, usTxdId, CModelTexturesInfo());
            pInfo = MapFind(ms_ModelTexturesInfoMap, usTxdId);
        }
        pInfo->usTxdId = usTxdId;
        pInfo->pTxd = pTxd;

        // Save original textures
        pInfo->originalTextures.clear();
        GetTxdTextures(pInfo->originalTextures, pInfo->pTxd);
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
bool CRenderWareSA::ModelInfoTXDLoadTextures(SReplacementTextures* pReplacementTextures, const SString& strFilename, const SString& buffer,
                                             bool bFilteringEnabled)
{
    // Are we already loaded?
    if (!pReplacementTextures->textures.empty())
        return false;

    // Try to load it
    RwTexDictionary* pTxd = ReadTXD(strFilename, buffer);
    if (pTxd)
    {
        // Get the list of textures into our own list
        GetTxdTextures(pReplacementTextures->textures, pTxd);

        for (uint i = 0; i < pReplacementTextures->textures.size(); i++)
        {
            pReplacementTextures->textures[i]->txd = NULL;
            if (bFilteringEnabled)
                pReplacementTextures->textures[i]->flags = 0x1102;  // Enable filtering (otherwise textures are pixely)
        }

        // Make the txd forget it has any textures and destroy it
        pTxd->textures.root.next = &pTxd->textures.root;
        pTxd->textures.root.prev = &pTxd->textures.root;
        RwTexDictionaryDestroy(pTxd);
        pTxd = NULL;

        // We succeeded if we got any textures
        return pReplacementTextures->textures.size() > 0;
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
bool CRenderWareSA::ModelInfoTXDAddTextures(SReplacementTextures* pReplacementTextures, ushort usModelId)
{
    // Already done for this modelid?
    if (ListContains(pReplacementTextures->usedInModelIds, usModelId))
        return false;

    // Get valid textures info for this model
    CModelTexturesInfo* pInfo = GetModelTexturesInfo(usModelId);
    if (!pInfo)
        return false;

    // Remember which models this set has been applied to
    pReplacementTextures->usedInModelIds.push_back(usModelId);

    // Already done for this txd?
    if (ListContains(pReplacementTextures->usedInTxdIds, pInfo->usTxdId))
        return true;  // Return true as model may need restreaming

    //
    // Add section for this txd
    //
    pReplacementTextures->perTxdList.push_back(SReplacementTextures::SPerTxd());
    SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList.back();

    perTxdInfo.usTxdId = pInfo->usTxdId;
    perTxdInfo.bTexturesAreCopies = (pReplacementTextures->usedInTxdIds.size() > 0);

    // Copy / clone textures
    for (std::vector<RwTexture*>::iterator iter = pReplacementTextures->textures.begin(); iter != pReplacementTextures->textures.end(); iter++)
    {
        RwTexture* pNewTexture = *iter;

        // Use a copy if not first txd
        if (perTxdInfo.bTexturesAreCopies)
        {
            // Reuse the given texture's raster
            RwTexture* pCopyTex = RwTextureCreate(pNewTexture->raster);

            // Copy over additional properties
            MemCpyFast(&pCopyTex->name, &pNewTexture->name, RW_TEXTURE_NAME_LENGTH);
            MemCpyFast(&pCopyTex->mask, &pNewTexture->mask, RW_TEXTURE_NAME_LENGTH);
            pCopyTex->flags = pNewTexture->flags;

            pNewTexture = pCopyTex;
        }
        perTxdInfo.usingTextures.push_back(pNewTexture);
    }

    //
    // Add each texture to the target txd
    //
    for (std::vector<RwTexture*>::iterator iter = perTxdInfo.usingTextures.begin(); iter != perTxdInfo.usingTextures.end(); iter++)
    {
        RwTexture* pNewTexture = *iter;

        // If there is a name clash with an existing texture, replace it
        RwTexture* pExistingTexture = RwTexDictionaryFindNamedTexture(pInfo->pTxd, pNewTexture->name);
        if (pExistingTexture)
        {
            RwTexDictionaryRemoveTexture(pInfo->pTxd, pExistingTexture);
        }

        // Add the texture
        dassert(!RwTexDictionaryContainsTexture(pInfo->pTxd, pNewTexture));
        RwTexDictionaryAddTexture(pInfo->pTxd, pNewTexture);
    }

    // Remember which txds this set has been applied to
    pReplacementTextures->usedInTxdIds.push_back(pInfo->usTxdId);
    dassert(!ListContains(pInfo->usedByReplacements, pReplacementTextures));
    pInfo->usedByReplacements.push_back(pReplacementTextures);
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDRemoveTextures
//
// Remove the textures from the txds that are using them.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ModelInfoTXDRemoveTextures(SReplacementTextures* pReplacementTextures)
{
    // For each using txd
    for (uint i = 0; i < pReplacementTextures->perTxdList.size(); i++)
    {
        SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList[i];

        // Get textures info
        ushort              usTxdId = perTxdInfo.usTxdId;
        CModelTexturesInfo* pInfo = MapFind(ms_ModelTexturesInfoMap, usTxdId);

        // Fix #4028: In release builds, dassert does not halt execution.
        // If pInfo is null, continuing prevents a crash dereferencing null.
        if (!pInfo)
            continue;

        dassert(ListContains(pInfo->usedByReplacements, pReplacementTextures));

        // Fix #4028: Check if the TXD pointer is still valid and loaded in CTxdStore.
        // If GTA streaming or TxdForceUnload unloaded this TXD, RwTexDictionaryDestroy
        // was already called by the engine, meaning the dictionary and all textures inside
        // it have already been deallocated.
        RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(usTxdId);
        bool bTxdValid = (pInfo->pTxd != nullptr) &&
                         (pCurrentTxd == pInfo->pTxd) &&
                         SharedUtil::IsReadablePointer(pInfo->pTxd, sizeof(RwTexDictionary));

        // If the dictionary is alive and valid, take a snapshot of its current active textures.
        std::vector<RwTexture*> liveTextures;
        if (bTxdValid)
        {
            GetTxdTextures(liveTextures, pInfo->pTxd);
        }

        // Remove replacement textures
        for (uint j = 0; j < perTxdInfo.usingTextures.size(); j++)
        {
            RwTexture* pOldTexture = perTxdInfo.usingTextures[j];

            // Fix #4028: Only remove the texture from the dictionary if the dictionary is alive
            // and the texture actually exists within it. If the TXD was already destroyed, its textures
            // were already freed, and calling RwTexDictionaryRemoveTexture will crash with an AV (0xC0000005)
            // when accessing deallocated memory (pTex->txd).
            if (bTxdValid && pOldTexture && ListContains(liveTextures, pOldTexture))
            {
                RwTexDictionaryRemoveTexture(pInfo->pTxd, pOldTexture);
                dassert(!RwTexDictionaryContainsTexture(pInfo->pTxd, pOldTexture));
                if (perTxdInfo.bTexturesAreCopies)
                {
                    // Destroy the copy (but not the raster as that was not copied)
                    pOldTexture->raster = NULL;
                    RwTextureDestroy(pOldTexture);
                }
            }
            else if (perTxdInfo.bTexturesAreCopies && !bTxdValid)
            {
                // When the dictionary was destroyed, RenderWare's RwTexDictionaryDestroy iterated
                // through all textures in the dictionary and called RwTextureDestroy. Therefore,
                // the copy has already been freed and attempting to destroy it again would cause a crash / double free.
            }
        }
        perTxdInfo.usingTextures.clear();

        // Ensure there are original named textures in the txd (only if TXD is still alive)
        if (bTxdValid)
        {
            for (uint j = 0; j < pInfo->originalTextures.size(); j++)
            {
                RwTexture* pOriginalTexture = pInfo->originalTextures[j];
                if (pOriginalTexture && SharedUtil::IsReadablePointer(pOriginalTexture, sizeof(RwTexture)))
                {
                    if (!RwTexDictionaryFindNamedTexture(pInfo->pTxd, pOriginalTexture->name))
                        RwTexDictionaryAddTexture(pInfo->pTxd, pOriginalTexture);
                }
            }
        }

        // Remove refs
        ListRemove(pInfo->usedByReplacements, pReplacementTextures);

        // If no refs left, check original state and then remove info
        if (pInfo->usedByReplacements.empty())
        {
            if (bTxdValid)
            {
#ifdef MTA_DEBUG
                std::vector<RwTexture*> currentTextures;
                GetTxdTextures(currentTextures, pInfo->pTxd);
                assert(currentTextures.size() == pInfo->originalTextures.size());
                for (uint j = 0; j < pInfo->originalTextures.size(); j++)
                {
                    RwTexture* pOriginalTexture = pInfo->originalTextures[j];
                    assert(ListContains(currentTextures, pOriginalTexture));
                    ListRemove(currentTextures, pOriginalTexture);
                }
                assert(currentTextures.empty());

                int32_t refsCount = CTxdStore_GetNumRefs(pInfo->usTxdId);
                assert(refsCount > 0 && "Should have at least one TXD reference here");
#endif
                // Remove ref from CTxdStore only if it's still alive
                CTxdStore_RemoveRef(pInfo->usTxdId);
            }
            // Remove info from map
            MapRemove(ms_ModelTexturesInfoMap, usTxdId);
        }
    }

    // Destroy replacement textures
    for (uint i = 0; i < pReplacementTextures->textures.size(); i++)
    {
        RwTexture* pOldTexture = pReplacementTextures->textures[i];
        DestroyTexture(pOldTexture);
    }
    pReplacementTextures->textures.clear();
}
