/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItemManager.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <game/CRenderWare.h>
#include <game/CSettings.h>
#include "CRenderItem.EffectCloner.h"

// Type of vertex used to emulate StretchRect for SwiftShader bug
struct SRTVertex
{
    static const uint FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
    float             x, y, z, w;
    float             u, v;
};

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CRenderItemManager
//
//
//
////////////////////////////////////////////////////////////////
CRenderItemManager::CRenderItemManager()
{
    m_pEffectCloner = new CEffectCloner(this);
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::~CRenderItemManager
//
//
//
////////////////////////////////////////////////////////////////
CRenderItemManager::~CRenderItemManager()
{
    SAFE_DELETE(m_pEffectCloner);
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
    m_uiDefaultViewportSizeX = fViewportSizeX;
    m_uiDefaultViewportSizeY = fViewportSizeY;

    m_pRenderWare = CCore::GetSingleton().GetGame()->GetRenderWare();

    // Get some stats
    m_strVideoCardName = (const char*)g_pDeviceState->AdapterState.Name;
    m_iVideoCardMemoryKBTotal = g_pDeviceState->AdapterState.InstalledMemoryKB;

    m_iVideoCardMemoryKBForMTATotal = (m_iVideoCardMemoryKBTotal - (64 * 1024)) * 4 / 5;
    m_iVideoCardMemoryKBForMTATotal = std::max(0, m_iVideoCardMemoryKBForMTATotal);

    D3DCAPS9 caps;
    pDevice->GetDeviceCaps(&caps);
    int iMinor = caps.PixelShaderVersion & 0xFF;
    int iMajor = (caps.PixelShaderVersion & 0xFF00) >> 8;
    m_strVideoCardPSVersion = SString("%d", iMajor);
    if (iMinor)
        m_strVideoCardPSVersion += SString(".%d", iMinor);

    UpdateMemoryUsage();

    // Check if using SwiftShader dll
    SLibVersionInfo libVersionInfo;
    if (GetLibVersionInfo("d3d9.dll", &libVersionInfo))
    {
        if (libVersionInfo.strProductName.ContainsI("Swift"))
            m_bIsSwiftShader = true;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::OnLostDevice()
{
    for (std::set<CRenderItem*>::iterator iter = m_CreatedItemList.begin(); iter != m_CreatedItemList.end(); iter++)
        (*iter)->OnLostDevice();

    SAFE_RELEASE(m_pSavedSceneDepthSurface);
    SAFE_RELEASE(m_pSavedSceneRenderTargetAA);
    SAFE_RELEASE(g_pDeviceState->MainSceneState.DepthBuffer);
    SAFE_RELEASE(m_pNonAARenderTargetTexture);
    SAFE_RELEASE(m_pNonAARenderTarget);
    SAFE_RELEASE(m_pNonAADepthSurface2);
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::OnResetDevice()
{
    for (std::set<CRenderItem*>::iterator iter = m_CreatedItemList.begin(); iter != m_CreatedItemList.end(); iter++)
        (*iter)->OnResetDevice();

    UpdateMemoryUsage();
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateTexture
//
// TODO: Make underlying data for textures shared
//
////////////////////////////////////////////////////////////////
CTextureItem* CRenderItemManager::CreateTexture(const SString& strFullFilePath, const CPixels* pPixels, bool bMipMaps, uint uiSizeX, uint uiSizeY,
                                                ERenderFormat format, ETextureAddress textureAddress, ETextureType textureType, uint uiVolumeDepth)
{
    CFileTextureItem* pTextureItem = new CFileTextureItem();
    pTextureItem->PostConstruct(this, strFullFilePath, pPixels, bMipMaps, uiSizeX, uiSizeY, format, textureAddress, textureType, uiVolumeDepth);

    if (!pTextureItem->IsValid())
    {
        SAFE_RELEASE(pTextureItem);
        return nullptr;
    }

    UpdateMemoryUsage();

    return pTextureItem;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateVectorGraphic
//
//
//
////////////////////////////////////////////////////////////////
CVectorGraphicItem* CRenderItemManager::CreateVectorGraphic(uint width, uint height)
{
    if (!CanCreateRenderItem(CVectorGraphicItem::GetClassId()))
        return nullptr;

    CVectorGraphicItem* pVectorItem = new CVectorGraphicItem;
    pVectorItem->PostConstruct(this, width, height);

    if (!pVectorItem->IsValid())
    {
        SAFE_RELEASE(pVectorItem);
        return nullptr;
    }

    UpdateMemoryUsage();

    return pVectorItem;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateRenderTarget
//
//
//
////////////////////////////////////////////////////////////////
CRenderTargetItem* CRenderItemManager::CreateRenderTarget(uint uiSizeX, uint uiSizeY, bool bHasSurfaceFormat, bool bWithAlphaChannel, int surfaceFormat,
                                                          bool bForce)
{
    if (!bForce && !CanCreateRenderItem(CRenderTargetItem::GetClassId()))
        return NULL;

    // Include in memory stats only if render target is not for MTA internal use
    bool bIncludeInMemoryStats = (bForce == false);

    CRenderTargetItem* pRenderTargetItem = new CRenderTargetItem();
    pRenderTargetItem->PostConstruct(this, uiSizeX, uiSizeY, bHasSurfaceFormat, bWithAlphaChannel, surfaceFormat, bIncludeInMemoryStats);

    if (!pRenderTargetItem->IsValid())
    {
        SAFE_RELEASE(pRenderTargetItem);
        return NULL;
    }

    UpdateMemoryUsage();

    return pRenderTargetItem;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateScreenSource
//
//
//
////////////////////////////////////////////////////////////////
CScreenSourceItem* CRenderItemManager::CreateScreenSource(uint uiSizeX, uint uiSizeY)
{
    if (!CanCreateRenderItem(CScreenSourceItem::GetClassId()))
        return NULL;

    CScreenSourceItem* pScreenSourceItem = new CScreenSourceItem();
    pScreenSourceItem->PostConstruct(this, uiSizeX, uiSizeY);

    if (!pScreenSourceItem->IsValid())
    {
        SAFE_RELEASE(pScreenSourceItem);
        return NULL;
    }

    UpdateMemoryUsage();

    return pScreenSourceItem;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateWebBrowser
//
//
//
////////////////////////////////////////////////////////////////
CWebBrowserItem* CRenderItemManager::CreateWebBrowser(uint uiSizeX, uint uiSizeY)
{
    if (!CanCreateRenderItem(CWebBrowserItem::GetClassId()))
        return NULL;

    CWebBrowserItem* pWebBrowserItem = new CWebBrowserItem;
    pWebBrowserItem->PostConstruct(this, uiSizeX, uiSizeY);

    if (!pWebBrowserItem->IsValid())
    {
        SAFE_RELEASE(pWebBrowserItem);
        return NULL;
    }

    UpdateMemoryUsage();

    return pWebBrowserItem;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateShader
//
// Create a D3DX effect from .fx file
//
////////////////////////////////////////////////////////////////
CShaderItem* CRenderItemManager::CreateShader(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, float fPriority,
                                              float fMaxDistance, bool bLayered, bool bDebug, int iTypeMask, const EffectMacroList& macros)
{
    if (!CanCreateRenderItem(CShaderItem::GetClassId()))
        return NULL;

    strOutStatus = "";

    CShaderItem* pShaderItem = new CShaderItem();
    pShaderItem->PostConstruct(this, strFile, strRootPath, bIsRawData, strOutStatus, fPriority, fMaxDistance, bLayered, bDebug, iTypeMask, macros);

    if (!pShaderItem->IsValid())
    {
        SAFE_RELEASE(pShaderItem);
        return NULL;
    }

    return pShaderItem;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateDxFont
//
// TODO: Make underlying data for fonts shared
//
////////////////////////////////////////////////////////////////
CDxFontItem* CRenderItemManager::CreateDxFont(const SString& strFullFilePath, uint uiSize, bool bBold, DWORD ulQuality)
{
    if (!CanCreateRenderItem(CDxFontItem::GetClassId()))
        return NULL;

    CDxFontItem* pDxFontItem = new CDxFontItem();
    pDxFontItem->PostConstruct(this, strFullFilePath, uiSize, bBold, ulQuality);

    if (!pDxFontItem->IsValid())
    {
        SAFE_RELEASE(pDxFontItem);
        return NULL;
    }

    UpdateMemoryUsage();

    return pDxFontItem;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateGuiFont
//
// TODO: Make underlying data for fonts shared
//
////////////////////////////////////////////////////////////////
CGuiFontItem* CRenderItemManager::CreateGuiFont(const SString& strFullFilePath, const SString& strFontName, uint uiSize)
{
    if (!CanCreateRenderItem(CGuiFontItem::GetClassId()))
        return NULL;

    CGuiFontItem* pGuiFontItem = new CGuiFontItem();
    pGuiFontItem->PostConstruct(this, strFullFilePath, strFontName, uiSize);

    if (!pGuiFontItem->IsValid())
    {
        SAFE_RELEASE(pGuiFontItem);
        return NULL;
    }

    UpdateMemoryUsage();

    return pGuiFontItem;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::NotifyContructRenderItem
//
// Add to managers list
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::NotifyContructRenderItem(CRenderItem* pItem)
{
    assert(!MapContains(m_CreatedItemList, pItem));
    MapInsert(m_CreatedItemList, pItem);

    if (CScreenSourceItem* pScreenSourceItem = DynamicCast<CScreenSourceItem>(pItem))
        m_bBackBufferCopyMaybeNeedsResize = true;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::NotifyDestructRenderItem
//
// Remove from managers list
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::NotifyDestructRenderItem(CRenderItem* pItem)
{
    assert(MapContains(m_CreatedItemList, pItem));
    MapRemove(m_CreatedItemList, pItem);

    if (CScreenSourceItem* pScreenSourceItem = DynamicCast<CScreenSourceItem>(pItem))
        m_bBackBufferCopyMaybeNeedsResize = true;
    else if (CShaderItem* pShaderItem = DynamicCast<CShaderItem>(pItem))
        RemoveShaderItemFromWatchLists(pShaderItem);

    UpdateMemoryUsage();
}

//
//
// Render targets and back buffers
//
//

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::DoPulse
//
// Update stuff
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::DoPulse()
{
    m_pRenderWare->PulseWorldTextureWatch();

    m_PrevFrameTextureUsage = m_FrameTextureUsage;
    m_FrameTextureUsage.clear();

    m_pEffectCloner->DoPulse();

    UpdateBackBufferCopy();
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateBackBufferCopy
//
// Save back buffer pixels in our special place
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateBackBufferCopy()
{
    if (m_bBackBufferCopyMaybeNeedsResize)
        UpdateBackBufferCopySize();

    // Don't bother doing this unless at least one screen source in active
    if (!m_pBackBufferCopy)
        return;

    // Try to get the back buffer
    IDirect3DSurface9* pD3DBackBufferSurface = NULL;
    m_pDevice->GetRenderTarget(0, &pD3DBackBufferSurface);
    if (!pD3DBackBufferSurface)
        return;

    // Copy back buffer into our private render target
    D3DTEXTUREFILTERTYPE FilterType = D3DTEXF_LINEAR;
    HRESULT              hr = m_pDevice->StretchRect(pD3DBackBufferSurface, NULL, m_pBackBufferCopy->m_pD3DRenderTargetSurface, NULL, FilterType);

    m_uiBackBufferCopyRevision++;

    // Clean up
    SAFE_RELEASE(pD3DBackBufferSurface);
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateScreenSource
//
// Copy from back buffer store to screen source
// TODO - Optimize the case where the screen source is the same size as the back buffer copy (i.e. Use back buffer copy resources instead)
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateScreenSource(CScreenSourceItem* pScreenSourceItem, bool bResampleNow)
{
    if (bResampleNow)
    {
        // Tell graphics things are about to change
        CGraphics::GetSingleton().OnChangingRenderTarget(m_uiDefaultViewportSizeX, m_uiDefaultViewportSizeY);

        // Try to get the back buffer
        IDirect3DSurface9* pD3DBackBufferSurface = NULL;
        m_pDevice->GetRenderTarget(0, &pD3DBackBufferSurface);
        if (!pD3DBackBufferSurface)
            return;

        // Copy back buffer into our private render target
        D3DTEXTUREFILTERTYPE FilterType = D3DTEXF_LINEAR;
        HRESULT              hr = m_pDevice->StretchRect(pD3DBackBufferSurface, NULL, pScreenSourceItem->m_pD3DRenderTargetSurface, NULL, FilterType);

        // Clean up
        SAFE_RELEASE(pD3DBackBufferSurface);
        return;
    }

    // Only do update if back buffer copy has changed
    if (pScreenSourceItem->m_uiRevision == m_uiBackBufferCopyRevision)
        return;

    pScreenSourceItem->m_uiRevision = m_uiBackBufferCopyRevision;

    if (m_pBackBufferCopy)
    {
        D3DTEXTUREFILTERTYPE FilterType = D3DTEXF_LINEAR;
        m_pDevice->StretchRect(m_pBackBufferCopy->m_pD3DRenderTargetSurface, NULL, pScreenSourceItem->m_pD3DRenderTargetSurface, NULL, FilterType);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateBackBufferCopySize
//
// Create/resize/destroy back buffer copy depending on what is required
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateBackBufferCopySize()
{
    m_bBackBufferCopyMaybeNeedsResize = false;

    // Set what the max size requirement is for the back buffer copy
    uint uiSizeX = 0;
    uint uiSizeY = 0;
    for (std::set<CRenderItem*>::iterator iter = m_CreatedItemList.begin(); iter != m_CreatedItemList.end(); iter++)
    {
        if (CScreenSourceItem* pScreenSourceItem = DynamicCast<CScreenSourceItem>(*iter))
        {
            uiSizeX = std::max(uiSizeX, pScreenSourceItem->m_uiSizeX);
            uiSizeY = std::max(uiSizeY, pScreenSourceItem->m_uiSizeY);
        }
    }

    // Update?
    if (!m_pBackBufferCopy || m_pBackBufferCopy->m_uiSizeX != uiSizeX || m_pBackBufferCopy->m_uiSizeY != uiSizeY)
    {
        // Delete old one if it exists
        SAFE_RELEASE(m_pBackBufferCopy);

        // Try to create new one if needed
        if (uiSizeX > 0)
            m_pBackBufferCopy = CreateRenderTarget(uiSizeX, uiSizeY, false, false, (_D3DFORMAT)0, true);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetRenderTarget
//
// Set current render target to a custom one
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetRenderTarget(CRenderTargetItem* pItem, bool bClear)
{
    if (!m_pDefaultD3DRenderTarget)
        SaveDefaultRenderTarget();

    ChangeRenderTarget(pItem->m_uiSizeX, pItem->m_uiSizeY, pItem->m_pD3DRenderTargetSurface, pItem->m_pD3DZStencilSurface);

    if (bClear)
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);

    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::IsSetRenderTargetEnabledOldVer
//
// See if in enabled zones for old versions
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::IsSetRenderTargetEnabledOldVer()
{
    return m_bSetRenderTargetEnabledOldVer;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::EnableSetRenderTargetOldVer
//
// Enabled zones for old versions
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::EnableSetRenderTargetOldVer(bool bEnable)
{
    m_bSetRenderTargetEnabledOldVer = bEnable;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SaveDefaultRenderTarget
//
// Save current render target as the default one
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SaveDefaultRenderTarget()
{
    // Make sure any special depth buffer has been removed
    SaveReadableDepthBuffer();

    // Update our info about what rendertarget is active
    IDirect3DSurface9* pActiveD3DRenderTarget;
    IDirect3DSurface9* pActiveD3DZStencilSurface;
    m_pDevice->GetRenderTarget(0, &pActiveD3DRenderTarget);
    m_pDevice->GetDepthStencilSurface(&pActiveD3DZStencilSurface);

    m_pDefaultD3DRenderTarget = pActiveD3DRenderTarget;
    m_pDefaultD3DZStencilSurface = pActiveD3DZStencilSurface;

    // Don't hold any refs because it goes all funny during fullscreen minimize/maximize, even if they are released at onLostDevice
    SAFE_RELEASE(pActiveD3DRenderTarget)
    SAFE_RELEASE(pActiveD3DZStencilSurface)

    // Do this in case dxSetRenderTarget is being called from some unexpected place
    CGraphics::GetSingleton().MaybeEnteringMTARenderZone();
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::RestoreDefaultRenderTarget
//
// Set render target back to the default one
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::RestoreDefaultRenderTarget()
{
    // Only need to change if we have info
    if (m_pDefaultD3DRenderTarget)
    {
        ChangeRenderTarget(m_uiDefaultViewportSizeX, m_uiDefaultViewportSizeY, m_pDefaultD3DRenderTarget, m_pDefaultD3DZStencilSurface);
        m_pDefaultD3DRenderTarget = NULL;

        // Do this in case dxSetRenderTarget is being called from some unexpected place
        CGraphics::GetSingleton().MaybeLeavingMTARenderZone();
    }

    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::IsUsingDefaultRenderTarget
//
// Check if currently drawing to the default render target
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::IsUsingDefaultRenderTarget()
{
    // If this is NULL, it means we haven't saved it, so aren't using another render target
    return m_pDefaultD3DRenderTarget == NULL;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ChangeRenderTarget
//
// Worker function to change the D3D render target
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ChangeRenderTarget(uint uiSizeX, uint uiSizeY, IDirect3DSurface9* pD3DRenderTarget, IDirect3DSurface9* pD3DZStencilSurface)
{
    // Make sure any special depth buffer has been removed
    SaveReadableDepthBuffer();

    // Check if we need to change
    IDirect3DSurface9* pCurrentRenderTarget;
    IDirect3DSurface9* pCurrentZStencilSurface;
    m_pDevice->GetRenderTarget(0, &pCurrentRenderTarget);
    m_pDevice->GetDepthStencilSurface(&pCurrentZStencilSurface);

    bool bAlreadySet = (pD3DRenderTarget == pCurrentRenderTarget && pD3DZStencilSurface == pCurrentZStencilSurface);

    SAFE_RELEASE(pCurrentRenderTarget);
    SAFE_RELEASE(pCurrentZStencilSurface);

    // Already set?
    if (bAlreadySet)
        return;

    // Tell graphics things are about to change
    CGraphics::GetSingleton().OnChangingRenderTarget(uiSizeX, uiSizeY);

    // Do change
    m_pDevice->SetRenderTarget(0, pD3DRenderTarget);
    m_pDevice->SetDepthStencilSurface(pD3DZStencilSurface);

    D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = uiSizeX;
    viewport.Height = uiSizeY;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    m_pDevice->SetViewport(&viewport);
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CanCreateRenderItem
//
//
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::CanCreateRenderItem(ClassId classId)
{
    // none:        Don't create font or rendertarget if no memory
    // no_mem:      Don't create font or rendertarget
    // low_mem:     Don't create font or rendertarget randomly
    // no_shader:   Don't validate any shaders

    if (classId == CRenderTargetItem::GetClassId() || classId == CGuiFontItem::GetClassId() || classId == CDxFontItem::GetClassId())
    {
        if (m_iMemoryKBFreeForMTA <= 0)
            return false;

        if (m_TestMode == DX_TEST_MODE_NO_MEM)
            return false;

        if (m_TestMode == DX_TEST_MODE_LOW_MEM)
        {
            if ((rand() % 1000) > 750)
                return false;
        }
    }
    else if (classId == CShaderItem::GetClassId())
    {
        if (m_TestMode == DX_TEST_MODE_NO_SHADER)
            return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetTestMode
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::SetTestMode(eDxTestMode testMode)
{
    m_TestMode = testMode;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateMemoryUsage
//
// Should be called when a render item is created/destroyed or changes
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateMemoryUsage()
{
    m_iTextureMemoryKBUsed = 0;
    m_iRenderTargetMemoryKBUsed = 0;
    m_iFontMemoryKBUsed = 0;
    for (std::set<CRenderItem*>::iterator iter = m_CreatedItemList.begin(); iter != m_CreatedItemList.end(); iter++)
    {
        CRenderItem* pRenderItem = *iter;
        if (!pRenderItem->GetIncludeInMemoryStats())
            continue;
        int iMemoryKBUsed = pRenderItem->GetVideoMemoryKBUsed();

        if (pRenderItem->IsA(CFileTextureItem::GetClassId()))
            m_iTextureMemoryKBUsed += iMemoryKBUsed;
        else if (pRenderItem->IsA(CRenderTargetItem::GetClassId()) || pRenderItem->IsA(CScreenSourceItem::GetClassId()))
            m_iRenderTargetMemoryKBUsed += iMemoryKBUsed;
        else if (pRenderItem->IsA(CGuiFontItem::GetClassId()) || pRenderItem->IsA(CDxFontItem::GetClassId()))
            m_iFontMemoryKBUsed += iMemoryKBUsed;
    }

    m_iMemoryKBFreeForMTA = m_iVideoCardMemoryKBForMTATotal;
    m_iMemoryKBFreeForMTA -= m_iFontMemoryKBUsed / 2;
    m_iMemoryKBFreeForMTA -= m_iTextureMemoryKBUsed / 4;
    m_iMemoryKBFreeForMTA -= m_iRenderTargetMemoryKBUsed;
    m_iMemoryKBFreeForMTA = std::max(0, m_iMemoryKBFreeForMTA);
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::GetDxStatus
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::GetDxStatus(SDxStatus& outStatus)
{
    outStatus.testMode = m_TestMode;

    // Copy hardware settings
    outStatus.videoCard.strName = m_strVideoCardName;
    outStatus.videoCard.iInstalledMemoryKB = m_iVideoCardMemoryKBTotal;
    outStatus.videoCard.strPSVersion = m_strVideoCardPSVersion;
    outStatus.videoCard.depthBufferFormat = m_depthBufferFormat;
    outStatus.videoCard.iMaxAnisotropy = g_pDeviceState->AdapterState.MaxAnisotropicSetting;
    outStatus.videoCard.iNumSimultaneousRTs = g_pDeviceState->DeviceCaps.NumSimultaneousRTs;

    // State
    outStatus.state.iNumShadersUsingReadableDepthBuffer = m_ShadersUsingDepthBuffer.size();

    // Memory usage
    outStatus.videoMemoryKB.iFreeForMTA = m_iMemoryKBFreeForMTA;
    outStatus.videoMemoryKB.iUsedByFonts = m_iFontMemoryKBUsed;
    outStatus.videoMemoryKB.iUsedByTextures = m_iTextureMemoryKBUsed;
    outStatus.videoMemoryKB.iUsedByRenderTargets = m_iRenderTargetMemoryKBUsed;

    // Option settings
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    outStatus.settings.bWindowed = GetVideoModeManager()->IsDisplayModeWindowed();
    outStatus.settings.iFullScreenStyle = GetVideoModeManager()->GetFullScreenStyle();
    outStatus.settings.iFXQuality = gameSettings->GetFXQuality();
    ;
    outStatus.settings.iDrawDistance = (gameSettings->GetDrawDistance() - 0.925f) / 0.8749f * 100;
    outStatus.settings.iAntiAliasing = gameSettings->GetAntiAliasing() - 1;
    outStatus.settings.bVolumetricShadows = false;
    outStatus.settings.bAllowScreenUpload = true;
    outStatus.settings.iStreamingMemory = 0;
    outStatus.settings.bGrassEffect = false;
    outStatus.settings.bHeatHaze = false;
    outStatus.settings.iAnisotropicFiltering = 0;
    outStatus.settings.aspectRatio = gameSettings->GetAspectRatio();
    outStatus.settings.bHUDMatchAspectRatio = true;
    outStatus.settings.fFieldOfView = 70;
    outStatus.settings.bHighDetailVehicles = false;
    outStatus.settings.bHighDetailPeds = false;
    outStatus.settings.bBlur = true;
    outStatus.settings.bCoronaReflections = false;
    outStatus.settings.bDynamicPedShadows = false;

    CVARS_GET("streaming_memory", outStatus.settings.iStreamingMemory);
    CVARS_GET("volumetric_shadows", outStatus.settings.bVolumetricShadows);
    CVARS_GET("allow_screen_upload", outStatus.settings.bAllowScreenUpload);
    CVARS_GET("grass", outStatus.settings.bGrassEffect);
    CVARS_GET("heat_haze", outStatus.settings.bHeatHaze);
    CVARS_GET("anisotropic", outStatus.settings.iAnisotropicFiltering);
    CVARS_GET("hud_match_aspect_ratio", outStatus.settings.bHUDMatchAspectRatio);
    CVARS_GET("fov", outStatus.settings.fFieldOfView);
    CVARS_GET("high_detail_vehicles", outStatus.settings.bHighDetailVehicles);
    CVARS_GET("high_detail_peds", outStatus.settings.bHighDetailPeds);
    CVARS_GET("blur", outStatus.settings.bBlur);
    CVARS_GET("corona_reflections", outStatus.settings.bCoronaReflections);
    CVARS_GET("dynamic_ped_shadows", outStatus.settings.bDynamicPedShadows);

    if (outStatus.settings.iFXQuality == 0)
    {
        // These are always off with low fx quality
        outStatus.settings.bVolumetricShadows = false;
        outStatus.settings.bGrassEffect = false;
    }

    if (outStatus.settings.iFXQuality < 2)
    {
        outStatus.settings.bDynamicPedShadows = false;
    }

    // Display color depth
    D3DFORMAT BackBufferFormat = g_pDeviceState->CreationState.PresentationParameters.BackBufferFormat;
    if (BackBufferFormat >= D3DFMT_R5G6B5 && BackBufferFormat < D3DFMT_A8R3G3B2)
        outStatus.settings.b32BitColor = 0;
    else
        outStatus.settings.b32BitColor = 1;

    // Modify if using test mode
    if (m_TestMode == DX_TEST_MODE_NO_MEM)
        outStatus.videoMemoryKB.iFreeForMTA = 0;

    if (m_TestMode == DX_TEST_MODE_LOW_MEM)
        outStatus.videoMemoryKB.iFreeForMTA = 1;

    if (m_TestMode == DX_TEST_MODE_NO_SHADER)
        outStatus.videoCard.strPSVersion = "0";
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::GetBitsPerPixel
//
// Returns bits per pixel for a given D3D format
//
////////////////////////////////////////////////////////////////
int CRenderItemManager::GetBitsPerPixel(D3DFORMAT Format)
{
    switch (Format)
    {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_Q8W8V8U8:
        case D3DFMT_X8L8V8U8:
        case D3DFMT_A2B10G10R10:
        case D3DFMT_V16U16:
        case D3DFMT_G16R16:
        case D3DFMT_D24X4S4:
        case D3DFMT_D32:
        case D3DFMT_D24X8:
        case D3DFMT_D24S8:
            return 32;

        case D3DFMT_R8G8B8:
            return 24;

        case D3DFMT_X1R5G5B5:
        case D3DFMT_R5G6B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_D16:
        case D3DFMT_A8L8:
        case D3DFMT_V8U8:
        case D3DFMT_L6V5U5:
        case D3DFMT_D16_LOCKABLE:
        case D3DFMT_D15S1:
        case D3DFMT_A8P8:
        case D3DFMT_A8R3G3B2:
        case D3DFMT_X4R4G4B4:
        case D3DFMT_A4R4G4B4:
            return 16;

        case D3DFMT_R3G3B2:
        case D3DFMT_A4L4:
        case D3DFMT_P8:
        case D3DFMT_A8:
        case D3DFMT_L8:
        case D3DFMT_DXT2:
        case D3DFMT_DXT3:
        case D3DFMT_DXT4:
        case D3DFMT_DXT5:
            return 8;

        case D3DFMT_DXT1:
            return 4;

        default:
            return 32;            // unknown - guess at 32
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::GetPitchDivisor
//
// Returns block width for a given D3D format
//
////////////////////////////////////////////////////////////////
int CRenderItemManager::GetPitchDivisor(D3DFORMAT Format)
{
    switch (Format)
    {
        case D3DFMT_DXT1:
        case D3DFMT_DXT2:
        case D3DFMT_DXT3:
        case D3DFMT_DXT4:
        case D3DFMT_DXT5:
            return 4;

        default:
            return 1;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CalcD3DResourceMemoryKBUsage
//
// Calculate how much video memory a D3D resource will take
//
////////////////////////////////////////////////////////////////
int CRenderItemManager::CalcD3DResourceMemoryKBUsage(IDirect3DResource9* pD3DResource)
{
    D3DRESOURCETYPE type = pD3DResource->GetType();

    if (type == D3DRTYPE_SURFACE)
        return CalcD3DSurfaceMemoryKBUsage((IDirect3DSurface9*)pD3DResource);

    if (type == D3DRTYPE_TEXTURE)
        return CalcD3DTextureMemoryKBUsage((IDirect3DTexture9*)pD3DResource);

    if (type == D3DRTYPE_VOLUMETEXTURE)
        return CalcD3DVolumeTextureMemoryKBUsage((IDirect3DVolumeTexture9*)pD3DResource);

    if (type == D3DRTYPE_CUBETEXTURE)
        return CalcD3DCubeTextureMemoryKBUsage((IDirect3DCubeTexture9*)pD3DResource);

    return 0;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CalcD3DSurfaceMemoryKBUsage
//
//
//
////////////////////////////////////////////////////////////////
int CRenderItemManager::CalcD3DSurfaceMemoryKBUsage(IDirect3DSurface9* pD3DSurface)
{
    D3DSURFACE_DESC surfaceDesc;
    pD3DSurface->GetDesc(&surfaceDesc);

    int iBitsPerPixel = GetBitsPerPixel(surfaceDesc.Format);
    int iMemoryUsed = surfaceDesc.Width * surfaceDesc.Height / 8 * iBitsPerPixel;

    return iMemoryUsed / 1024;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CalcD3DTextureMemoryKBUsage
//
//
//
////////////////////////////////////////////////////////////////
int CRenderItemManager::CalcD3DTextureMemoryKBUsage(IDirect3DTexture9* pD3DTexture)
{
    int iMemoryUsed = 0;

    // Calc memory usage
    int iLevelCount = pD3DTexture->GetLevelCount();
    for (int i = 0; i < iLevelCount; i++)
    {
        D3DSURFACE_DESC surfaceDesc;
        pD3DTexture->GetLevelDesc(i, &surfaceDesc);

        int iBitsPerPixel = GetBitsPerPixel(surfaceDesc.Format);
        iMemoryUsed += surfaceDesc.Width * surfaceDesc.Height / 8 * iBitsPerPixel;
    }

    return iMemoryUsed / 1024;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CalcD3DVolumeTextureMemoryKBUsage
//
//
//
////////////////////////////////////////////////////////////////
int CRenderItemManager::CalcD3DVolumeTextureMemoryKBUsage(IDirect3DVolumeTexture9* pD3DVolumeTexture)
{
    int iMemoryUsed = 0;

    // Calc memory usage
    int iLevelCount = pD3DVolumeTexture->GetLevelCount();
    for (int i = 0; i < iLevelCount; i++)
    {
        D3DVOLUME_DESC volumeDesc;
        pD3DVolumeTexture->GetLevelDesc(i, &volumeDesc);

        int iBitsPerPixel = GetBitsPerPixel(volumeDesc.Format);
        iMemoryUsed += volumeDesc.Width * volumeDesc.Height * volumeDesc.Depth / 8 * iBitsPerPixel;
    }

    return iMemoryUsed / 1024;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CalcD3DCubeTextureMemoryKBUsage
//
//
//
////////////////////////////////////////////////////////////////
int CRenderItemManager::CalcD3DCubeTextureMemoryKBUsage(IDirect3DCubeTexture9* pD3DCubeTexture)
{
    int iMemoryUsed = 0;

    // Calc memory usage
    int iLevelCount = pD3DCubeTexture->GetLevelCount();
    for (int i = 0; i < iLevelCount; i++)
    {
        D3DSURFACE_DESC surfaceDesc;
        pD3DCubeTexture->GetLevelDesc(i, &surfaceDesc);

        int iBitsPerPixel = GetBitsPerPixel(surfaceDesc.Format);
        iMemoryUsed += surfaceDesc.Width * surfaceDesc.Height / 8 * iBitsPerPixel;
    }

    return iMemoryUsed * 6 / 1024;
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::NotifyShaderItemUsesDepthBuffer
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::NotifyShaderItemUsesDepthBuffer(CShaderItem* pShaderItem, bool bUsesDepthBuffer)
{
    if (bUsesDepthBuffer)
        MapInsert(m_ShadersUsingDepthBuffer, pShaderItem);
    else
        MapRemove(m_ShadersUsingDepthBuffer, pShaderItem);
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::NotifyShaderItemUsesMultipleRenderTargets
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::NotifyShaderItemUsesMultipleRenderTargets(CShaderItem* pShaderItem, bool bUsesMultipleRenderTargets)
{
    if (bUsesMultipleRenderTargets)
        MapInsert(m_ShadersUsingMultipleRenderTargets, pShaderItem);
    else
        MapRemove(m_ShadersUsingMultipleRenderTargets, pShaderItem);
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::PreDrawWorld
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::PreDrawWorld()
{
    // Save scene matrices
    g_pDeviceState->MainSceneState.TransformState = g_pDeviceState->TransformState;
    IDirect3DTexture9*& pReadableDepthBuffer = g_pDeviceState->MainSceneState.DepthBuffer;

    // Determine what is needed
    bool bRequireDepthBuffer = false;
    if (!m_ShadersUsingDepthBuffer.empty() && m_depthBufferFormat != RFORMAT_UNKNOWN)
        bRequireDepthBuffer = true;

    bool bRequireNonAADisplay = false;
    if (g_pDeviceState->CreationState.PresentationParameters.MultiSampleType != D3DMULTISAMPLE_NONE)
        bRequireNonAADisplay = bRequireDepthBuffer || !m_ShadersUsingMultipleRenderTargets.empty();

    // Readable depth buffer is not compatible with volumetric shadows
    CCore::GetSingleton().GetGame()->GetSettings()->SetVolumetricShadowsSuspended(bRequireDepthBuffer);

    // Destroy old stuff that we don't need anymore
    if (!bRequireDepthBuffer)
    {
        SAFE_RELEASE(pReadableDepthBuffer);
    }
    if (!bRequireNonAADisplay)
    {
        SAFE_RELEASE(m_pNonAARenderTargetTexture);
        SAFE_RELEASE(m_pNonAARenderTarget);
        SAFE_RELEASE(m_pNonAADepthSurface2);
    }

    // Create new stuff that we need now
    if (bRequireDepthBuffer && !pReadableDepthBuffer)
    {
        // Create readable depth buffer
        m_pDevice->CreateTexture(m_uiDefaultViewportSizeX, m_uiDefaultViewportSizeY, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)m_depthBufferFormat, D3DPOOL_DEFAULT,
                                 &pReadableDepthBuffer, NULL);
    }
    if (bRequireNonAADisplay && !m_pNonAARenderTarget)
    {
        // Create a non-AA render target and depth buffer
        assert(!m_pNonAARenderTargetTexture);
        assert(!m_pNonAADepthSurface2);

        const D3DPRESENT_PARAMETERS& pp = g_pDeviceState->CreationState.PresentationParameters;
        if (!m_bIsSwiftShader)
            m_pDevice->CreateRenderTarget(m_uiDefaultViewportSizeX, m_uiDefaultViewportSizeY, pp.BackBufferFormat, D3DMULTISAMPLE_NONE, 0, false,
                                          &m_pNonAARenderTarget, NULL);
        else
        {
            // Render target texture is needed when emulating StretchRect
            m_pDevice->CreateTexture(m_uiDefaultViewportSizeX, m_uiDefaultViewportSizeY, 1, D3DUSAGE_RENDERTARGET, pp.BackBufferFormat, D3DPOOL_DEFAULT,
                                     &m_pNonAARenderTargetTexture, NULL);
            if (m_pNonAARenderTargetTexture)
                m_pNonAARenderTargetTexture->GetSurfaceLevel(0, &m_pNonAARenderTarget);
        }

        m_pDevice->CreateDepthStencilSurface(m_uiDefaultViewportSizeX, m_uiDefaultViewportSizeY, pp.AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, true,
                                             &m_pNonAADepthSurface2, NULL);
    }

    // Set depth buffer and maybe render target
    if ((pReadableDepthBuffer || m_pNonAADepthSurface2) && m_pSavedSceneDepthSurface == NULL)
    {
        if (m_pDevice->GetDepthStencilSurface(&m_pSavedSceneDepthSurface) == D3D_OK)
        {
            if (pReadableDepthBuffer)
            {
                // Set readable depth buffer
                IDirect3DSurface9* pSurf = NULL;
                if (pReadableDepthBuffer->GetSurfaceLevel(0, &pSurf) == D3D_OK)
                {
                    m_pDevice->SetDepthStencilSurface(pSurf);
                    m_bUsingReadableDepthBuffer = true;
                    pSurf->Release();
                }
            }
            else
            {
                // Set non-AA depth buffer
                m_pDevice->SetDepthStencilSurface(m_pNonAADepthSurface2);
            }

            // Also switch to non-AA render target if created
            if (m_pNonAARenderTarget)
            {
                if (m_pDevice->GetRenderTarget(0, &m_pSavedSceneRenderTargetAA) == D3D_OK)
                {
                    m_pDevice->SetRenderTarget(0, m_pNonAARenderTarget);
                }
            }
            m_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);
        }
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SaveReadableDepthBuffer
//
// Ensure our readable depth buffer is no longer being used
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::SaveReadableDepthBuffer()
{
    if (m_bUsingReadableDepthBuffer)
    {
        m_bUsingReadableDepthBuffer = false;

        if (m_pNonAADepthSurface2)
        {
            // If using AA hacks, change to the other depth buffer we created
            m_pDevice->SetDepthStencilSurface(m_pNonAADepthSurface2);
            m_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);
        }
        else
        {
            // If not using AA hacks, just change back to the GTA depth buffer
            if (m_pSavedSceneDepthSurface)
            {
                m_pDevice->SetDepthStencilSurface(m_pSavedSceneDepthSurface);
                SAFE_RELEASE(m_pSavedSceneDepthSurface);
            }
        }
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::FlushNonAARenderTarget
//
// If using AA hacks, change everything back
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::FlushNonAARenderTarget()
{
    if (m_pSavedSceneDepthSurface)
    {
        m_pDevice->SetDepthStencilSurface(m_pSavedSceneDepthSurface);
        SAFE_RELEASE(m_pSavedSceneDepthSurface);
    }

    if (m_pSavedSceneRenderTargetAA)
    {
        // Restore GTA AA render target, and copy our non-AA data to it
        m_pDevice->SetRenderTarget(0, m_pSavedSceneRenderTargetAA);

        if (m_pNonAARenderTarget)
        {
            if (!m_bIsSwiftShader)
                m_pDevice->StretchRect(m_pNonAARenderTarget, NULL, m_pSavedSceneRenderTargetAA, NULL, D3DTEXF_POINT);
            else
            {
                // Emulate StretchRect using DrawPrimitive

                // Save render states
                IDirect3DStateBlock9* pSavedStateBlock = NULL;
                m_pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);

                // Prepare vertex buffer
                float fX1 = -0.5f;
                float fY1 = -0.5f;
                float fX2 = m_uiDefaultViewportSizeX + fX1;
                float fY2 = m_uiDefaultViewportSizeY + fY1;
                float fU1 = 0;
                float fV1 = 0;
                float fU2 = 1;
                float fV2 = 1;

                const SRTVertex vertices[] = {{fX1, fY1, 0, 1, fU1, fV1}, {fX2, fY1, 0, 1, fU2, fV1}, {fX1, fY2, 0, 1, fU1, fV2},
                                              {fX2, fY1, 0, 1, fU2, fV1}, {fX2, fY2, 0, 1, fU2, fV2}, {fX1, fY2, 0, 1, fU1, fV2}};

                // Set vertex stream
                uint        PrimitiveCount = NUMELMS(vertices) / 3;
                const void* pVertexStreamZeroData = &vertices[0];
                uint        VertexStreamZeroStride = sizeof(SRTVertex);
                m_pDevice->SetFVF(SRTVertex::FVF);

                // Set render states
                m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
                m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
                m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
                m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
                m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
                m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

                // Draw using texture
                m_pDevice->SetTexture(0, m_pNonAARenderTargetTexture);
                m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);

                // Restore render states
                if (pSavedStateBlock)
                {
                    pSavedStateBlock->Apply();
                    SAFE_RELEASE(pSavedStateBlock);
                }
            }
        }
        SAFE_RELEASE(m_pSavedSceneRenderTargetAA);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::HandleStretchRect
//
// Maybe replace source surface with our non-AA rt, depending on things
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::HandleStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect,
                                           int Filter)
{
    if (pSourceSurface == m_pSavedSceneRenderTargetAA)
    {
        // If trying to copy from the saved render target, use the active render target instead
        if (m_pDevice->GetRenderTarget(0, &pSourceSurface) == D3D_OK)
        {
            m_pDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, (D3DTEXTUREFILTERTYPE)Filter);
            SAFE_RELEASE(pSourceSurface);
        }
    }
    else
    {
        m_pDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, (D3DTEXTUREFILTERTYPE)Filter);
    }
}
