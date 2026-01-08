/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientVectorGraphic.h"
#include <lunasvg.h>

CClientVectorGraphic::CClientVectorGraphic(CClientManager* pManager, ElementID ID, CVectorGraphicItem* pVectorGraphicItem)
    : ClassInit(this), CClientTexture(pManager, ID, pVectorGraphicItem)
{
    SetTypeName("svg");

    m_pManager = pManager;
    m_pVectorGraphicDisplay = m_pManager->GetDisplayManager()->CreateVectorGraphicDisplay(this);

    // Generate the default XML document
    SString defaultXmlString = SString("<svg viewBox='0 0 %u %u'></svg>", pVectorGraphicItem->m_uiSizeX, pVectorGraphicItem->m_uiSizeY);

    m_pXMLString = std::move(g_pCore->GetXML()->ParseString(defaultXmlString));
    m_pXMLDocument = m_pXMLString->node;
}

CClientVectorGraphic::~CClientVectorGraphic()
{
    Unlink();
}

bool CClientVectorGraphic::LoadFromString(std::string strData)
{
    std::unique_ptr<SXMLString> xmlString = g_pCore->GetXML()->ParseString(strData.c_str());

    if (!xmlString)
        return false;

    m_pXMLString = std::move(xmlString);

    return SetDocument(m_pXMLString->node);
}

bool CClientVectorGraphic::SetDocument(CXMLNode* node)
{
    if (!node || !node->IsValid())
        return false;

    if (m_pXMLString && m_pXMLString->node != node)
        m_pXMLString = nullptr;

    m_pXMLDocument = node;
    m_pSVGDocument = lunasvg::Document::loadFromData(node->ToString());

    m_pVectorGraphicDisplay->Update();

    return true;
}

bool CClientVectorGraphic::SetUpdateCallback(CLuaFunctionRef& funcRef)
{
    if (!VERIFY_FUNCTION(funcRef))
        return false;

    m_updateCallbackRef = funcRef;
    return true;
}

bool CClientVectorGraphic::RemoveUpdateCallback()
{
    if (!std::holds_alternative<CLuaFunctionRef>(m_updateCallbackRef))
        return false;

    m_updateCallbackRef = false;
    return true;
}

void CClientVectorGraphic::OnUpdate()
{
    m_pVectorGraphicDisplay->UpdateTexture();

    if (std::holds_alternative<CLuaFunctionRef>(m_updateCallbackRef))
    {
        auto& func = std::get<CLuaFunctionRef>(m_updateCallbackRef);
        auto state = func.GetLuaVM();

        if (VERIFY_FUNCTION(func) && state != NULL)
        {
            CLuaMain& luaMain = lua_getownercluamain(state);

            CLuaArguments arguments;
            arguments.PushElement(this);
            arguments.Call(&luaMain, func);
        }
    }
}

void CClientVectorGraphic::Unlink()
{
    m_bIsDestroyed = true;
    CClientRenderElement::Unlink();
}
