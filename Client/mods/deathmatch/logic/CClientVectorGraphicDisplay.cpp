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

void CClientVectorGraphicDisplay::UnpremultiplyBitmap(Bitmap& bitmap)
{
    auto width = bitmap.width();
    auto height = bitmap.height();
    auto stride = bitmap.stride();
    auto rowData = bitmap.data();

    for (std::uint32_t y = 0; y < height; y++)
    {
        auto data = rowData;
        for (std::uint32_t x = 0; x < width; x++)
        {
            auto& b = data[0];
            auto& g = data[1];
            auto& r = data[2];
            auto& a = data[3];

            if (a != 0)
            {
                r = (r * 255) / a;
                g = (g * 255) / a;
                b = (b * 255) / a;
            }

            data += 4;
        }

        rowData += stride;
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

    // SVG has a predefined width and height. We need transform it to the requested size
    const Matrix transformationMatrix(pVectorGraphicItem->m_uiSizeX / svgDocument->width(), 0, 0, pVectorGraphicItem->m_uiSizeY / svgDocument->height(), 0, 0);

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    if (SUCCEEDED(surface->LockRect(&LockedRect, nullptr, D3DLOCK_DISCARD)))
    {
        auto surfaceData = static_cast<std::uint8_t*>(LockedRect.pBits);
        auto stride = static_cast<std::uint32_t>(LockedRect.Pitch);

        Bitmap bitmap{surfaceData, pVectorGraphicItem->m_uiSizeX, pVectorGraphicItem->m_uiSizeY, stride};
        bitmap.clear(0);
        svgDocument->render(bitmap, transformationMatrix);
        UnpremultiplyBitmap(bitmap);

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
