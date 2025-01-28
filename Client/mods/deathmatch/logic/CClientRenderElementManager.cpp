/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientRenderElementManager.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientVectorGraphic.h"

constexpr std::int64_t TEMPORARY_TEXTURES_CLEANUP_PERIOD = 5000ll;
constexpr std::int64_t TEMPORARY_TEXTURES_CLEANUP_THRESHOLD = 10000ll;
constexpr std::size_t TEMPORARY_TEXTURES_DELETE_THRESHOLD = 10u;

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CClientRenderElementManager
//
//
////////////////////////////////////////////////////////////////
CClientRenderElementManager::CClientRenderElementManager(CClientManager* pClientManager)
{
    m_pClientManager = pClientManager;
    m_pRenderItemManager = g_pCore->GetGraphics()->GetRenderItemManager();
    m_uiStatsDxFontCount = 0;
    m_uiStatsGuiFontCount = 0;
    m_uiStatsTextureCount = 0;
    m_uiStatsShaderCount = 0;
    m_uiStatsRenderTargetCount = 0;
    m_uiStatsScreenSourceCount = 0;
    m_uiStatsWebBrowserCount = 0;
    m_uiStatsVectorGraphicCount = 0;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::~CClientRenderElementManager
//
//
////////////////////////////////////////////////////////////////
CClientRenderElementManager::~CClientRenderElementManager()
{
    // Remove any existing
    while (m_ItemElementMap.size())
        Remove(m_ItemElementMap.begin()->second);
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateDxFont
//
//
//
////////////////////////////////////////////////////////////////
CClientDxFont* CClientRenderElementManager::CreateDxFont(const SString& strFullFilePath, uint uiSize, bool bBold, const DWORD ulQuality)
{
    // Create the item
    CDxFontItem* pDxFontItem = m_pRenderItemManager->CreateDxFont(strFullFilePath, uiSize, bBold, ulQuality);

    // Check create worked
    if (!pDxFontItem)
        return NULL;

    // Create the element
    CClientDxFont* pDxFontElement = new CClientDxFont(m_pClientManager, INVALID_ELEMENT_ID, pDxFontItem);

    // Add to this manager's list
    MapSet(m_ItemElementMap, pDxFontItem, pDxFontElement);

    // Update stats
    m_uiStatsDxFontCount++;

    return pDxFontElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateGuiFont
//
//
//
////////////////////////////////////////////////////////////////
CClientGuiFont* CClientRenderElementManager::CreateGuiFont(const SString& strFullFilePath, const SString& strUniqueName, uint uiSize)
{
    // Create the item
    CGuiFontItem* pGuiFontItem = m_pRenderItemManager->CreateGuiFont(strFullFilePath, strUniqueName, uiSize);

    // Check create worked
    if (!pGuiFontItem)
        return NULL;

    // Create the element
    CClientGuiFont* pGuiFontElement = new CClientGuiFont(m_pClientManager, INVALID_ELEMENT_ID, pGuiFontItem);

    // Add to this manager's list
    MapSet(m_ItemElementMap, pGuiFontItem, pGuiFontElement);

    // Update stats
    m_uiStatsGuiFontCount++;

    return pGuiFontElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateTexture
//
//
//
////////////////////////////////////////////////////////////////
CClientTexture* CClientRenderElementManager::CreateTexture(const SString& strFullFilePath, const CPixels* pPixels, bool bMipMaps, uint uiSizeX, uint uiSizeY,
                                                           ERenderFormat format, ETextureAddress textureAddress, ETextureType textureType, uint uiVolumeDepth)
{
    // Create the item
    CTextureItem* pTextureItem =
        m_pRenderItemManager->CreateTexture(strFullFilePath, pPixels, bMipMaps, uiSizeX, uiSizeY, format, textureAddress, textureType, uiVolumeDepth);

    // Check create worked
    if (!pTextureItem)
        return NULL;

    // Create the element
    CClientTexture* pTextureElement = new CClientTexture(m_pClientManager, INVALID_ELEMENT_ID, pTextureItem);

    // Add to this manager's list
    MapSet(m_ItemElementMap, pTextureItem, pTextureElement);

    // Update stats
    m_uiStatsTextureCount++;

    return pTextureElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateShader
//
//
//
////////////////////////////////////////////////////////////////
CClientShader* CClientRenderElementManager::CreateShader(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus,
                                                         float fPriority, float fMaxDistance, bool bLayered, bool bDebug, int iTypeMask,
                                                         const EffectMacroList& macros)
{
    // Create the item
    CShaderItem* pShaderItem =
        m_pRenderItemManager->CreateShader(strFile, strRootPath, bIsRawData, strOutStatus, fPriority, fMaxDistance, bLayered, bDebug, iTypeMask, macros);

    // Check create worked
    if (!pShaderItem)
        return NULL;

    // Create the element
    CClientShader* pShaderElement = new CClientShader(m_pClientManager, INVALID_ELEMENT_ID, pShaderItem);

    // Add to this manager's list
    MapSet(m_ItemElementMap, pShaderItem, pShaderElement);

    // Update stats
    m_uiStatsShaderCount++;

    return pShaderElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateRenderTarget
//
//
//
////////////////////////////////////////////////////////////////
CClientRenderTarget* CClientRenderElementManager::CreateRenderTarget(uint uiSizeX, uint uiSizeY, bool bHasSurfaceFormat, bool bWithAlphaChannel,
                                                                     _D3DFORMAT surfaceFormat)
{
    // Create the item
    CRenderTargetItem* pRenderTargetItem = m_pRenderItemManager->CreateRenderTarget(uiSizeX, uiSizeY, bHasSurfaceFormat, bWithAlphaChannel, surfaceFormat);

    // Check create worked
    if (!pRenderTargetItem)
        return NULL;

    // Create the element
    CClientRenderTarget* pRenderTargetElement = new CClientRenderTarget(m_pClientManager, INVALID_ELEMENT_ID, pRenderTargetItem);

    // Add to this manager's list
    MapSet(m_ItemElementMap, pRenderTargetItem, pRenderTargetElement);

    // Update stats
    m_uiStatsRenderTargetCount++;

    return pRenderTargetElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateScreenSource
//
//
//
////////////////////////////////////////////////////////////////
CClientScreenSource* CClientRenderElementManager::CreateScreenSource(uint uiSizeX, uint uiSizeY)
{
    // Create the item
    CScreenSourceItem* pScreenSourceItem = m_pRenderItemManager->CreateScreenSource(uiSizeX, uiSizeY);

    // Check create worked
    if (!pScreenSourceItem)
        return NULL;

    // Create the element
    CClientScreenSource* pScreenSourceElement = new CClientScreenSource(m_pClientManager, INVALID_ELEMENT_ID, pScreenSourceItem);

    // Add to this manager's list
    MapSet(m_ItemElementMap, pScreenSourceItem, pScreenSourceElement);

    // Update stats
    m_uiStatsScreenSourceCount++;

    return pScreenSourceElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateWebBrowser
//
//
//
////////////////////////////////////////////////////////////////
CClientWebBrowser* CClientRenderElementManager::CreateWebBrowser(uint uiSizeX, uint uiSizeY, bool bIsLocal, bool bTransparent)
{
    // Create the item
    CWebBrowserItem* pWebBrowserItem = m_pRenderItemManager->CreateWebBrowser(uiSizeX, uiSizeY);

    // Check create worked
    if (!pWebBrowserItem)
        return NULL;

    // Create the element
    CClientWebBrowser* pWebBrowserElement = new CClientWebBrowser(m_pClientManager, INVALID_ELEMENT_ID, pWebBrowserItem, bIsLocal, bTransparent);

    // Add to this manager's list
    MapSet(m_ItemElementMap, pWebBrowserItem, pWebBrowserElement);

    // Update stats
    m_uiStatsWebBrowserCount++;

    return pWebBrowserElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateVectorGraphic
//
//
//
////////////////////////////////////////////////////////////////
CClientVectorGraphic* CClientRenderElementManager::CreateVectorGraphic(uint width, uint height)
{
    // Create the item
    CVectorGraphicItem* pVectorGraphicItem = m_pRenderItemManager->CreateVectorGraphic(width, height);

    // Check create worked
    if (!pVectorGraphicItem)
        return nullptr;

    // Create the element
    CClientVectorGraphic* pVectorGraphicElement = new CClientVectorGraphic(m_pClientManager, INVALID_ELEMENT_ID, pVectorGraphicItem);

    // Add to this manager's list
    MapSet(m_ItemElementMap, pVectorGraphicElement->GetRenderItem(), pVectorGraphicElement);

    m_uiStatsVectorGraphicCount++;

    return pVectorGraphicElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::FindAutoTexture
//
// Find texture by unique name. Create if not found.
//
////////////////////////////////////////////////////////////////
CClientTexture* CClientRenderElementManager::FindAutoTexture(const SString& strFullFilePath, const SString& strUniqueName)
{
    // Check if we've already done this file
    SAutoTexture* ppTextureElement = MapFind(m_AutoTextureMap, strUniqueName);
    if (!ppTextureElement)
    {
        // Try to create
        CClientTexture* pNewTextureElement = CreateTexture(strFullFilePath);
        if (!pNewTextureElement)
            return nullptr;

        pNewTextureElement->MakeSystemEntity();

        // Add to automap if created
        MapSet(m_AutoTextureMap, strUniqueName, SAutoTexture{pNewTextureElement});
        ppTextureElement = MapFind(m_AutoTextureMap, strUniqueName);
    }

    ppTextureElement->lastUse = CTickCount::Now();

    return ppTextureElement->texture;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::Remove
//
// Called when an element is being deleted.
// Remove from lists and release render item
//
////////////////////////////////////////////////////////////////
void CClientRenderElementManager::Remove(CClientRenderElement* pElement)
{
    // Validate
    assert(pElement == *MapFind(m_ItemElementMap, pElement->GetRenderItem()));

    // Remove from this managers list
    MapRemove(m_ItemElementMap, pElement->GetRenderItem());

    // Remove from auto texture map
    if (pElement->IsA(CClientTexture::GetClassId()))
    {
        for (auto iter = m_AutoTextureMap.begin(); iter != m_AutoTextureMap.end(); ++iter)
        {
            if (iter->second.texture == pElement)
            {
                m_AutoTextureMap.erase(iter);
                break;
            }
        }
    }

    // Update stats
    if (pElement->IsA(CClientDxFont::GetClassId()))
        m_uiStatsDxFontCount--;
    else if (pElement->IsA(CClientGuiFont::GetClassId()))
        m_uiStatsGuiFontCount--;
    else if (pElement->IsA(CClientShader::GetClassId()))
        m_uiStatsShaderCount--;
    else if (pElement->IsA(CClientRenderTarget::GetClassId()))
        m_uiStatsRenderTargetCount--;
    else if (pElement->IsA(CClientScreenSource::GetClassId()))
        m_uiStatsScreenSourceCount--;
    else if (pElement->IsA(CClientWebBrowser::GetClassId()))
        m_uiStatsWebBrowserCount--;
    else if (pElement->IsA(CClientVectorGraphic::GetClassId()))
        m_uiStatsVectorGraphicCount--;
    else if (pElement->IsA(CClientTexture::GetClassId()))
        m_uiStatsTextureCount--;

    // Release render item
    CRenderItem* pRenderItem = pElement->GetRenderItem();
    SAFE_RELEASE(pRenderItem);
}

void CClientRenderElementManager::DoPulse()
{
    if (m_texturePulseTimer.Get() < TEMPORARY_TEXTURES_CLEANUP_PERIOD)
        return;

    m_texturePulseTimer.Reset();

    const CTickCount now = CTickCount::Now();

    std::vector<CClientTexture*> deleteCandidates;
    deleteCandidates.reserve(TEMPORARY_TEXTURES_DELETE_THRESHOLD);

    for (const auto& [texName, texInfo] : m_AutoTextureMap)
    {
        const std::int64_t timeElapsedMs = (now - texInfo.lastUse).ToLongLong();
        if (timeElapsedMs < TEMPORARY_TEXTURES_CLEANUP_THRESHOLD)
            continue;

        CTextureItem* textureItem = texInfo.texture->GetTextureItem();
        if (textureItem && textureItem->m_iRefCount > 1)
            continue;

        // CElementDeleter::Delete causes changes in m_AutoTextureMap
        // and we cannot delete a texture while iterating over it
        deleteCandidates.push_back(texInfo.texture);

        // The deletion procedure can be expensive
        // and we're interested in capping on the number of deleted texture at once
        if (deleteCandidates.size() == TEMPORARY_TEXTURES_DELETE_THRESHOLD)
            break;
    }   

    for (CClientTexture* texture : deleteCandidates)
        g_pClientGame->GetElementDeleter()->Delete(texture);   
}
