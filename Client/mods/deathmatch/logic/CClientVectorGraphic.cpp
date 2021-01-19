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

std::variant<bool, int, std::string> CClientVectorGraphic::AddRect(std::variant<float, std::string> x, std::variant<float, std::string> y, std::variant<float, std::string> width,                         std::variant<float, std::string> height, std::variant<float, std::string> rx, std::variant<float, std::string> ry, float pathLength, std::string fill)
{
    SVGElement* rootElement = m_pDocument->rootElement();

    SString rectSVGNode = SString("<rect x='%s' y='%s' width='%s' height='%s' rx='%s' ry='%s' pathLength='%s' fill='%s' color='%s' data-id='%s' />",
        (x.index() == 0) ? std::to_string(std::get<float>(x)).c_str() : std::get<string>(x).c_str(),
        (y.index() == 0) ? std::to_string(std::get<float>(y)).c_str() : std::get<string>(y).c_str(),
        (width.index() == 0) ? std::to_string(std::get<float>(width)).c_str() : std::get<string>(width).c_str(),
        (height.index() == 0) ? std::to_string(std::get<float>(height)).c_str() : std::get<string>(height).c_str(),
        (rx.index() == 0) ? std::to_string(std::get<float>(rx)).c_str() : std::get<string>(rx).c_str(),
        (ry.index() == 0) ? std::to_string(std::get<float>(ry)).c_str() : std::get<string>(ry).c_str(),
        std::to_string(pathLength).c_str(),
        fill.c_str(),
        color.c_str(),
        std::to_string(++m_iShapeCount).c_str());

    m_pDocument->appendContent(rectSVGNode.c_str());

    m_bHasUpdated = false;
                            
    return rectSVGNode;
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
