/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLNodeImpl.cpp
 *  PURPOSE:     XML node class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

CXMLNodeImpl::CXMLNodeImpl(pugi::xml_node& node, bool usingIDs, CXMLNodeImpl* parent) : m_ID(INVALID_XML_ID), m_node(node), m_parent(parent)
{
    if (usingIDs)
        m_ID = CXMLArray::PopUniqueID(this);
}

CXMLNodeImpl::~CXMLNodeImpl()
{
    if (m_ID != INVALID_XML_ID)
        CXMLArray::PushUniqueID(this);
}

CXMLNode* CXMLNodeImpl::CreateChild(const std::string& tagName, CXMLNode* insertAfter)
{
    auto node = m_node.append_child(tagName.c_str());
    auto wrapper = std::make_unique<CXMLNodeImpl>(node, m_ID != INVALID_XML_ID, this);
    if (insertAfter)
    {
        auto matchingIter = std::find_if(m_children.begin(), m_children.end(), [insertAfter](const auto& p) { return insertAfter == p.get(); });
        if (matchingIter != m_children.end())
            m_children.insert(matchingIter++, std::move(wrapper));
        else
            m_children.push_back(std::move(wrapper));
    }
    else
        m_children.push_back(std::move(wrapper));
    return m_children.back().get();
}

void CXMLNodeImpl::RemoveChild(CXMLNode* node)
{
    // Remove from pugixml node
    m_node.remove_child(reinterpret_cast<CXMLNodeImpl*>(node)->GetNode());

    // Remove from child list
    m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [node](const auto& entry) { return node == entry.get(); }), m_children.end());
}

unsigned int CXMLNodeImpl::GetChildCount() const
{
    return static_cast<unsigned int>(m_children.size());
}

CXMLNode* CXMLNodeImpl::GetChild(unsigned int index) const
{
    // Lookup the node
    for (auto& child : m_children)
    {
        if (index == 0)
            return child.get();
        --index;
    }

    // No such node
    return nullptr;
}

CXMLNode* CXMLNodeImpl::GetChild(const std::string& tagName, unsigned int index) const
{
    // Lookup the node
    for (auto& node : m_children)
    {
        if (node->GetTagName() == tagName)
        {
            if (index == 0)
                return node.get();
            --index;
        }
    }

    // No such node
    return nullptr;
}

const std::string CXMLNodeImpl::GetTagName() const
{
    return m_node.name();
}

void CXMLNodeImpl::SetTagName(const std::string& string)
{
    m_node.set_name(string.c_str());
}

const std::string CXMLNodeImpl::GetTagContent() const
{
    const char* subText = m_node.child_value();
    return subText ? std::string(subText) : std::string("");
}

bool CXMLNodeImpl::GetTagContent(bool& content) const
{
    const char* szText = m_node.child_value();
    if (!szText)
        return false;

    std::string strText = szText;
    if (strText == "1")
    {
        content = true;
        return true;
    }
    if (strText == "0")
    {
        content = false;
        return true;
    }
    return false;
}

bool CXMLNodeImpl::GetTagContent(int& content)
{
    const char* text = m_node.child_value();
    if (!text)
        return false;

    return StringToNumber(text, content);
}

bool CXMLNodeImpl::GetTagContent(unsigned int& content)
{
    const char* text = m_node.child_value();
    if (!text)
        return false;

    return StringToNumber(text, content);
}

bool CXMLNodeImpl::GetTagContent(float& content)
{
    const char* text = m_node.child_value();
    if (!text)
        return false;

    return StringToNumber(text, content);
}

void CXMLNodeImpl::SetTagContent(const std::string& content, bool cdata)
{
    SetTagContent(content.c_str(), cdata);
}

void CXMLNodeImpl::SetTagContent(const char* text, bool cdata)
{
    // Remove children if any
    for (auto& child : m_node.children())
        m_node.remove_child(child);
    m_children.clear();

    m_node.append_child(cdata ? pugi::node_cdata : pugi::node_pcdata).set_value(text);
}

void CXMLNodeImpl::SetTagContent(bool content)
{
    if (content)
        SetTagContent("1");
    else
        SetTagContent("0");
}

void CXMLNodeImpl::SetTagContent(int content)
{
    SetTagContent(std::to_string(content));
}

void CXMLNodeImpl::SetTagContent(unsigned int content)
{
    SetTagContent(std::to_string(content));
}

void CXMLNodeImpl::SetTagContent(float content)
{
    SetTagContent(std::to_string(content));
}

