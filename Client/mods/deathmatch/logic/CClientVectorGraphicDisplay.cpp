/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientVectorGraphicDisplay.h"
#include "CClientVectorGraphic.h"
#include <lunasvg.h>

using namespace lunasvg;

CClientVectorGraphicDisplay::CClientVectorGraphicDisplay(CClientDisplayManager* pDisplayManager, CClientVectorGraphic* pVectorGraphic, int ID)
    : CClientDisplay(pDisplayManager, ID)
{
    m_pVectorGraphic = pVectorGraphic;
    m_bVisible = true;
    m_bIsCleared = false;

    UpdateTexture();
}

void CClientVectorGraphicDisplay::Render()
{
    if (!m_pVectorGraphic || m_pVectorGraphic->IsDestroyed())
        return;

    if (!m_bVisible)
    {
        if (!IsCleared())
            ClearTexture();

        return;
    }
    else if (IsCleared())
        m_bIsCleared = false;

    if (HasUpdated())
        UpdateTexture();
}

void CClientVectorGraphicDisplay::UpdateTexture()
{
    if (!m_pVectorGraphic || m_pVectorGraphic->IsDestroyed())
        return;

    Document* svgDocument = m_pVectorGraphic->GetSVGDocument();

    if (svgDocument == nullptr)
        return;

    CVectorGraphicItem* pVectorGraphicItem = m_pVectorGraphic->GetRenderItem();

    if (!pVectorGraphicItem)
        return;

    IDirect3DSurface9* surface = m_pVectorGraphic->GetRenderItem()->m_pD3DRenderTargetSurface;

    if (!surface)
        return;

    IDirect3DDevice9* device = pVectorGraphicItem->m_pDevice;

    uint width = pVectorGraphicItem->m_uiSizeX;
    uint height = pVectorGraphicItem->m_uiSizeY;

    Bitmap bitmap = svgDocument->renderToBitmap(width, height);

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    surface->LockRect(&LockedRect, nullptr, 0);

    auto surfaceData = static_cast<byte*>(LockedRect.pBits);
    auto sourceData = static_cast<const byte*>(bitmap.data());

    for (int y = 0; y < bitmap.height(); ++y)
    {
        memcpy(surfaceData, sourceData, bitmap.width() * 4);            // 4 bytes per pixel

        // advance row pointers
        sourceData += bitmap.stride();
        surfaceData += LockedRect.Pitch;
    }

    // Unlock surface
    surface->UnlockRect();

    m_bHasUpdated = false;
}

void CClientVectorGraphicDisplay::ClearTexture()
{
    if (!m_pVectorGraphic || m_pVectorGraphic->IsDestroyed())
        return;

    CVectorGraphicItem* pVectorGraphicItem = m_pVectorGraphic->GetRenderItem();

    if (!pVectorGraphicItem)
        return;

    IDirect3DSurface9* surface = pVectorGraphicItem->m_pD3DRenderTargetSurface;

    if (!surface)
        return;

    IDirect3DDevice9* device = pVectorGraphicItem->m_pDevice;

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    surface->LockRect(&LockedRect, nullptr, 0);

    device->ColorFill(surface, nullptr, D3DCOLOR_ARGB(0, 0, 0, 0));

    // Unlock surface
    surface->UnlockRect();

    m_bIsCleared = true;
}

void CClientVectorGraphicDisplay::Update()
{
    m_bHasUpdated = true;
}
