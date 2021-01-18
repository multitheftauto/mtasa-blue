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

CClientVectorGraphic::CClientVectorGraphic(CClientManager* pManager, ElementID ID, CVectorGraphicItem* pVectorGraphicItem)
    : ClassInit(this), CClientTexture(pManager, ID, pVectorGraphicItem)
{
    m_pDocument = nullptr;
    m_pResource = nullptr;
    m_pManager = pManager;

    m_pRenderItem = pVectorGraphicItem; // This is cast to CClientRenderItem
    m_pVectorGraphicItem = pVectorGraphicItem; // Keep this as CVectorGraphicItem so we don't have to cast everywhere later on...

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
    rootElement->setAttribute("x", "50%");
    rootElement->setAttribute("y", "50%");
    rootElement->setAttribute("width", strWidth);
    rootElement->setAttribute("height", strHeight);
}

void CClientVectorGraphic::UpdateTexture()
{
    if (m_bHasUpdated)
        return;

    IDirect3DSurface9* surface = m_pVectorGraphicItem->m_pD3DRenderTargetSurface;

    if (!surface)
        return;

    IDirect3DDevice9* device = m_pVectorGraphicItem->m_pDevice;

    uint width = m_pVectorGraphicItem->m_uiSizeX;
    uint height = m_pVectorGraphicItem->m_uiSizeY;

    Bitmap bitmap = m_pDocument->renderToBitmap(width, height, 96.0);

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

    m_bHasUpdated = true;
}

void CClientVectorGraphic::ClearTexture()
{
    // Does nothing right now
}

void CClientVectorGraphic::LoadFromFile(std::string strFilePath)
{
    m_pDocument->loadFromFile(strFilePath);
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
