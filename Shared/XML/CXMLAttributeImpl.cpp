/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLAttributeImpl.cpp
 *  PURPOSE:     XML attribute class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
using namespace tinyxml2;

#define XML_ATTRIBUTE_VALUE_BUFFER 40

CXMLAttributeImpl::CXMLAttributeImpl(CXMLAttributesImpl& Attributes, XMLElement& Node, const std::string& strName)
    : m_ulID(INVALID_XML_ID), m_bUsingIDs(Attributes.IsUsingIDs()), m_Attributes(Attributes), m_Node(Node), m_pAttribute(nullptr)
{
    m_bDeleteAttribute = true;
    Node.SetAttribute(strName.c_str(), "");
    m_pAttribute = Node.FindAttribute(strName.c_str());

    m_Attributes.AddToList(this);

    if (m_bUsingIDs)
        m_ulID = CXMLArray::PopUniqueID(this);
}

CXMLAttributeImpl::CXMLAttributeImpl(CXMLAttributesImpl& Attributes, XMLElement& Node, const XMLAttribute& Attribute)
    : m_ulID(INVALID_XML_ID), m_bUsingIDs(Attributes.IsUsingIDs()), m_Attributes(Attributes), m_Node(Node), m_pAttribute(&Attribute)
{
    m_bDeleteAttribute = true;

    m_Attributes.AddToList(this);

    if (m_bUsingIDs)
        m_ulID = CXMLArray::PopUniqueID(this);
}

CXMLAttributeImpl::~CXMLAttributeImpl()
{
    if (m_bUsingIDs)
        CXMLArray::PushUniqueID(this);

    if (m_bDeleteAttribute && m_pAttribute)
    {
        m_Node.DeleteAttribute(m_pAttribute->Name());
    }

    m_Attributes.RemoveFromList(this);
}

const std::string CXMLAttributeImpl::GetName() const
{
    return m_pAttribute ? std::string(m_pAttribute->Name()) : std::string{};
}

const std::string& CXMLAttributeImpl::GetValue() const
{
    if (m_pAttribute)
    {
        const char* szValue = m_pAttribute->Value();
        if (szValue)
        {
            m_strValueCache = szValue;
            return m_strValueCache;
        }
    }
    static std::string empty;
    return empty;
}

void CXMLAttributeImpl::SetValue(const char* szValue)
{
    if (m_pAttribute)
    {
        m_Node.SetAttribute(m_pAttribute->Name(), szValue);
        m_pAttribute = m_Node.FindAttribute(m_pAttribute->Name());
    }
}

void CXMLAttributeImpl::SetValue(bool bValue)
{
    // Convert to string and set it
    char szBuffer[2];
    szBuffer[1] = 0;

    if (bValue)
    {
        szBuffer[0] = '1';
    }
    else
    {
        szBuffer[0] = '0';
    }

    SetValue(szBuffer);
}

void CXMLAttributeImpl::SetValue(int iValue)
{
    // Convert to string and set it
    char szBuffer[XML_ATTRIBUTE_VALUE_BUFFER];
    snprintf(szBuffer, XML_ATTRIBUTE_VALUE_BUFFER - 1, "%i", iValue);
    SetValue(szBuffer);
}

void CXMLAttributeImpl::SetValue(unsigned int uiValue)
{
    // Convert to string and set it
    char szBuffer[XML_ATTRIBUTE_VALUE_BUFFER];
    snprintf(szBuffer, XML_ATTRIBUTE_VALUE_BUFFER - 1, "%u", uiValue);
    SetValue(szBuffer);
}

void CXMLAttributeImpl::SetValue(float fValue)
{
    // Convert to string and set it
    char szBuffer[XML_ATTRIBUTE_VALUE_BUFFER];
    snprintf(szBuffer, XML_ATTRIBUTE_VALUE_BUFFER - 1, "%f", fValue);
    SetValue(szBuffer);
}

void CXMLAttributeImpl::DeleteWrapper()
{
    // Delete us, but don't delete the attribute
    m_bDeleteAttribute = false;
    delete this;
}
