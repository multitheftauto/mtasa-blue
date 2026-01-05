/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRenderWareSA.h"

#ifdef MTA_DEBUG
// Comment out the lines below for your desired debug options
// #define SHADER_DEBUG_CHECKS         // Do lots of validation which can slow a debug build
// #define SHADER_DEBUG_OUTPUT         // Output lots of debug strings
#endif

// Various dodgy pointer values
#define FAKE_D3DTEXTURE_EMPTY_KEY   ( (CD3DDUMMY*)-1 )
#define FAKE_D3DTEXTURE_DELETED_KEY ( (CD3DDUMMY*)-2 )
#define FAKE_D3DTEXTURE_NO_TEXTURE  ( (CD3DDUMMY*)-9 )
#define FAKE_RWTEXTURE_NO_TEXTURE   ( (RwTexture*)-10 )
#define FAKE_NAME_NO_TEXTURE        "unnamed"

// CFastHashMap key funcs for CD3DDUMMY* - must be declared before CFastHashMap<CD3DDUMMY*, ...> is used
// These override the default template keys (-3, -4) to use our values
inline CD3DDUMMY* GetEmptyMapKey(CD3DDUMMY**)
{
    return FAKE_D3DTEXTURE_EMPTY_KEY;
}

inline CD3DDUMMY* GetDeletedMapKey(CD3DDUMMY**)
{
    return FAKE_D3DTEXTURE_DELETED_KEY;
}

class CMatchChannel;
class CMatchChannelManager;
struct STexNameInfo;

//
// Info on how a texture is identified.
//
struct STexTag
{
    STexTag(ushort usTxdId) : m_bUsingTxdId(true), m_usTxdId(usTxdId), m_pTex(nullptr) {}

    STexTag(RwTexture* pTex) : m_bUsingTxdId(false), m_usTxdId(0), m_pTex(pTex) {}

    [[nodiscard]] bool operator==(ushort usTxdId) const { return m_bUsingTxdId && usTxdId == m_usTxdId; }

    [[nodiscard]] bool operator==(RwTexture* pTex) const { return !m_bUsingTxdId && pTex == m_pTex; }

    const bool       m_bUsingTxdId;
    const ushort     m_usTxdId;            // Streamed textures are identified using the TXD id
    const RwTexture* m_pTex;               // Custom textures are identified using the RwTexture pointer
};

struct SOrderValue
{
    SOrderValue(float fOrderPriority, uint uiShaderCreateTime) : fOrderPriority(fOrderPriority), uiShaderCreateTime(uiShaderCreateTime) {}

    // Less than means higher priority
    [[nodiscard]] constexpr bool operator<(const SOrderValue& other) const
    {
        return fOrderPriority < other.fOrderPriority || (fOrderPriority == other.fOrderPriority && uiShaderCreateTime < other.uiShaderCreateTime);
    }

    const float fOrderPriority;            // Lower is higher priority
    const uint  uiShaderCreateTime;
};

//
// Info about an active shader which can replace textures
//
struct SShaderInfo
{
    SShaderInfo(CSHADERDUMMY* pShaderData, float fOrderPriority, bool bLayered, int iTypeMask, uint uiShaderCreateTime, bool bUsesVertexShader)
        : pShaderData(pShaderData),
          orderValue(fOrderPriority, uiShaderCreateTime),
          bLayered(bLayered),
          iTypeMask(iTypeMask),
          bUsesVertexShader(bUsesVertexShader)
    {
    }

    CSHADERDUMMY* const pShaderData;
    const SOrderValue   orderValue;
    const bool          bLayered;
    const int           iTypeMask;
    const bool          bUsesVertexShader;
};

//
// Info about a streamed in texture
//
struct STexInfo
{
    STexInfo(const STexTag& texTag, const SString& strTextureName, CD3DDUMMY* pD3DData)
        : texTag(texTag), strTextureName(strTextureName.ToLower()), pD3DData(pD3DData), pAssociatedTexNameInfo(nullptr)
    {
    }
    STexTag          texTag;
    const SString    strTextureName;            // Always lower case
    CD3DDUMMY* const pD3DData;
    STexNameInfo*    pAssociatedTexNameInfo;
};