void CXMLNodeImpl::SetTagContentf(const char* format, ...)
{
    // Convert the formatted string to a string (MAX 1024 BYTES) and set it
    char    buffer[1024];
    va_list va;
    va_start(va, format);
    VSNPRINTF(buffer, 1024, format, va);
    va_end(va);
    SetTagContent(buffer);
}

int CXMLNodeImpl::GetLine() const
{
    // TODO
    return 0;
}

bool CXMLNodeImpl::RemoveAttribute(const std::string& name)
{
    bool removed = false;

    // Remove from attributes list
    m_attributes.erase(std::remove_if(m_attributes.begin(), m_attributes.end(),
                                      [&removed, name](const auto& node) {
                                          if (node->GetName() == name)
                                          {
                                              removed = true;
                                              return true;
                                          }
                                          return false;
                                      }),
                       m_attributes.end());

    // Remove from pugixml node
    m_node.remove_attribute(name.c_str());

    return removed;
}

CXMLAttribute* CXMLNodeImpl::AddAttribute(const std::string& name)
{
    auto xmlAttribute = m_node.append_attribute(name.c_str());
    auto wrapper = std::make_unique<CXMLAttributeImpl>(xmlAttribute, m_ID != INVALID_XML_ID);
    AddAttribute(std::move(wrapper));
    return m_attributes.back().get();
}

CXMLAttribute* CXMLNodeImpl::GetAttribute(const std::string& name) const
{
    for (auto& attribute : m_attributes)
        if (attribute->GetName() == name)
            return attribute.get();
    return nullptr;
}

SString CXMLNodeImpl::GetCommentText() const
{
    // TODO
    return "";

    //SString        strComment;
    //pugi::xml_node pCommentNode = m_node.previous_sibling();
    //if (pCommentNode && pCommentNode.type() == pugi::node_comment)
    //{
    //    strComment = pCommentNode.value();
    //    // Remove indents
    //    std::vector<SString> lineList;
    //    ReadTokenSeparatedList("\n", strComment, lineList);
    //    strComment = SString::Join("\n", lineList);
    //}
    //return strComment;
}

//
// Set comment text for this node.
// Leading blank line can only be inserted when creating the comment.
//
void CXMLNodeImpl::SetCommentText(const char* szCommentText, bool bLeadingBlankLine)
{
    // TODO

    //// If previous sibling is not a comment, then insert one (with blank line if required)
    //pugi::xml_node pCommentNode = m_node.previous_sibling();
    //if (!pCommentNode || pCommentNode.type() != pugi::node_comment)
    //{
    //    if (bLeadingBlankLine)
    //    {
    //        pugi::xml_node pBlankLine = m_node.parent().insert_child_before(pugi::node_comment, m_node);
    //    }
    //    pCommentNode = m_node.parent().insert_child_before(pugi::node_comment, m_node);
    //}

    //// Calc indent
    //SString   strIndent = "     ";
    //CXMLNode* pTemp = this;
    //while (pTemp = pTemp->GetParent())
    //{
    //    strIndent += "    ";
    //}

    //// Apply indent
    //std::vector<SString> lineList;
    //SStringX(szCommentText).Split("\n", lineList);
    //for (uint i = 1; i < lineList.size(); ++i)
    //{
    //    lineList[i] = strIndent + lineList[i];
    //}

    //// Compose final comment string
    //SString strComment = " " + SString::Join("\n", lineList) + " ";
    //pCommentNode.set_value(strComment);
}

void CXMLNodeImpl::RemoveAllChildren()
{
    for (auto& child : m_children)
        m_node.remove_child(reinterpret_cast<CXMLNodeImpl*>(child.get())->m_node);
    m_children.clear();
}

void CXMLNodeImpl::AddAttribute(std::unique_ptr<CXMLAttribute> attribute)
{
    m_attributes.push_back(std::move(attribute));
}

void CXMLNodeImpl::AddChild(std::unique_ptr<CXMLNode> child)
{
    m_children.push_back(std::move(child));
}

bool CXMLNodeImpl::StringToNumber(const char* string, long& value)
{
    try
    {
        value = std::stol(string);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CXMLNodeImpl::StringToNumber(const char* string, unsigned int& value)
{
    try
    {
        // Sadly there's no stoui
        unsigned long temp = std::stoul(string);
        if (temp > std::numeric_limits<unsigned int>::max())
            return false;
        value = temp;
        return true;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CXMLNodeImpl::StringToNumber(const char* string, int& value)
{
    try
    {
        value = std::stoi(string);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CXMLNodeImpl::StringToNumber(const char* string, float& value)
{
    try
    {
        value = std::stof(string);
    }
    catch (...)
    {
        return false;
    }
    return true;
}
