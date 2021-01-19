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

    m_pVectorGraphicDisplay = new CClientVectorGraphicDisplay(m_pManager->GetDisplayManager(), this);

    CreateDocument();
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

    m_pVectorGraphicDisplay->Update();
}

std::variant<bool, int> CClientVectorGraphic::AddRect(std::variant<float, std::string> x, std::variant<float, std::string> y, std::variant<float, std::string> width,                         std::variant<float, std::string> height, std::variant<float, std::string> rx, std::variant<float, std::string> ry, float pathLength, std::string fill)
{
    if(!m_pDocument)
        return false;

    SVGElement* rootElement = m_pDocument->rootElement();

    // There has to be a better way to do this (:
    SString rectSVGNode = SString("<rect x='%s' y='%s' width='%s' height='%s' rx='%s' ry='%s' pathLength='%s' fill='%s' id='rect%s' />",
        (x.index() == 0) ? std::to_string(std::get<float>(x)).c_str() : std::get<string>(x).c_str(),
        (y.index() == 0) ? std::to_string(std::get<float>(y)).c_str() : std::get<string>(y).c_str(),
        (width.index() == 0) ? std::to_string(std::get<float>(width)).c_str() : std::get<string>(width).c_str(),
        (height.index() == 0) ? std::to_string(std::get<float>(height)).c_str() : std::get<string>(height).c_str(),
        (rx.index() == 0) ? std::to_string(std::get<float>(rx)).c_str() : std::get<string>(rx).c_str(),
        (ry.index() == 0) ? std::to_string(std::get<float>(ry)).c_str() : std::get<string>(ry).c_str(),
        std::to_string(pathLength).c_str(),
        fill.c_str(),
        std::to_string(++m_iShapeCount).c_str());

    m_pDocument->appendContent(rectSVGNode.c_str());

    m_pVectorGraphicDisplay->Update();
                            
    return m_iShapeCount;
}

std::variant<bool, int> CClientVectorGraphic::AddCircle(std::variant<float, std::string> cx, std::variant<float, std::string> cy, float radius, float pathLength, std::string fill)
{
    if(!m_pDocument)
        return false;

    SVGElement* rootElement = m_pDocument->rootElement();

    // There has to be a better way to do this (:
    SString circleSVGNode = SString("<circle cx='%s' cy='%s' r='%s' pathLength='%s' fill='%s' id='circle%s' />",
        (cx.index() == 0) ? std::to_string(std::get<float>(cx)).c_str() : std::get<string>(cx).c_str(),
        (cy.index() == 0) ? std::to_string(std::get<float>(cy)).c_str() : std::get<string>(cy).c_str(),
        std::to_string(radius).c_str(),
        std::to_string(pathLength).c_str(),
        fill.c_str(),
        std::to_string(++m_iShapeCount).c_str());

    m_pDocument->appendContent(circleSVGNode.c_str());

    m_pVectorGraphicDisplay->Update();
                            
    return m_iShapeCount;
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

const bool CClientVectorGraphic::IsDestroyed()
{
    return m_bIsDestroyed;
}

void CClientVectorGraphic::Destroy()
{
    m_bIsDestroyed = true;
}

CClientVectorGraphic::~CClientVectorGraphic()
{
    delete m_pDocument;
    m_pDocument = nullptr;
}
