/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

#include <pugixml.hpp>
#include <xml/CXMLNode.h>

class CXMLNodeImpl : public CXMLNode
{
public:
    // CXMLNode Interface
    CXMLNodeImpl(pugi::xml_node &node, bool bUsingIDs);
    ~CXMLNodeImpl();

    CXMLNode *GetParent() override { return m_pParent; }
    CXMLNode *CreateChild(const std::string& strName) override;
    void RemoveChild(CXMLNode *pNode) override;

    void RemoveAllChildren() override;
    unsigned int GetChildCount() override;
    CXMLNode *GetChild(unsigned int uiIndex) override;
    CXMLNode *GetChild(const std::string& strTagName, unsigned int uiIndex = 0) override;
    const std::list<std::unique_ptr<CXMLNode>> &GetChildren() override
    {
        return m_Children;
    }

    const std::string GetTagName() override;
    void SetTagName(const std::string &strString) override;

    const std::string GetTagContent() override;
    bool GetTagContent(bool &bContent) override;
    bool GetTagContent(int &iContent) override;
    bool GetTagContent(unsigned int &uiContent) override;
    bool GetTagContent(float &fContent) override;

    void SetTagContent(const std::string& strContent, bool bCDATA = false) override;
    void SetTagContent(const char *szContent, bool bCDATA = false) override;
    void SetTagContent(bool bContent) override;
    void SetTagContent(int iContent) override;
    void SetTagContent(unsigned int uiContent) override;
    void SetTagContent(float fContent) override;
    void SetTagContentf(const char *szFormat, ...) override;

    CXMLAttribute *AddAttribute(const std::string &strName) override;
    bool RemoveAttribute(const std::string &strName) override;
    CXMLAttribute *GetAttribute(const std::string &strName) override;
    const std::list<std::unique_ptr<CXMLAttribute>> &GetAttributes() override
    {
        return m_Attributes;
    }

    // CXMLCommon Interface
    eXMLClass GetClassType() override { return CXML_NODE; };
    unsigned long GetID() override { return m_ulID; };

    // CXMLNodeImpl
    void AddAttribute(std::unique_ptr<CXMLAttribute> pAttribute);
    void AddChild(std::unique_ptr<CXMLNode> pNode);
    pugi::xml_node &GetNode() { return m_node; }

private:
    bool StringToNumber(const char *szString, long &lValue);
    bool StringToNumber(const char *szString, unsigned int &uiValue);
    bool StringToNumber(const char *szString, int &lValue);
    bool StringToNumber(const char *szString, float &fValue);

private:
    unsigned long m_ulID;
    pugi::xml_node m_node;
    std::list<std::unique_ptr<CXMLAttribute>> m_Attributes;
    std::list<std::unique_ptr<CXMLNode>> m_Children;
    CXMLNodeImpl *m_pParent;
};
