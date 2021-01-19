/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientVectorGraphicDisplay.cpp
 *  PURPOSE:     Client vector graphic display base class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

float CClientVectorGraphicDisplay::m_fGlobalScale = 1.0f;

CClientVectorGraphicDisplay::CClientVectorGraphicDisplay(CClientDisplayManager* pDisplayManager, CClientVectorGraphic* pVectorGraphic, int ID) : CClientDisplay(pDisplayManager, ID)
{
    // Init
    m_pVectorGraphic = pVectorGraphic;
    m_fScale = 1;
    m_ulFormat = 0;
    m_bVisible = true;
    m_bIsCleared = false;

    UpdateTexture();
}

CClientVectorGraphicDisplay::~CClientVectorGraphicDisplay()
{
}

void CClientVectorGraphicDisplay::SetPosition(const CVector& vecPosition)
{
    m_vecPosition = vecPosition;
}

void CClientVectorGraphicDisplay::Render()
{
    if (!m_pVectorGraphic || m_pVectorGraphic->IsDestroyed())
        return;

    if (!m_bVisible)
    {
        if (!IsCleared())
        {
            ClearTexture();
        }
            
        return;
    }
    else
    {
        if (IsCleared())
        {
            m_bIsCleared = false;
        }
    }

    if (HasUpdated())
        UpdateTexture();
}



void CClientVectorGraphicDisplay::UpdateTexture()
{
    if (m_pVectorGraphic->IsDestroyed())
        return;

    SVGDocument* svgDocument = m_pVectorGraphic->GetSVGDocument();

    if(!m_pVectorGraphic->GetSVGDocument())
        return;

    CVectorGraphicItem* pVectorGraphicItem = m_pVectorGraphic->GetRenderItem();

    IDirect3DSurface9* surface = m_pVectorGraphic->GetRenderItem()->m_pD3DRenderTargetSurface;

    if (!surface)
        return;

    IDirect3DDevice9* device = pVectorGraphicItem->m_pDevice;

    uint width = pVectorGraphicItem->m_uiSizeX;
    uint height = pVectorGraphicItem->m_uiSizeY;

    Bitmap bitmap = svgDocument->renderToBitmap(width, height, 96.0);

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    surface->LockRect(&LockedRect, nullptr, 0);

    auto surfaceData = static_cast<byte*>(LockedRect.pBits);
    auto sourceData = static_cast<const byte*>(bitmap.data());

    for (int y = 0; y < bitmap.height(); ++y)
    {
        memcpy(surfaceData, sourceData, bitmap.width() * 4);  // 4 bytes per pixel

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
   CVectorGraphicItem* pVectorGraphicItem = m_pVectorGraphic->GetRenderItem();
   IDirect3DSurface9* surface =  pVectorGraphicItem->m_pD3DRenderTargetSurface;

    if (!surface)
        return;

    IDirect3DDevice9* device = pVectorGraphicItem->m_pDevice;

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    surface->LockRect(&LockedRect, nullptr, 0);

    device->ColorFill(surface, NULL, D3DCOLOR_ARGB(0, 0, 0, 0));

    // Unlock surface
    surface->UnlockRect();

    m_bIsCleared = true;
}

void CClientVectorGraphicDisplay::Update()
{
    m_bHasUpdated = true;
}

void CClientVectorGraphicDisplay::SetColor(const SColor color)
{
    m_Color = color;
}

void CClientVectorGraphicDisplay::SetColorAlpha(unsigned char ucAlpha)
{
    m_Color.A = ucAlpha;
}

void CClientVectorGraphicDisplay::SetShadowAlpha(unsigned char ucShadowAlpha)
{
    m_ucShadowAlpha = ucShadowAlpha;
}

void CClientVectorGraphicDisplay::SetScale(float fScale)
{
    m_fScale = fScale;
}

void CClientVectorGraphicDisplay::SetFormat(unsigned long ulFormat)
{
    m_ulFormat = ulFormat;
}

void CClientVectorGraphicDisplay::SetVisible(bool bVisible)
{
    CClientDisplay::SetVisible(bVisible);
}
