/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLNodeImpl.h
 *  PURPOSE:     XML node class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <pugixml.hpp>
#include <xml/CXMLCommon.h>
#include <xml/CXMLNode.h>

class CXMLNodeImpl : public CXMLNode
{
public:
    // CXMLNode Interface
    CXMLNodeImpl(pugi::xml_node& node, bool usingIDs, CXMLNodeImpl* parent = nullptr);
    ~CXMLNodeImpl();

    inline CXMLNode* GetParent() const override { return m_parent; }
    CXMLNode*        CreateChild(const std::string& name, CXMLNode* insertAfter = nullptr) override;
    void             RemoveChild(CXMLNode* node) override;

    void                                               RemoveAllChildren() override;
    unsigned int                                       GetChildCount() const override;
    CXMLNode*                                          GetChild(unsigned int index) const override;
    CXMLNode*                                          GetChild(const std::string& tagName, unsigned int index = 0) const override;
    inline const std::list<std::unique_ptr<CXMLNode>>& GetChildren() const override { return m_children; }

    const std::string GetTagName() const override;
    void              SetTagName(const std::string& string) override;

    const std::string GetTagContent() const override;
    bool              GetTagContent(bool& content) const override;
    bool              GetTagContent(int& content) override;
    bool              GetTagContent(unsigned int& content) override;
    bool              GetTagContent(float& content) override;

    void SetTagContent(const std::string& content, bool cdata = false) override;
    void SetTagContent(const char* content, bool cdata = false) override;
    void SetTagContent(bool content) override;
    void SetTagContent(int content) override;
    void SetTagContent(unsigned int content) override;
    void SetTagContent(float content) override;
    void SetTagContentf(const char* format, ...) override;

    int         GetLine() const override;
    inline bool IsValid() const override { return m_ID != INVALID_XML_ID; }

    CXMLAttribute*                                          AddAttribute(const std::string& name) override;
    bool                                                    RemoveAttribute(const std::string& name) override;
    CXMLAttribute*                                          GetAttribute(const std::string& name) const override;
    inline const std::list<std::unique_ptr<CXMLAttribute>>& GetAttributes() const override { return m_attributes; }

    SString GetCommentText() const override;
    void    SetCommentText(const char* commentText, bool leadingBlankLine = false) override;

    // CXMLCommon Interface
    inline eXMLClass     GetClassType() const override { return eXMLClass::CXML_NODE; }
    inline unsigned long GetID() const override { return m_ID; }

    // CXMLNodeImpl
    void                   AddAttribute(std::unique_ptr<CXMLAttribute> attribute);
    void                   AddChild(std::unique_ptr<CXMLNode> node);
    inline pugi::xml_node& GetNode() { return m_node; }

private:
    bool StringToNumber(const char* string, long& value);
    bool StringToNumber(const char* string, unsigned int& value);
    bool StringToNumber(const char* string, int& value);
    bool StringToNumber(const char* string, float& value);

    unsigned long                             m_ID;
    pugi::xml_node                            m_node;
    std::list<std::unique_ptr<CXMLAttribute>> m_attributes;
    std::list<std::unique_ptr<CXMLNode>>      m_children;
    CXMLNodeImpl*                             m_parent;
};
