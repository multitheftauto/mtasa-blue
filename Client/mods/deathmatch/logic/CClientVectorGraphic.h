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
    ~CClientVectorGraphic() = default;

    CResource* GetResource() const { return m_pResource; }
    void       SetResource(CResource* pResource) { m_pResource = pResource; }

    eClientEntityType GetType() const { return CCLIENTVECTORGRAPHIC; }

    void CreateDocument();
    bool LoadFromData(std::string strData);

    void Destroy();
    bool IsDestroyed() const { return m_bIsDestroyed; }

    lunasvg::Document*   GetSVGDocument() const { return m_pDocument.get(); }
    CVectorGraphicItem*  GetRenderItem() const { return static_cast<CVectorGraphicItem*>(m_pRenderItem); }

    CXMLNode* GetSVGDocumentXML() const;

    bool IsDisplayCleared() const { return m_pVectorGraphicDisplay->IsCleared(); }

    float GetDPI() const { return m_fDPI; }
    void  SetDPI(const float dpi) { m_fDPI = dpi; }
private:
    CResource*                                     m_pResource;
    CClientManager*                                m_pManager;

    std::unique_ptr<lunasvg::Document>             m_pDocument;
    CXMLNode*                                      m_pXMLDocument;

    std::unique_ptr<CClientVectorGraphicDisplay>   m_pVectorGraphicDisplay;

    float m_fDPI = 96.0;
    bool  m_bIsDestroyed = false;
};
