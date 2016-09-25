/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"

CXMLAttributeImpl::CXMLAttributeImpl(pugi::xml_attribute &attribute, bool bUsingIDs) : 
    m_ulID(INVALID_XML_ID), 
    m_Attribute(attribute)
{
    if (bUsingIDs)
        m_ulID = CXMLArray::PopUniqueID(this);
}

CXMLAttributeImpl::~CXMLAttributeImpl()
{
    if (m_ulID != INVALID_XML_ID)
        CXMLArray::PushUniqueID(this);
}

const std::string CXMLAttributeImpl::GetName() const
{
    return std::string(m_Attribute.name());
}

const std::string CXMLAttributeImpl::GetValue() const
{
    return std::string(m_Attribute.value());
}

void CXMLAttributeImpl::SetValue(const std::string &strValue)
{
    SetValue(strValue.c_str());
}

void CXMLAttributeImpl::SetValue(const char *szValue)
{
    m_Attribute.set_value(szValue);
}

void CXMLAttributeImpl::SetValue(bool bValue)
{
    if(bValue)
        SetValue("1");
    else 
        SetValue("0");
}

void CXMLAttributeImpl::SetValue(int iValue)
{
    SetValue(std::to_string(iValue));
}

void CXMLAttributeImpl::SetValue(unsigned int uiValue)
{
    SetValue(std::to_string(uiValue));
}

void CXMLAttributeImpl::SetValue(float fValue)
{
    SetValue(std::to_string(fValue));
}
