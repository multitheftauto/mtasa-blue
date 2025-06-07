/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientVectorGraphicDisplay.h"

namespace lunasvg
{
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

    bool LoadFromString(std::string strData);

    eClientEntityType   GetType() const { return CCLIENTVECTORGRAPHIC; }
    CVectorGraphicItem* GetRenderItem() const { return static_cast<CVectorGraphicItem*>(m_pRenderItem); }

    lunasvg::Document* GetSVGDocument() const { return m_pSVGDocument.get(); }
    CXMLNode*          GetXMLDocument() const { return m_pXMLDocument; }

    CClientVectorGraphicDisplay* GetDisplay() const { return m_pVectorGraphicDisplay.get(); }

    bool IsDisplayCleared() const { return m_pVectorGraphicDisplay->IsCleared(); }
    bool IsDestroyed() const { return m_bIsDestroyed; }

    bool SetDocument(CXMLNode* node);

    bool                                SetUpdateCallback(CLuaFunctionRef& funcRef);
    std::variant<CLuaFunctionRef, bool> GetUpdateCallback() const { return m_updateCallbackRef; }
    bool                                RemoveUpdateCallback();

    void OnUpdate();

private:
    CResource*      m_pResource = nullptr;
    CClientManager* m_pManager;

    std::unique_ptr<lunasvg::Document> m_pSVGDocument = nullptr;
    std::unique_ptr<SXMLString>        m_pXMLString = nullptr;
    CXMLNode*                          m_pXMLDocument = nullptr;

    std::shared_ptr<CClientVectorGraphicDisplay> m_pVectorGraphicDisplay;

    std::variant<CLuaFunctionRef, bool> m_updateCallbackRef = false;

    bool m_bIsDestroyed = false;
};
