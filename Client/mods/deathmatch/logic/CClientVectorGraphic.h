/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <svgdocument.h>
#include "CClientVectorGraphicDisplay.h"

using namespace lunasvg;

constexpr float DPI = 96.0;

class CClientVectorGraphic : public CClientTexture
{
    DECLARE_CLASS(CClientVectorGraphic, CClientTexture)
public:
    CClientVectorGraphic(CClientManager* pManager, ElementID ID, std::unique_ptr<CVectorGraphicItem> pVectorGraphicItem);
    ~CClientVectorGraphic() = default;

    CResource* GetResource() const { return m_pResource; }
    void       SetResource(CResource* pResource) { m_pResource = pResource; }

    eClientEntityType GetType() const { return CCLIENTVECTORGRAPHIC; }

    void CreateDocument();

    bool LoadFromFile(std::string strFilePath);
    bool LoadFromData(std::string strData);

    void Destroy();
    bool IsDestroyed() const { return m_bIsDestroyed; }

    SVGDocument*        GetSVGDocument() const { return m_pDocument.get(); }
    CVectorGraphicItem* GetRenderItem() const { return m_pVectorGraphicItem.get(); }

    std::string GetSVGDocumentXML() const { return m_pDocument->toString(); };
    bool        SetSVGDocumentXML(CXMLNode* xmlNode);

    bool IsDisplayCleared() const { return m_pVectorGraphicDisplay->IsCleared(); }
private:
    CResource*                                        m_pResource;
    CClientManager*                                   m_pManager;
    std::unique_ptr<SVGDocument>                      m_pDocument;
    std::unique_ptr<CVectorGraphicItem>               m_pVectorGraphicItem;
    std::unique_ptr<CClientVectorGraphicDisplay>      m_pVectorGraphicDisplay;

    bool m_bIsDestroyed = false;
};
