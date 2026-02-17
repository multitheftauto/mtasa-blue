/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CCoreInterface.h>
#include "CGameSA.h"
#include "CRenderWareSA.ShaderMatching.h"
#include "CRenderWareSA.ShaderSupport.h"

extern CCoreInterface* g_pCore;
extern CGameSA*        pGame;

#define ADDR_CCustomCarPlateMgr_CreatePlateTexture_TextureSetName    0x06FDF40
#define ADDR_CCustomRoadsignMgr_CreateRoadsignTexture_TextureSetName 0x06FED49
#define ADDR_CClothesBuilder_ConstructTextures_Start                 0x05A6040
#define ADDR_CClothesBuilder_ConstructTextures_End                   0x05A6520
#define ADDR_CVehicle_DoHeadLightBeam_RenderPrimitive                0x06E13CD
#define ADDR_CHeli_SearchLightCone_RenderPrimitive                   0x06C62AD
#define ADDR_CWaterCannon_Render_RenderPrimitive                     0x072956B

enum
{
    RT_NONE,
    RT_2DI,
    RT_2DNI,
    RT_3DI,
    RT_3DNI,
};

int CRenderWareSA::ms_iRenderingType = 0;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// The hooks
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

// Hooks for creating txd create and destroy events
#define HOOKPOS_CTxdStore_SetupTxdParent 0x731D55
DWORD RETURN_CTxdStore_SetupTxdParent = 0x731D5B;
#define HOOKPOS_CTxdStore_RemoveTxd 0x731E90
DWORD RETURN_CTxdStore_RemoveTxd = 0x731E96;

//
// STxdStreamEvent stores the txd create and destroy events for the current frame
//
struct STxdStreamEvent
{
    STxdStreamEvent(bool bAdded, ushort usTxdId) : bAdded(bAdded), usTxdId(usTxdId) {}

    bool   bAdded;
    ushort usTxdId;
};

static std::vector<STxdStreamEvent> ms_txdStreamEventList;

////////////////////////////////////////////////////////////////
// Txd created
////////////////////////////////////////////////////////////////
__declspec(noinline) void _cdecl OnStreamingAddedTxd(DWORD dwTxdId)
{
    ushort usTxdId = (ushort)dwTxdId;
    // Remove any previous events for this txd (erase-remove idiom)
    ms_txdStreamEventList.erase(
        std::remove_if(ms_txdStreamEventList.begin(), ms_txdStreamEventList.end(),
                       [usTxdId](const STxdStreamEvent& e) { return e.usTxdId == usTxdId; }),
        ms_txdStreamEventList.end());
    // Append 'added'
    ms_txdStreamEventList.emplace_back(true, usTxdId);
}

