/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientVectorGraphicDisplay.h"

namespace lunasvg {
    class Document;
}

class CClientVectorGraphic final : public CClientTexture
{
    DECLARE_CLASS(CClientVectorGraphic, CClientTexture)
public:
    CClientVectorGraphic(CClientManager* pManager, ElementID ID, CVectorGraphicItem* pVectorGraphicItem);
    ~CClientVectorGraphic();

    virtual void Unlink() override;

    CResource* GetResource() const { return m_pResource; }
    void       SetResource(CResource* pResource) { m_pResource = pResource; }

    eClientEntityType GetType() const { return CCLIENTVECTORGRAPHIC; }

    bool LoadFromData(std::string strData);
    bool IsDestroyed() const { return m_bIsDestroyed; }

    lunasvg::Document*   GetSVGDocument() const { return m_pDocument.get(); }
    CVectorGraphicItem*  GetRenderItem() const { return static_cast<CVectorGraphicItem*>(m_pRenderItem); }
    CXMLNode*            GetSVGDocumentXML() const { return m_pXMLDocument; }

    bool IsDisplayCleared() const { return m_pVectorGraphicDisplay->IsCleared(); }
private:
    CResource*                                     m_pResource = nullptr;
    CClientManager*                                m_pManager;

    std::unique_ptr<lunasvg::Document>             m_pDocument = nullptr;
    CXMLNode*                                      m_pXMLDocument = nullptr;

    std::unique_ptr<CClientVectorGraphicDisplay>   m_pVectorGraphicDisplay;

    bool  m_bIsDestroyed = false;
};
