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
#include <algorithm>
#include <cctype>
#include <vector>

namespace
{
    constexpr size_t MAX_DATA_URI_SIZE = 10 * 1024 * 1024;
    constexpr size_t MAX_LOG_MESSAGES = 10;

    CXMLAttribute* FindAttributeI(CXMLNode* node, const char* name)
    {
        if (!node)
            return nullptr;

        SString targetName = SStringX(name).ToLower();
        for (auto iter = node->GetAttributes().ListBegin(); iter != node->GetAttributes().ListEnd(); ++iter)
        {
            if (SString((*iter)->GetName()).CompareI(targetName))
                return *iter;
        }
        return nullptr;
    }

    bool IsHrefAllowed(const std::string& href)
    {
        if (href.empty())
            return true;

        if (href[0] == '#')
            return true;

        SString hrefStr(href);
        if (hrefStr.BeginsWithI("data:"))
        {
            if (hrefStr.BeginsWithI("data:image/svg"))
                return false;

            if (!hrefStr.BeginsWithI("data:image/"))
                return false;

            return href.size() <= MAX_DATA_URI_SIZE;
        }

        return false;
    }

    bool StyleContainsExternalReference(const std::string& styleValue)
    {
        SString styleLower = SString(styleValue).ToLower();
        return styleLower.find("url(") != std::string::npos ||
               styleLower.find("@import") != std::string::npos;
    }

    bool AttributeContainsExternalUrl(const std::string& value)
    {
        SString valueLower = SString(value).ToLower();

        size_t urlPos = valueLower.find("url(");
        if (urlPos == std::string::npos)
            return false;

        size_t start = urlPos + 4;

        while (start < valueLower.length() &&
               (valueLower[start] == ' ' || valueLower[start] == '"' || valueLower[start] == '\''))
            start++;

        if (start >= valueLower.length())
            return false;

        SString urlContent = valueLower.substr(start);

        if (urlContent.empty())
            return false;

        if (urlContent[0] == '#')
            return false;

        if (urlContent.BeginsWithI("data:image/") && !urlContent.BeginsWithI("data:image/svg"))
            return false;

        return true;
    }

    bool IsEventAttribute(const std::string& attrName)
    {
        return SString(attrName).BeginsWithI("on");
    }

    void AddLogMessage(std::vector<SString>& info, const SString& message)
    {
        if (info.size() < MAX_LOG_MESSAGES)
            info.push_back(message);
        else if (info.size() == MAX_LOG_MESSAGES)
            info.push_back("... additional warnings suppressed");
    }

    bool ProcessSVGNode(CXMLNode* node, int depth, std::vector<SString>& info)
    {
        if (!node)
            return false;

        if (depth > 100)
        {
            AddLogMessage(info, "SVG parsing limit exceeded");
            return true;
        }

        SString tagName = node->GetTagName();
        SString tagNameLower = tagName.ToLower();

        constexpr const char* blockedElements[] = {"script", "foreignobject", "handler"};
        for (const char* blocked : blockedElements)
        {
            if (tagNameLower == blocked)
            {
                AddLogMessage(info, SString("SVG element <%s> is not supported", tagName.c_str()));
                return true;
            }
        }

        const char* hrefAttrs[] = {"href", "xlink:href"};
        for (const char* attrName : hrefAttrs)
        {
            CXMLAttribute* attr = FindAttributeI(node, attrName);
            if (attr && !IsHrefAllowed(attr->GetValue()))
            {
                AddLogMessage(info, SString("SVG element <%s> contains unsupported content", tagName.c_str()));
                return true;
            }
        }

        std::vector<std::string> attrsToRemove;
        for (auto iter = node->GetAttributes().ListBegin(); iter != node->GetAttributes().ListEnd(); ++iter)
        {
            std::string attrName = (*iter)->GetName();
            std::string attrValue = (*iter)->GetValue();

            if (IsEventAttribute(attrName))
            {
                attrsToRemove.push_back(attrName);
                AddLogMessage(info, SString("SVG attribute '%s' on <%s> is not supported", attrName.c_str(), tagName.c_str()));
            }
            else if (AttributeContainsExternalUrl(attrValue))
            {
                attrsToRemove.push_back(attrName);
                AddLogMessage(info, SString("SVG attribute '%s' on <%s> contains unsupported content", attrName.c_str(), tagName.c_str()));
            }
        }

        for (const std::string& attrName : attrsToRemove)
            node->GetAttributes().Delete(attrName.c_str());

        CXMLAttribute* xmlBaseAttr = FindAttributeI(node, "xml:base");
        if (xmlBaseAttr)
        {
            node->GetAttributes().Delete(xmlBaseAttr->GetName().c_str());
            AddLogMessage(info, SString("SVG attribute 'xml:base' on <%s> is not supported", tagName.c_str()));
        }

        CXMLAttribute* styleAttr = FindAttributeI(node, "style");
        if (styleAttr)
        {
            SString styleLower = SString(styleAttr->GetValue()).ToLower();
            if (styleLower.find("@import") != std::string::npos)
            {
                node->GetAttributes().Delete(styleAttr->GetName().c_str());
                AddLogMessage(info, SString("SVG 'style' attribute on <%s> contains unsupported content", tagName.c_str()));
            }
        }

        std::vector<CXMLNode*> toRemove;
        for (unsigned int i = 0; i < node->GetSubNodeCount(); i++)
        {
            CXMLNode* child = node->GetSubNode(i);
            if (ProcessSVGNode(child, depth + 1, info))
                toRemove.push_back(child);
        }

        for (CXMLNode* child : toRemove)
            node->DeleteSubNode(child);

        return false;
    }

    bool ProcessSVGDocument(CXMLNode* root, std::vector<SString>& info)
    {
        if (!root)
            return false;
        return ProcessSVGNode(root, 0, info);
    }
}

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

    std::vector<SString> info;
    if (ProcessSVGDocument(node, info))
    {
        g_pClientGame->GetScriptDebugging()->LogWarning(nullptr, "SVG load failed: document contains unsupported content");
        return false;
    }

    for (const SString& msg : info)
        g_pClientGame->GetScriptDebugging()->LogInformation(nullptr, "%s", msg.c_str());

    std::string strSvgData = node->ToString();

    if (m_pXMLString && m_pXMLString->node != node)
        m_pXMLString = nullptr;

    m_pXMLDocument = node;
    m_pSVGDocument = lunasvg::Document::loadFromData(strSvgData);

    // Check if LunaSVG successfully parsed the document
    if (!m_pSVGDocument)
        return false;

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
