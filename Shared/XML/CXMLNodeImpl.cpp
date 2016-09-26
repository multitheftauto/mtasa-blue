/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"

CXMLNodeImpl::CXMLNodeImpl(pugi::xml_node &node, bool bUsingIDs, CXMLNodeImpl* pParent) : 
    m_ulID(INVALID_XML_ID), 
    m_node(node),
    m_pParent(pParent)
{
    if (bUsingIDs)
        m_ulID = CXMLArray::PopUniqueID(this);
}

CXMLNodeImpl::~CXMLNodeImpl()
{
    if (m_ulID != INVALID_XML_ID)
        CXMLArray::PushUniqueID(this);
}

CXMLNode *CXMLNodeImpl::CreateChild(const std::string& strTagName)
{
    auto node = m_node.append_child(strTagName.c_str());
    auto wrapper = std::make_unique<CXMLNodeImpl>(node, m_ulID != INVALID_XML_ID, this);
    m_Children.push_back(std::move(wrapper));
    return m_Children.back().get();
}

void CXMLNodeImpl::RemoveChild(CXMLNode *pNode)
{
    // Remove from pugixml node
    m_node.remove_child(reinterpret_cast<CXMLNodeImpl*>(pNode)->GetNode());

    // Remove from child list
    m_Children.erase(std::remove_if(m_Children.begin(), m_Children.end(),
        [pNode](const auto &pEntry) 
        {
            return pNode == pEntry.get();
        }),
    m_Children.end());
}

unsigned int CXMLNodeImpl::GetChildCount()
{
    return static_cast<unsigned int>(m_Children.size());
}

CXMLNode *CXMLNodeImpl::GetChild(unsigned int uiIndex)
{
    // Lookup the node
    for (auto &pChild : m_Children)
    {
        if (uiIndex == 0)
        {
            return pChild.get();
        }
        --uiIndex;
    }

    // No such node
    return nullptr;
}

CXMLNode *CXMLNodeImpl::GetChild(const std::string& strTagName, unsigned int uiIndex)
{
    // Lookup the node
    for (auto &pNode : m_Children)
    {
        if (pNode->GetTagName() == strTagName)
        {
            if (uiIndex == 0)
            {
                return pNode.get();
            }
            --uiIndex;
        }
    }

    // No such node
    return nullptr;
}

const std::string CXMLNodeImpl::GetTagName() { 
    return m_node.name(); 
}

void CXMLNodeImpl::SetTagName(const std::string &strString)
{
    m_node.set_name(strString.c_str());
}

const std::string CXMLNodeImpl::GetTagContent()
{
    const char *szSubText = m_node.child_value();
    return szSubText ? std::string(szSubText) : std::string("");
}

bool CXMLNodeImpl::GetTagContent(bool &bContent)
{
    const char *szText = m_node.child_value();
    if (!szText)
        return false;

    std::string strText = szText;
    if(strText == "1") 
    {
        bContent = true;
        return true;
    } 
    if (strText == "0")
    {
        bContent = false;
        return true;
    }
    return false; 
}

bool CXMLNodeImpl::GetTagContent(int &iContent)
{
    const char *szText = m_node.child_value();
    if (!szText)
        return false;

    return StringToNumber(szText, iContent);
}

bool CXMLNodeImpl::GetTagContent(unsigned int &uiContent)
{
    const char *szText = m_node.child_value();
    if (!szText)
        return false;

    return StringToNumber(szText, uiContent);
}

bool CXMLNodeImpl::GetTagContent(float &fContent)
{
    const char *szText = m_node.child_value();
    if (!szText)
        return false;

    return StringToNumber(szText, fContent);
}

void CXMLNodeImpl::SetTagContent(const std::string & strContent, bool bCDATA)
{
    SetTagContent(strContent.c_str(), bCDATA);
}

// TODO CDATA?
void CXMLNodeImpl::SetTagContent(const char *szText, bool bCDATA)
{
    // Remove children if any
    for (auto &child : m_node.children())
        m_node.remove_child(child);
    m_Children.clear();

    m_node.text().set(szText);
}

void CXMLNodeImpl::SetTagContent(bool bContent)
{
    if (bContent)
        SetTagContent("1");
    else
        SetTagContent("0");
}

void CXMLNodeImpl::SetTagContent(int iContent)
{
    SetTagContent(std::to_string(iContent));
}

void CXMLNodeImpl::SetTagContent(unsigned int uiContent)
{
    SetTagContent(std::to_string(uiContent));
}

void CXMLNodeImpl::SetTagContent(float fContent)
{
    SetTagContent(std::to_string(fContent));
}

void CXMLNodeImpl::SetTagContentf(const char *szFormat, ...)
{
    // Convert the formatted string to a string (MAX 1024 BYTES) and set it
    char szBuffer[1024];
    va_list va;
    va_start(va, szFormat);
    VSNPRINTF(szBuffer, 1024, szFormat, va);
    va_end(va);
    SetTagContent(szBuffer);
}

bool CXMLNodeImpl::RemoveAttribute(const std::string &strName)
{
    bool bRemoved = false;

    // Remove from attributes list
    m_Attributes.erase(std::remove_if(m_Attributes.begin(), m_Attributes.end(),
        [&bRemoved, strName](const auto &pNode) 
        {
            if (pNode->GetName() == strName)
            {
                bRemoved = true;
                return true;
            }
            return false;
        }),
    m_Attributes.end());

    // Remove from pugixml node
    m_node.remove_attribute(strName.c_str());

    return bRemoved;
}

CXMLAttribute *CXMLNodeImpl::AddAttribute(const std::string &strName)
{
    auto xmlAttribute = m_node.append_attribute(strName.c_str());
    auto wrapper = std::make_unique<CXMLAttributeImpl>(xmlAttribute, m_ulID != INVALID_XML_ID);
    AddAttribute(std::move(wrapper));
    return m_Attributes.back().get();
}

CXMLAttribute *CXMLNodeImpl::GetAttribute(const std::string &strName)
{
    for (auto &pAttribute : m_Attributes)
    {
        if (pAttribute->GetName() == strName)
        {
            return pAttribute.get();
        }
    }
    return nullptr;
}

void CXMLNodeImpl::RemoveAllChildren()
{
    for (auto &pChild : m_Children)
        m_node.remove_child(reinterpret_cast<CXMLNodeImpl *>(pChild.get())->m_node);
}

void CXMLNodeImpl::AddAttribute(std::unique_ptr<CXMLAttribute> pAttribute)
{
    m_Attributes.push_back(std::move(pAttribute));
}

void CXMLNodeImpl::AddChild(std::unique_ptr<CXMLNode> pChild)
{
    m_Children.push_back(std::move(pChild));
}

bool CXMLNodeImpl::StringToNumber(const char *szString, long &lValue)
{
    try
    {
        lValue = std::stol(szString);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CXMLNodeImpl::StringToNumber(const char *szString, unsigned int &uiValue)
{
    try
    {
        // Sadly there's no stoui
        unsigned long temp = std::stoul(szString);
        if (temp > std::numeric_limits<unsigned int>::max())
            return false;
        uiValue = temp;
        return true;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CXMLNodeImpl::StringToNumber(const char *szString, int &iValue)
{
    try
    {
        iValue = std::stoi(szString);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CXMLNodeImpl::StringToNumber(const char *szString, float &fValue)
{
    try
    {
        fValue = std::stof(szString);
    }
    catch (...)
    {
        return false;
    }
    return true;
}
