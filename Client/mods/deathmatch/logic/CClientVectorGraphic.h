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

constexpr float DPI() { return 96.0; }

class CClientVectorGraphic : public CClientTexture
{
    DECLARE_CLASS(CClientVectorGraphic, CClientTexture)
public:
    CClientVectorGraphic(CClientManager* pManager, ElementID ID, std::unique_ptr<CVectorGraphicItem> pVectorGraphicItem);
    ~CClientVectorGraphic() = default;

    CResource* GetResource() { return m_pResource; }
    void       SetResource(CResource* pResource) { m_pResource = pResource; }

    eClientEntityType GetType() const { return CCLIENTVECTORGRAPHIC; }

    void CreateDocument();

    std::variant<bool, int> AddRect(std::variant<float, std::string> x, std::variant<float, std::string> y, std::variant<float, std::string> width, std::variant<float, std::string> height, std::variant<float, std::string> rx, std::variant<float, std::string> ry, float pathLength, std::string fill);

    std::variant<bool, int> AddCircle(std::variant<float, std::string> cx, std::variant<float, std::string> cy, float radius, float pathLength, std::string fill);

    bool LoadFromFile(std::string strFilePath);
    bool LoadFromData(std::string strData);

    void Destroy();
    bool IsDestroyed() const { return m_bIsDestroyed; }

    SVGDocument*        GetSVGDocument() const { return m_pDocument.get(); }
    CVectorGraphicItem* GetRenderItem() const { return m_pVectorGraphicItem.get(); }

    bool IsDisplayCleared() const { return m_pVectorGraphicDisplay->IsCleared(); }
private:
    CResource*                                        m_pResource;
    CClientManager*                                   m_pManager;
    std::unique_ptr<SVGDocument>                      m_pDocument;
    std::unique_ptr<CVectorGraphicItem>               m_pVectorGraphicItem;
    std::unique_ptr<CClientVectorGraphicDisplay>      m_pVectorGraphicDisplay;

    int m_iShapeCount;
    
    bool m_bIsDestroyed;
};
