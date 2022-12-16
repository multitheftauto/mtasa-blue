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
    {
        m_pVectorGraphic->OnUpdate();
    }        
}

void CClientVectorGraphicDisplay::UpdateTexture()
{
    if (!m_pVectorGraphic || m_pVectorGraphic->IsDestroyed())
        return;

    Document* svgDocument = m_pVectorGraphic->GetSVGDocument();
    if (!svgDocument)
        return;

    CVectorGraphicItem* pVectorGraphicItem = m_pVectorGraphic->GetRenderItem();
    if (!pVectorGraphicItem)
        return;

    IDirect3DSurface9* surface = m_pVectorGraphic->GetRenderItem()->m_pD3DRenderTargetSurface;
    if (!surface)
        return;

    Bitmap bitmap = svgDocument->renderToBitmap(pVectorGraphicItem->m_uiSizeX, pVectorGraphicItem->m_uiSizeY);
    if (!bitmap.valid())
        return;

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    if (SUCCEEDED(surface->LockRect(&LockedRect, nullptr, D3DLOCK_DISCARD)))
    {
        auto surfaceData = static_cast<byte*>(LockedRect.pBits);
        auto sourceData = static_cast<const byte*>(bitmap.data());

        for (uint32_t y = 0; y < bitmap.height(); ++y)
        {
            memcpy(surfaceData, sourceData, bitmap.width() * 4);            // 4 bytes per pixel

            // advance row pointers
            sourceData += bitmap.stride();
            surfaceData += LockedRect.Pitch;
        }

        // Unlock surface
        surface->UnlockRect();
    }

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

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    if (SUCCEEDED(surface->LockRect(&LockedRect, nullptr, D3DLOCK_DISCARD)))
    {
        std::memset(LockedRect.pBits, 0x0, LockedRect.Pitch * pVectorGraphicItem->m_uiSurfaceSizeY);

        // Unlock surface
        surface->UnlockRect();
    }

    m_bIsCleared = true;
}

void CClientVectorGraphicDisplay::Update()
{
    m_bHasUpdated = true;
}
