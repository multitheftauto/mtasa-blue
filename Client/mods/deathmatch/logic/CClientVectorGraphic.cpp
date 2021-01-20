/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientVectorGraphic.h"
#include <svgdocument.h>

CClientVectorGraphic::CClientVectorGraphic(CClientManager* pManager, ElementID ID, std::unique_ptr<CVectorGraphicItem> pVectorGraphicItem)
    : ClassInit(this), CClientTexture(pManager, ID, pVectorGraphicItem.get())
{
    SetTypeName("svg");

    m_pDocument = nullptr;
    m_pResource = nullptr;
    m_pManager = pManager;

    m_pVectorGraphicItem = std::unique_ptr<CVectorGraphicItem>(std::move(pVectorGraphicItem));
    m_pVectorGraphicDisplay = std::unique_ptr<CClientVectorGraphicDisplay>(new CClientVectorGraphicDisplay(m_pManager->GetDisplayManager(), this));

    CreateDocument();
}

void CClientVectorGraphic::CreateDocument()
{
    if (m_pDocument)
        return;

    m_pDocument = std::unique_ptr<SVGDocument>(new SVGDocument());

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

bool CClientVectorGraphic::LoadFromFile(std::string strFilePath)
{
    if(!m_pDocument)
        return false;

    if (m_pDocument->loadFromFile(strFilePath))
    {
        m_pVectorGraphicDisplay->Update();
        return true;
    }

    return false;
}

bool CClientVectorGraphic::LoadFromData(std::string strData)
{
    if(!m_pDocument)
        return false;

    if (m_pDocument->loadFromData(strData))
    {
        m_pVectorGraphicDisplay->Update();
        return true;
    }

    return false;
}

void CClientVectorGraphic::Destroy()
{
    m_bIsDestroyed = true;
}
