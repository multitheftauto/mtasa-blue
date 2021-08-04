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
#include <document.h>

CClientVectorGraphic::CClientVectorGraphic(CClientManager* pManager, ElementID ID, CVectorGraphicItem* pVectorGraphicItem)
    : ClassInit(this), CClientTexture(pManager, ID, pVectorGraphicItem)
{
    SetTypeName("svg");

    m_pDocument = nullptr;
    m_pXMLDocument = nullptr;

    m_pResource = nullptr;
    m_pManager = pManager;
    
    m_pVectorGraphicDisplay = std::make_unique<CClientVectorGraphicDisplay>(m_pManager->GetDisplayManager(), this);
}

bool CClientVectorGraphic::LoadFromData(std::string strData)
{
    m_pDocument = lunasvg::Document::loadFromData(strData);
    auto xmlStringNode = g_pCore->GetXML()->ParseString(strData.c_str());

    if (!m_pDocument || !xmlStringNode)
        return false;

    CXMLNode* node = xmlStringNode->node;

    if (!node || !node->IsValid())
        return false;

    m_pXMLDocument = node;
    m_pVectorGraphicDisplay->Update();
    return true;
}

CXMLNode* CClientVectorGraphic::GetSVGDocumentXML() const
{
    return m_pXMLDocument;
}

void CClientVectorGraphic::Destroy()
{
    delete m_pXMLDocument;

    m_bIsDestroyed = true;
}
