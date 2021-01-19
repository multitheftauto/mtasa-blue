/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientVectorGraphic.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <svgdocument.h>
#include "CClientVectorGraphicDisplay.h"

using namespace lunasvg;

class CClientVectorGraphic : public CClientTexture
{
    DECLARE_CLASS(CClientVectorGraphic, CClientTexture)
public:
    CClientVectorGraphic(CClientManager* pManager, ElementID ID, CVectorGraphicItem* pWebBrowserItem);
    ~CClientVectorGraphic();

    CResource* GetResource() { return m_pResource; }
    void       SetResource(CResource* pResource) { m_pResource = pResource; }

    eClientEntityType GetType() const { return CCLIENTVECTORGRAPHIC; }

    void CreateDocument();
    void UpdateTexture();
    void ClearTexture();

    std::variant<bool, int> AddRect(std::variant<float, std::string> x, std::variant<float, std::string> y, std::variant<float, std::string> width, std::variant<float, std::string> height, std::variant<float, std::string> rx, std::variant<float, std::string> ry, float pathLength, std::string fill);

    std::variant<bool, int> CClientVectorGraphic::AddCircle(std::variant<float, std::string> cx, std::variant<float, std::string> cy, float radius, float pathLength, std::string fill);

    void LoadFromFile(std::string strFilePath);

    const bool IsDisplayCleared() { return m_pVectorGraphicDisplay->IsCleared(); }
private:
    SVGDocument*                     m_pDocument;
    CResource*                       m_pResource;
    CClientManager*                  m_pManager;
    CVectorGraphicItem*              m_pVectorGraphicItem;
    CClientVectorGraphicDisplay*     m_pVectorGraphicDisplay;

    int m_iShapeCount;
    
    bool m_bHasUpdated;
};