// called from streaming on TXD create
static void _declspec(naked) HOOK_CTxdStore_SetupTxdParent()
{
    // clang-format off
    __asm
    {
        // Hooked from 731D55  6 bytes

        // eax - txd id
        pushad
        push    eax
        call    OnStreamingAddedTxd
        add     esp, 4
        popad

        // orig
        mov     esi, ds:00C8800Ch
        jmp     RETURN_CTxdStore_SetupTxdParent  // 731D5B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////
// Txd remove
////////////////////////////////////////////////////////////////
__declspec(noinline) void _cdecl OnStreamingRemoveTxd(DWORD dwTxdId)
{
    ushort usTxdId = (ushort)dwTxdId - pGame->GetBaseIDforTXD();
    // Remove any previous events for this txd (erase-remove idiom)
    ms_txdStreamEventList.erase(
        std::remove_if(ms_txdStreamEventList.begin(), ms_txdStreamEventList.end(),
                       [usTxdId](const STxdStreamEvent& e) { return e.usTxdId == usTxdId; }),
        ms_txdStreamEventList.end());
    // Append 'removed'
    ms_txdStreamEventList.emplace_back(false, usTxdId);
}

// called from streaming on TXD destroy
static void _declspec(naked) HOOK_CTxdStore_RemoveTxd()
{
    // clang-format off
    __asm
    {
        // Hooked from 731E90  6 bytes

        // esi - txd id + 20000
        pushad
        push    esi
        call    OnStreamingRemoveTxd
        add     esp, 4
        popad

        // orig
        mov     ecx, ds:00C8800Ch
        jmp     RETURN_CTxdStore_RemoveTxd      // 731E96
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::InitTextureWatchHooks
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::InitTextureWatchHooks()
{
    HookInstall(HOOKPOS_CTxdStore_SetupTxdParent, (DWORD)HOOK_CTxdStore_SetupTxdParent, 6);
    HookInstall(HOOKPOS_CTxdStore_RemoveTxd, (DWORD)HOOK_CTxdStore_RemoveTxd, 6);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// Process results of hooks
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::PulseWorldTextureWatch
//
// Process ms_txdStreamEventList
//
////////////////////////////////////////////////////////////////

// SEH-protected texture field access; returns false on access violation
static __declspec(noinline) bool TryReadTextureFields(RwTexture* texture, const char** ppName, CD3DDUMMY** ppD3DData)
{
    __try
    {
        *ppName = texture->name;
        RwRaster* pRaster = texture->raster;
        *ppD3DData = (pRaster != nullptr) ? static_cast<CD3DDUMMY*>(pRaster->renderResource) : nullptr;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        *ppName = nullptr;
        *ppD3DData = nullptr;
        return false;
    }
}

void CRenderWareSA::PulseWorldTextureWatch()
{
    UpdateModuleTickCount64();
    UpdateDisableGTAVertexShadersTimer();

    TIMING_CHECKPOINT("+TextureWatch");

    // Go through ms_txdStreamEventList
    for (std::vector<STxdStreamEvent>::const_iterator iter = ms_txdStreamEventList.begin(); iter != ms_txdStreamEventList.end(); ++iter)
    {
        const STxdStreamEvent& action = *iter;
        if (action.bAdded)
        {
            //
            // New txd has been loaded
            //

            // Get list of texture names and data to add

            std::vector<RwTexture*> textureList;
            GetTxdTextures(textureList, action.usTxdId);

            for (std::vector<RwTexture*>::iterator iter = textureList.begin(); iter != textureList.end(); iter++)
            {
                RwTexture* texture = *iter;
                if (!texture)
                    continue;

                // Read texture fields with SEH protection instead of per-field IsReadablePointer syscalls.
                // The TXD was just streamed in so its textures are normally valid; SEH catches rare corruption.
                const char* szTextureName = nullptr;
                CD3DDUMMY*  pD3DData = nullptr;
                if (!TryReadTextureFields(texture, &szTextureName, &pD3DData))
                    continue;

                if (!MapContains(m_SpecialTextures, texture))
                    StreamingAddedTexture(action.usTxdId, szTextureName, pD3DData);
            }
        }
        else
        {
            //
            // Txd has been unloaded
            //

            StreamingRemovedTxd(action.usTxdId);
        }
    }

    ms_txdStreamEventList.clear();

    m_pMatchChannelManager->PulseStaleEntityCacheCleanup();

    ProcessPendingIsolatedModels();
    TIMING_CHECKPOINT("-TextureWatch");
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StreamingAddedTexture
//
// Called when a TXD is loaded.
// Create a texinfo for the texture.
// Note: We register textures with their actual GTA internal name (e.g., "#emap").
// The pattern matching in AppendAdditiveMatch/AppendSubtractiveMatch handles
// mapping external names in scripts (e.g. "remap*") to internal names.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::StreamingAddedTexture(ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData)
{
    // Skip textures without valid D3D data or name - shader matching requires both
    if (!pD3DData || strTextureName.empty())
        return;

    STexInfo* pTexInfo = CreateTexInfo(usTxdId, strTextureName, pD3DData);
    OnTextureStreamIn(pTexInfo);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StreamingRemovedTxd
//
// Called when a TXD is being unloaded.
// Delete texinfos which came from that TXD
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::StreamingRemovedTxd(ushort usTxdId)
{
    TIMING_CHECKPOINT("+StreamingRemovedTxd");

    typedef std::multimap<ushort, STexInfo*>::const_iterator ConstIterType;
    std::pair<ConstIterType, ConstIterType>                  range = m_TexInfoMap.equal_range(usTxdId);
    for (ConstIterType iter = range.first; iter != range.second;)
    {
        STexInfo* pTexInfo = iter->second;
        if (pTexInfo && pTexInfo->texTag == usTxdId)
        {
            OnTextureStreamOut(pTexInfo);
            DestroyTexInfo(pTexInfo);
            m_TexInfoMap.erase(iter++);
        }
        else
            ++iter;
    }

    TIMING_CHECKPOINT("-StreamingRemovedTxd");
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveStreamingTexture
//
// Remove a single texture that was added via StreamingAddedTexture.
// Finds the texture by matching TXD ID and D3D data pointer.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveStreamingTexture(unsigned short usTxdId, CD3DDUMMY* pD3DData)
{
    if (!pD3DData)
        return;

    typedef std::multimap<ushort, STexInfo*>::iterator IterType;
    std::pair<IterType, IterType>                      range = m_TexInfoMap.equal_range(usTxdId);
    for (IterType iter = range.first; iter != range.second;)
    {
        STexInfo* pTexInfo = iter->second;
        if (pTexInfo && pTexInfo->pD3DData == pD3DData)
        {
            OnTextureStreamOut(pTexInfo);
            DestroyTexInfo(pTexInfo);
            m_TexInfoMap.erase(iter++);
            return;  // Only one entry per D3D data
        }
        else
            ++iter;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::IsTexInfoRegistered
//
// Check if a D3D data pointer is already registered in the shader system.
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::IsTexInfoRegistered(CD3DDUMMY* pD3DData) const
{
    if (!pD3DData)
        return false;
    return MapContains(m_D3DDataTexInfoMap, pD3DData);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ScriptAddedTxd
//
// Called when a TXD is loaded outside of streaming
// Create texinfos for the textures
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ScriptAddedTxd(RwTexDictionary* pTxd)
{
    TIMING_CHECKPOINT("+ScriptAddedTxd");

    // Validate TXD pointer before iterating
    if (!pTxd || !SharedUtil::IsReadablePointer(pTxd, sizeof(RwTexDictionary)))
    {
        TIMING_CHECKPOINT("-ScriptAddedTxd");
        return;
    }

    std::vector<RwTexture*> textureList;
    GetTxdTextures(textureList, pTxd);
    for (std::vector<RwTexture*>::iterator iter = textureList.begin(); iter != textureList.end(); iter++)
    {
        RwTexture* texture = *iter;
        if (!texture || !SharedUtil::IsReadablePointer(texture, sizeof(RwTexture)))
            continue;

        const char* szTextureName = texture->name;
        CD3DDUMMY*  pD3DData =
            (texture->raster && SharedUtil::IsReadablePointer(texture->raster, sizeof(RwRaster))) ? (CD3DDUMMY*)texture->raster->renderResource : NULL;

        if (!pD3DData || !szTextureName[0])
            continue;

        // Added texture
        STexInfo* pTexInfo = CreateTexInfo(texture, szTextureName, pD3DData);
        OnTextureStreamIn(pTexInfo);
    }
    TIMING_CHECKPOINT("-ScriptAddedTxd");
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ScriptRemovedTexture
//
// Called when a texture is destroyed outside of streaming
// Delete texinfo for that texture
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ScriptRemovedTexture(RwTexture* pTex)
{
    TIMING_CHECKPOINT("+ScriptRemovedTexture");

    // Use reverse lookup instead of full m_TexInfoMap scan
    auto it = m_ScriptTexInfoMap.find(pTex);
    if (it != m_ScriptTexInfoMap.end())
    {
        STexInfo* pTexInfo = it->second;
        if (pTexInfo)
        {
            OnTextureStreamOut(pTexInfo);
            // Erase from m_TexInfoMap by scanning the TXD ID bucket
            typedef std::multimap<ushort, STexInfo*>::iterator IterType;
            std::pair<IterType, IterType> range = m_TexInfoMap.equal_range(pTexInfo->texTag.m_usTxdId);
            for (IterType iter = range.first; iter != range.second; ++iter)
            {
                if (iter->second == pTexInfo)
                {
                    m_TexInfoMap.erase(iter);
                    break;
                }
            }
            DestroyTexInfo(pTexInfo);
        }
    }

    TIMING_CHECKPOINT("-ScriptRemovedTexture");
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::SpecialAddedTexture
//
// For game textures that are created (not loaded)
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::SpecialAddedTexture(RwTexture* texture, const char* szTextureName)
{
    if (!texture || !SharedUtil::IsReadablePointer(texture, sizeof(RwTexture)))
        return;

    if (!szTextureName)
        szTextureName = texture->name;
    if (!szTextureName || !szTextureName[0])
        return;

    CD3DDUMMY* pD3DData =
        (texture->raster && SharedUtil::IsReadablePointer(texture->raster, sizeof(RwRaster))) ? (CD3DDUMMY*)texture->raster->renderResource : NULL;

    if (!pD3DData)
        return;

    OutputDebug(SString("Adding special texture %s", szTextureName));

    // Added texture
    STexInfo* pTexInfo = CreateTexInfo(texture, szTextureName, pD3DData);
    OnTextureStreamIn(pTexInfo);

    MapInsert(m_SpecialTextures, texture);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::SpecialRemovedTexture
//
// For game textures that are created (not loaded)
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::SpecialRemovedTexture(RwTexture* pTex)
{
    if (!MapContains(m_SpecialTextures, pTex))
        return;

    OutputDebug(SString("Removing special texture (%s)", pTex->name));

    MapRemove(m_SpecialTextures, pTex);

    // Use reverse lookup instead of full m_TexInfoMap scan
    auto it = m_ScriptTexInfoMap.find(pTex);
    if (it != m_ScriptTexInfoMap.end())
    {
        STexInfo* pTexInfo = it->second;
        if (pTexInfo)
        {
            OutputDebug(SString("     %s", *pTexInfo->strTextureName));
            OnTextureStreamOut(pTexInfo);
            // Erase from m_TexInfoMap by scanning the TXD ID bucket
            typedef std::multimap<ushort, STexInfo*>::iterator IterType;
            std::pair<IterType, IterType> range = m_TexInfoMap.equal_range(pTexInfo->texTag.m_usTxdId);
            for (IterType iter = range.first; iter != range.second; ++iter)
            {
                if (iter->second == pTexInfo)
                {
                    m_TexInfoMap.erase(iter);
                    break;
                }
            }
            DestroyTexInfo(pTexInfo);
        }
    }
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// Texture watch processing
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::CreateTexInfo
//
//
//
////////////////////////////////////////////////////////////////
STexInfo* CRenderWareSA::CreateTexInfo(const STexTag& texTag, const SString& strTextureName, CD3DDUMMY* pD3DData)
{
    // Create texinfo
    STexInfo* pTexInfo = new STexInfo(texTag, strTextureName, pD3DData);

    // Add to map
    MapInsert(m_TexInfoMap, pTexInfo->texTag.m_usTxdId, pTexInfo);

    // Add to D3DData lookup map
    MapSet(m_D3DDataTexInfoMap, pTexInfo->pD3DData, pTexInfo);

    // Add to script texture reverse lookup if tagged by a real RwTexture*
    if (!texTag.m_bUsingTxdId && texTag.m_pTex && texTag.m_pTex != FAKE_RWTEXTURE_NO_TEXTURE)
        m_ScriptTexInfoMap[texTag.m_pTex] = pTexInfo;

    return pTexInfo;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::DestroyTexInfo
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::DestroyTexInfo(STexInfo* pTexInfo)
{
    if (!pTexInfo)
        return;

    // Only remove if this specific STexInfo is the registered one
    STexInfo* pCurrentEntry = MapFindRef(m_D3DDataTexInfoMap, pTexInfo->pD3DData);
    if (pCurrentEntry == pTexInfo)
    {
        MapRemove(m_D3DDataTexInfoMap, pTexInfo->pD3DData);
    }

    // Remove from script texture reverse lookup
    if (!pTexInfo->texTag.m_bUsingTxdId && pTexInfo->texTag.m_pTex && pTexInfo->texTag.m_pTex != FAKE_RWTEXTURE_NO_TEXTURE)
    {
        auto it = m_ScriptTexInfoMap.find(pTexInfo->texTag.m_pTex);
        if (it != m_ScriptTexInfoMap.end() && it->second == pTexInfo)
            m_ScriptTexInfoMap.erase(it);
    }

    delete pTexInfo;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::SetRenderingClientEntity
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::SetRenderingClientEntity(CClientEntityBase* pClientEntity, ushort usModelId, int iTypeMask)
{
    m_pRenderingClientEntity = pClientEntity;
    m_usRenderingEntityModelId = usModelId;
    m_iRenderingEntityType = iTypeMask;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetAppliedShaderForD3DData
//
//
//
////////////////////////////////////////////////////////////////
SShaderItemLayers* CRenderWareSA::GetAppliedShaderForD3DData(CD3DDUMMY* pD3DData)
{
    m_uiReplacementRequestCounter++;

    // If rendering with no texture, and doing an 3d model like render, use the 'unnamed' texinfo
    if (pD3DData == NULL && CRenderWareSA::ms_iRenderingType == RT_NONE)
        pD3DData = FAKE_D3DTEXTURE_NO_TEXTURE;

    STexInfo* pTexInfo = MapFindRef(m_D3DDataTexInfoMap, pD3DData);

    if (!pTexInfo)
        return NULL;

    SShaderInfoLayers* pInfoLayers = m_pMatchChannelManager->GetShaderForTexAndEntity(pTexInfo, m_pRenderingClientEntity, m_iRenderingEntityType);

    if (!pInfoLayers->output.pBase && pInfoLayers->output.layerList.empty())
        return NULL;

    if (m_iRenderingEntityType == TYPE_MASK_PED && pInfoLayers->output.bUsesVertexShader)
    {
        // If a possible conflict is detected, make sure GTA vertex shaders are disabled (effective from the next ped rendered)
        DisableGTAVertexShadersForAWhile();
    }

    m_uiReplacementMatchCounter++;

    return &pInfoLayers->output;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AppendAdditiveMatch
//
// Add additive match for a shader+entity combo
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::AppendAdditiveMatch(CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* szTextureNameMatch, float fShaderPriority,
                                        bool bShaderLayered, int iTypeMask, uint uiShaderCreateTime, bool bShaderUsesVertexShader, bool bAppendLayers)
{
    // NULL or empty pattern would cause issues
    if (!szTextureNameMatch || !szTextureNameMatch[0])
        return;

    TIMING_CHECKPOINT("+AppendAddMatch");

    // Make previous versions usage of "CJ" work with new way
    SString strTextureNameMatch = szTextureNameMatch;
    if (strTextureNameMatch.CompareI("cj"))
        strTextureNameMatch = "cj_ped_*";

    // Register the pattern as provided by script
    m_pMatchChannelManager->AppendAdditiveMatch(pShaderData, pClientEntity, strTextureNameMatch, fShaderPriority, bShaderLayered, iTypeMask, uiShaderCreateTime,
                                                bShaderUsesVertexShader, bAppendLayers);

    // Also register with internal texture name variant if pattern contains a known external name.
    // This handles the case where script uses external names (e.g. "remap") but GTA internally
    // renames textures (e.g., "#emap"). We register both patterns so the shader matches either.
    // Handles: "remap", "remap*", "*remap*", "vehicleremap", etc.
    SString strLower = strTextureNameMatch.ToLower();
    bool    bHasRemap = strLower.Contains("remap");
    bool    bHasWhite = strLower.Contains("white");

    // Check for "remap" anywhere in the pattern (case-insensitive)
    if (bHasRemap)
    {
        SString strInternalPattern = strTextureNameMatch.ReplaceI("remap", "#emap");
        // Only register if actually different (avoid duplicates)
        if (strInternalPattern != strTextureNameMatch)
        {
            m_pMatchChannelManager->AppendAdditiveMatch(pShaderData, pClientEntity, strInternalPattern, fShaderPriority, bShaderLayered, iTypeMask,
                                                        uiShaderCreateTime, bShaderUsesVertexShader, bAppendLayers);

            // If pattern also contains "white", register the doubly-transformed variant
            // e.g., "white_remap*" -> "@hite_#emap*"
            if (bHasWhite)
            {
                SString strBothInternal = strInternalPattern.ReplaceI("white", "@hite");
                if (strBothInternal != strInternalPattern)
                {
                    m_pMatchChannelManager->AppendAdditiveMatch(pShaderData, pClientEntity, strBothInternal, fShaderPriority, bShaderLayered, iTypeMask,
                                                                uiShaderCreateTime, bShaderUsesVertexShader, bAppendLayers);
                }
            }
        }
    }
    // Check for "white" anywhere in the pattern (case-insensitive)
    if (bHasWhite)
    {
        SString strInternalPattern = strTextureNameMatch.ReplaceI("white", "@hite");
        if (strInternalPattern != strTextureNameMatch)
        {
            m_pMatchChannelManager->AppendAdditiveMatch(pShaderData, pClientEntity, strInternalPattern, fShaderPriority, bShaderLayered, iTypeMask,
                                                        uiShaderCreateTime, bShaderUsesVertexShader, bAppendLayers);
        }
    }

    TIMING_CHECKPOINT("-AppendAddMatch");
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AppendSubtractiveMatch
//
// Add subtractive match for a shader+entity combo
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::AppendSubtractiveMatch(CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* szTextureNameMatch)
{
    // NULL or empty pattern would cause problems
    if (!szTextureNameMatch || !szTextureNameMatch[0])
        return;

    TIMING_CHECKPOINT("+AppendSubMatch");

    // Make previous versions usage of "CJ" work with new way
    SString strTextureNameMatch = szTextureNameMatch;
    if (strTextureNameMatch.CompareI("cj"))
        strTextureNameMatch = "cj_ped_*";

    // Register the pattern as provided by script
    m_pMatchChannelManager->AppendSubtractiveMatch(pShaderData, pClientEntity, strTextureNameMatch);

    // Also register with internal texture name variant (same logic as AppendAdditiveMatch)
    SString strLower = strTextureNameMatch.ToLower();
    bool    bHasRemap = strLower.Contains("remap");
    bool    bHasWhite = strLower.Contains("white");

    if (bHasRemap)
    {
        SString strInternalPattern = strTextureNameMatch.ReplaceI("remap", "#emap");
        if (strInternalPattern != strTextureNameMatch)
        {
            m_pMatchChannelManager->AppendSubtractiveMatch(pShaderData, pClientEntity, strInternalPattern);

            // If pattern also contains "white", register the doubly-transformed variant
            if (bHasWhite)
            {
                SString strBothInternal = strInternalPattern.ReplaceI("white", "@hite");
                if (strBothInternal != strInternalPattern)
                {
                    m_pMatchChannelManager->AppendSubtractiveMatch(pShaderData, pClientEntity, strBothInternal);
                }
            }
        }
    }
    if (bHasWhite)
    {
        SString strInternalPattern = strTextureNameMatch.ReplaceI("white", "@hite");
        if (strInternalPattern != strTextureNameMatch)
        {
            m_pMatchChannelManager->AppendSubtractiveMatch(pShaderData, pClientEntity, strInternalPattern);
        }
    }

    TIMING_CHECKPOINT("-AppendSubMatch");
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnTextureStreamIn
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnTextureStreamIn(STexInfo* pTexInfo)
{
    if (!pTexInfo)
        return;

    // Insert into all channels that match the name
    m_pMatchChannelManager->InsertTexture(pTexInfo);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnTextureStreamOut
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnTextureStreamOut(STexInfo* pTexInfo)
{
    if (!pTexInfo)
        return;

    m_pMatchChannelManager->RemoveTexture(pTexInfo);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveClientEntityRefs
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveClientEntityRefs(CClientEntityBase* pClientEntity)
{
    TIMING_CHECKPOINT("+RemoveEntityRefs");
    m_pMatchChannelManager->RemoveClientEntityRefs(pClientEntity);
    TIMING_CHECKPOINT("-RemoveEntityRefs");
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveShaderRefs
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveShaderRefs(CSHADERDUMMY* pShaderItem)
{
    TIMING_CHECKPOINT("+RemoveShaderRefs");
    m_pMatchChannelManager->RemoveShaderRefs(pShaderItem);
    TIMING_CHECKPOINT("-RemoveShaderRefs");
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ResetStats
//
// Save/reset counters
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ResetStats()
{
    m_uiNumReplacementRequests = m_uiReplacementRequestCounter;
    m_uiNumReplacementMatches = m_uiReplacementMatchCounter;
    m_uiReplacementRequestCounter = 0;
    m_uiReplacementMatchCounter = 0;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetShaderReplacementStats
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetShaderReplacementStats(SShaderReplacementStats& outStats)
{
    outStats.uiNumReplacementRequests = m_uiNumReplacementRequests;
    outStats.uiNumReplacementMatches = m_uiNumReplacementMatches;
    m_pMatchChannelManager->GetShaderReplacementStats(outStats);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::Initialize
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::Initialize()
{
    if (!MapContains(m_D3DDataTexInfoMap, FAKE_D3DTEXTURE_NO_TEXTURE))
    {
        // Make a fake texinfo to handle all non-textures
        STexInfo* pTexInfo = CreateTexInfo(FAKE_RWTEXTURE_NO_TEXTURE, FAKE_NAME_NO_TEXTURE, FAKE_D3DTEXTURE_NO_TEXTURE);
        OnTextureStreamIn(pTexInfo);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::DisableGTAVertexShadersForAWhile
//
// Disable GTA vertex shaders for the next 10 seconds
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::DisableGTAVertexShadersForAWhile()
{
    m_GTAVertexShadersDisabledTimer.Reset();
    SetGTAVertexShadersEnabled(false);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::UpdateDisableGTAVertexShadersTimer
//
// Update countdown before automatically re enabling GTA vertex shaders
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::UpdateDisableGTAVertexShadersTimer()
{
#if MTA_DEBUG
    if (m_GTAVertexShadersDisabledTimer.Get() > 1 * 1000)
#else
    if (m_GTAVertexShadersDisabledTimer.Get() > 10 * 1000)
#endif
    {
        SetGTAVertexShadersEnabled(true);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::SetGTAVertexShadersEnabled
//
// Set to false to disable GTA vertex shaders, so we can use our own ones
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::SetGTAVertexShadersEnabled(bool bEnable)
{
    if (m_bGTAVertexShadersEnabled == bEnable)
        return;

    m_bGTAVertexShadersEnabled = bEnable;

    DWORD pSkinAtomic = 0x07C7CD0;

    if (bEnable)
    {
        // Allow GTA vertex shaders (default)
        MemPut<BYTE>(pSkinAtomic + 0, 0x8B);  // mov  eax, [edi+20h]
        MemPut<BYTE>(pSkinAtomic + 1, 0x47);
        MemPut<BYTE>(pSkinAtomic + 2, 0x20);
        MemPut<BYTE>(pSkinAtomic + 3, 0x85);  // test eax, eax
        MemPut<BYTE>(pSkinAtomic + 4, 0xC0);
    }
    else
    {
        // Disallow GTA vertex shaders
        // This forces the current skin buffer to use software blending from now on
        MemPut<BYTE>(pSkinAtomic + 0, 0x33);  // xor  eax, eax
        MemPut<BYTE>(pSkinAtomic + 1, 0xC0);
        MemPut<BYTE>(pSkinAtomic + 2, 0x89);  // mov  dword ptr [edi+20h], eax
        MemPut<BYTE>(pSkinAtomic + 3, 0x47);
        MemPut<BYTE>(pSkinAtomic + 4, 0x20);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwTextureSetName
//
// Check each created texture, so we can add special ones to the shader system
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_RwTextureSetName(DWORD dwAddrCalledFrom, RwTexture* pTexture, const char* szName)
{
    SString strReplacementName;
    if (dwAddrCalledFrom == ADDR_CCustomCarPlateMgr_CreatePlateTexture_TextureSetName)
        strReplacementName = "custom_car_plate";
    else if (dwAddrCalledFrom == ADDR_CCustomRoadsignMgr_CreateRoadsignTexture_TextureSetName)
        strReplacementName = "custom_roadsign_text";
    else if (dwAddrCalledFrom > ADDR_CClothesBuilder_ConstructTextures_Start && dwAddrCalledFrom < ADDR_CClothesBuilder_ConstructTextures_End)
        strReplacementName = SString("cj_ped_%s", szName);

    if (!strReplacementName.empty())
        pGame->GetRenderWareSA()->SpecialAddedTexture(pTexture, strReplacementName);
}

// Hook info
#define HOOKPOS_RwTextureSetName  0x7F38A0
#define HOOKSIZE_RwTextureSetName 9
DWORD                        RETURN_RwTextureSetName = 0x7F38A9;
static void _declspec(naked) HOOK_RwTextureSetName()
{
    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnMY_RwTextureSetName
        add     esp, 4*3
        popad

        sub     esp, 8
        mov     ecx, ds:0x0C97B24
        jmp     RETURN_RwTextureSetName
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwTextureDestroy_Mid
//
// Check each destroyed texture, so we can remove special ones from the shader system
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_RwTextureDestroy_Mid(RwTexture* pTexture)
{
    pGame->GetRenderWareSA()->SpecialRemovedTexture(pTexture);
}

// Hook info
#define HOOKPOS_RwTextureDestroy_Mid  0x07F3834
#define HOOKSIZE_RwTextureDestroy_Mid 5
DWORD                        RETURN_RwTextureDestroy_Mid = 0x07F3839;
static void _declspec(naked) HOOK_RwTextureDestroy_Mid()
{
    // clang-format off
    __asm
    {
        pushad
        push    esi
        call    OnMY_RwTextureDestroy_Mid
        add     esp, 4*1
        popad

        push    0x08E23CC
        jmp     RETURN_RwTextureDestroy_Mid
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwIm3DRenderIndexedPrimitive
//
// Classify what is going on here
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_RwIm3DRenderIndexedPrimitive_Pre(DWORD dwAddrCalledFrom)
{
    if (dwAddrCalledFrom == ADDR_CVehicle_DoHeadLightBeam_RenderPrimitive || dwAddrCalledFrom == ADDR_CHeli_SearchLightCone_RenderPrimitive ||
        dwAddrCalledFrom == ADDR_CWaterCannon_Render_RenderPrimitive)
    {
        CRenderWareSA::ms_iRenderingType = RT_NONE;  // Treat these items like world models
    }
    else
    {
        CRenderWareSA::ms_iRenderingType = RT_3DI;
    }
}

__declspec(noinline) void OnMY_RwIm3DRenderIndexedPrimitive_Post(DWORD dwAddrCalledFrom)
{
    CRenderWareSA::ms_iRenderingType = RT_NONE;
}

// Hook info
#define HOOKPOS_RwIm3DRenderIndexedPrimitive  0x07EF550
#define HOOKSIZE_RwIm3DRenderIndexedPrimitive 5
DWORD                        RETURN_RwIm3DRenderIndexedPrimitive = 0x07EF555;
static void _declspec(naked) HOOK_RwIm3DRenderIndexedPrimitive()
{
    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm3DRenderIndexedPrimitive_Pre
        add     esp, 4*1
        popad

        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call    inner
        add     esp, 4*3

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm3DRenderIndexedPrimitive_Post
        add     esp, 4*1
        popad
        retn

        inner:
        mov     eax, ds:0x0C9C078
        jmp     RETURN_RwIm3DRenderIndexedPrimitive
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwIm3DRenderPrimitive
//
// Note that RwIm3DRenderPrimitive is being called to render something
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_RwIm3DRenderPrimitive_Pre(DWORD dwAddrCalledFrom)
{
    CRenderWareSA::ms_iRenderingType = RT_3DNI;
}

__declspec(noinline) void OnMY_RwIm3DRenderPrimitive_Post(DWORD dwAddrCalledFrom)
{
    CRenderWareSA::ms_iRenderingType = RT_NONE;
}

// Hook info
#define HOOKPOS_RwIm3DRenderPrimitive  0x07EF6B0
#define HOOKSIZE_RwIm3DRenderPrimitive 6
DWORD                        RETURN_RwIm3DRenderPrimitive = 0x07EF6B6;
static void _declspec(naked) HOOK_RwIm3DRenderPrimitive()
{
    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm3DRenderPrimitive_Pre
        add     esp, 4*1
        popad

        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call    inner
        add     esp, 4*3

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm3DRenderPrimitive_Post
        add     esp, 4*1
        popad
        retn

        inner:
        mov     ecx, ds:0x0C97B24
        jmp     RETURN_RwIm3DRenderPrimitive
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwIm2DRenderIndexedPrimitive
//
// Note that RwIm2DRenderIndexedPrimitive is being called to render something
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_RwIm2DRenderIndexedPrimitive_Pre(DWORD dwAddrCalledFrom)
{
    CRenderWareSA::ms_iRenderingType = RT_2DI;
}

__declspec(noinline) void OnMY_RwIm2DRenderIndexedPrimitive_Post(DWORD dwAddrCalledFrom)
{
    CRenderWareSA::ms_iRenderingType = RT_NONE;
}

// Hook info
#define HOOKPOS_RwIm2DRenderIndexedPrimitive  0x0734EA1
#define HOOKSIZE_RwIm2DRenderIndexedPrimitive 5
DWORD                        RETURN_RwIm2DRenderIndexedPrimitive = 0x0403927;
static void _declspec(naked) HOOK_RwIm2DRenderIndexedPrimitive()
{
    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm2DRenderIndexedPrimitive_Pre
        add     esp, 4*1
        popad

        push    [esp+4*5]
        push    [esp+4*5]
        push    [esp+4*5]
        push    [esp+4*5]
        push    [esp+4*5]
        call    inner
        add     esp, 4*5

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm2DRenderIndexedPrimitive_Post
        add     esp, 4*1
        popad
        retn

        inner:
        jmp     RETURN_RwIm2DRenderIndexedPrimitive
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwIm2DRenderPrimitive
//
// Note that RwIm2DRenderPrimitive is being called to render something
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_RwIm2DRenderPrimitive_Pre(DWORD dwAddrCalledFrom)
{
    CRenderWareSA::ms_iRenderingType = RT_2DNI;
}

__declspec(noinline) void OnMY_RwIm2DRenderPrimitive_Post(DWORD dwAddrCalledFrom)
{
    CRenderWareSA::ms_iRenderingType = RT_NONE;
}

// Hook info
#define HOOKPOS_RwIm2DRenderPrimitive  0x0734E90
#define HOOKSIZE_RwIm2DRenderPrimitive 5
DWORD                        RETURN_RwIm2DRenderPrimitive = 0x0734E95;
static void _declspec(naked) HOOK_RwIm2DRenderPrimitive()
{
    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm2DRenderPrimitive_Pre
        add     esp, 4*1
        popad

        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call    inner
        add     esp, 4*3

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm2DRenderPrimitive_Post
        add     esp, 4*1
        popad
        retn

        inner:
        mov     eax, ds:0x0C97B24
        jmp     RETURN_RwIm2DRenderPrimitive
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticSetHooks()
{
    EZHookInstall(RwTextureSetName);
    EZHookInstall(RwTextureDestroy_Mid);
    EZHookInstall(RwIm3DRenderIndexedPrimitive);
    EZHookInstall(RwIm3DRenderPrimitive);
    EZHookInstall(RwIm2DRenderIndexedPrimitive);
    EZHookInstall(RwIm2DRenderPrimitive);
}