struct SShaderInfoInstance
{
    SShaderInfoInstance() : pShaderInfo(nullptr), bMixEntityAndNonEntity(false) {}

    SShaderInfoInstance(SShaderInfo* pShaderInfo, bool bMixEntityAndNonEntity) : pShaderInfo(pShaderInfo), bMixEntityAndNonEntity(bMixEntityAndNonEntity) {}

    SShaderInfo* pShaderInfo;
    bool         bMixEntityAndNonEntity;

#ifdef SHADER_DEBUG_CHECKS
    [[nodiscard]] bool operator==(const SShaderInfoInstance& other) const
    {
        return pShaderInfo == other.pShaderInfo && bMixEntityAndNonEntity == other.bMixEntityAndNonEntity;
    }
#endif

    [[nodiscard]] bool operator<(const SShaderInfoInstance& other) const { return pShaderInfo->orderValue < other.pShaderInfo->orderValue; }
};

struct SShaderInfoLayers
{
    SShaderInfoInstance              pBase;
    std::vector<SShaderInfoInstance> layerList;
    SShaderItemLayers                output;            // For renderer

#ifdef SHADER_DEBUG_CHECKS
    [[nodiscard]] bool operator==(const SShaderInfoLayers& other) const
    {
        return pBase.pShaderInfo == other.pBase.pShaderInfo && pBase.bMixEntityAndNonEntity == other.pBase.bMixEntityAndNonEntity &&
               layerList == other.layerList;
    }
#endif
};

//
// Shader replacement, sort of cached
//
struct STexShaderReplacement
{
    STexShaderReplacement() : bSet(false), bValid(true) {}
    bool              bSet;
    bool              bValid;
    SShaderInfoLayers shaderLayers;
};

//
// Info about a persistent texture
//
struct STexNameInfo
{
    STexNameInfo(const SString& strTextureName) : strTextureName(strTextureName.ToLower())
    {
#ifdef SHADER_DEBUG_CHECKS
        iDebugCounter1 = 0;
        iDebugCounter2 = 0;
#endif
    }

    void ResetReplacementResults()
    {
        // Mark invalid without clearing bSet - preserves data for safe renderer access
        // Data will be rebuilt on next access when bValid=false is detected
        for (auto& shader : texNoEntityShaders)
        {
            shader.bValid = false;
        }
        for (auto& [entity, replacement] : texEntityShaderMap)
        {
            replacement.bValid = false;
        }
    }

    // Remove entries that have been marked invalid (deferred cleanup)
    void CleanupInvalidatedEntries()
    {
        // Remove invalidated entity shader mappings
        for (auto iter = texEntityShaderMap.begin(); iter != texEntityShaderMap.end();)
        {
            if (!iter->second.bValid)
            {
                texEntityShaderMap.erase(iter++);
            }
            else
                ++iter;
        }
    }

    [[nodiscard]] STexShaderReplacement& GetTexNoEntityShader(int iEntityType)
    {
        static constexpr char table[] = {-1, 0, 1, -1, 2, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, 4};
        dassert(iEntityType >= 0 && iEntityType < static_cast<int>(std::size(table)));
        const int idx = table[iEntityType];
        dassert(iEntityType >= TYPE_MASK_WORLD && iEntityType <= TYPE_MASK_OTHER && iEntityType == (1 << idx));
        return texNoEntityShaders[idx];
    }

    const SString           strTextureName;            // Always lower case
    CFastHashSet<STexInfo*> usedByTexInfoList;

    CFastHashSet<CMatchChannel*>                            matchChannelList;
    STexShaderReplacement                                   texNoEntityShaders[5];            // 0 - world  1-ped  2-vehicle  3-object  4-other
    CFastHashMap<CClientEntityBase*, STexShaderReplacement> texEntityShaderMap;

#ifdef SHADER_DEBUG_CHECKS
    int iDebugCounter1;
    int iDebugCounter2;
#endif
};
