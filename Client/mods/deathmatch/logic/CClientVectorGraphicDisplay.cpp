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

static bool SafeSvgRender(Document* doc, Bitmap& bitmap, const Matrix& matrix)
{
    __try
    {
        doc->render(bitmap, matrix);
        return true;
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
        return false;
    }
}

CClientVectorGraphicDisplay::CClientVectorGraphicDisplay(CClientVectorGraphic* pVectorGraphic, int ID) : CClientDisplay(ID)
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

    for (decltype(height) y = 0; y < height; y++)
    {
        auto data = rowData;
        for (decltype(width) x = 0; x < width; x++)
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

    // Validate render item dimensions
    if (pVectorGraphicItem->m_uiSizeX == 0 || pVectorGraphicItem->m_uiSizeY == 0)
        return;

    // Validate SVG document dimensions (reject zero, negative, NaN, Inf)
    float svgWidth = svgDocument->width();
    float svgHeight = svgDocument->height();
    if (svgWidth <= 0 || svgHeight <= 0 || !std::isfinite(svgWidth) || !std::isfinite(svgHeight))
        return;

    // Reject documents whose content bounding box is degenerate.
    // Catches fully-empty or NaN-dimension documents before they reach the renderer.
    Box bbox = svgDocument->boundingBox();
    if (bbox.w <= 0 || bbox.h <= 0 || !std::isfinite(bbox.w) || !std::isfinite(bbox.h)
        || !std::isfinite(bbox.x) || !std::isfinite(bbox.y))
        return;

    // SVG has a predefined width and height. We need to transform it to the requested size
    float scaleX = pVectorGraphicItem->m_uiSizeX / svgWidth;
    float scaleY = pVectorGraphicItem->m_uiSizeY / svgHeight;

    const Matrix transformationMatrix(scaleX, 0, 0, scaleY, 0, 0);

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    if (SUCCEEDED(surface->LockRect(&LockedRect, nullptr, D3DLOCK_DISCARD)))
    {
        auto surfaceData = static_cast<std::uint8_t*>(LockedRect.pBits);
        auto stride = static_cast<std::int32_t>(LockedRect.Pitch);

        Bitmap bitmap{surfaceData, (int32_t)pVectorGraphicItem->m_uiSizeX, (int32_t)pVectorGraphicItem->m_uiSizeY, stride};
        bitmap.clear(0);

        if (!SafeSvgRender(svgDocument, bitmap, transformationMatrix))
        {
            // SVG triggered an access violation during rendering (malformed content)
            surface->UnlockRect();
            m_bHasUpdated = false;
            return;
        }

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
