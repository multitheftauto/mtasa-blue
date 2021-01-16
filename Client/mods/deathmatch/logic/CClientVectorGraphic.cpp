/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientVectorGraphic.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <svgdocument.h>

using namespace lunasvg;

CClientVectorGraphic::CClientVectorGraphic(CClientManager* pManager, ElementID ID, CVectorGraphicItem* pVectorGraphicItem)
    : ClassInit(this), CClientTexture(pManager, ID, pVectorGraphicItem)
{
    m_pDocument = nullptr;
    m_pResource = nullptr;
    m_pManager = pManager;

    m_pVectorGraphicItem = pVectorGraphicItem;

    CreateDocument();

    m_pVectorGraphicDisplay = new CClientVectorGraphicDisplay(m_pManager->GetDisplayManager(), this);
}

void CClientVectorGraphic::CreateDocument()
{
    if (m_pDocument)
        return;

    m_pDocument = new SVGDocument();

    SVGElement* rootElement = m_pDocument->rootElement();

    uint uiWidth = m_pVectorGraphicItem->m_uiSurfaceSizeX;
    uint uiHeight = m_pVectorGraphicItem->m_uiSurfaceSizeY;

    std::string strWidth = std::to_string(uiWidth);
    std::string strHeight = std::to_string(uiHeight);

    rootElement->setAttribute("viewBox", "0, 0, " + strWidth + ", " + strHeight);
    rootElement->setAttribute("x", "0");
    rootElement->setAttribute("y", "0");
    rootElement->setAttribute("width", strWidth);
    rootElement->setAttribute("height", strHeight);
    rootElement->setAttribute("style", "fill:#FFF;stroke:#000;stroke-width:4");
}

void CClientVectorGraphic::UpdateTexture()
{
    IDirect3DSurface9* surface = m_pVectorGraphicItem->m_pD3DRenderTargetSurface;

    if (!surface)
        return;

    IDirect3DDevice9* device = m_pVectorGraphicItem->m_pDevice;

    uint width = m_pVectorGraphicItem->m_uiSizeX;
    uint height = m_pVectorGraphicItem->m_uiSizeY;

    Bitmap bitmap = m_pDocument->renderToBitmap(width, height);

    // Lock surface
    D3DLOCKED_RECT LockedRect;
    surface->LockRect(&LockedRect, nullptr, 0);

    auto surfaceData = static_cast<byte*>(LockedRect.pBits);
    auto sourceData = static_cast<const byte*>(bitmap.data());
    auto pitch = LockedRect.Pitch;

    memcpy(surfaceData, sourceData, bitmap.width() * bitmap.height() * 4);

    // Unlock surface
    surface->UnlockRect();
}

void CClientVectorGraphic::ClearTexture()
{
    // Does nothing right now
}

CClientVectorGraphic::~CClientVectorGraphic()
{
    delete m_pDocument;
    m_pDocument = nullptr;

    delete m_pVectorGraphicItem;
    m_pVectorGraphicItem = nullptr;

    delete m_pVectorGraphicDisplay;
    m_pVectorGraphicDisplay = nullptr;
}
