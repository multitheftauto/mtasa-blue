/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLAttributeImpl.cpp
 *  PURPOSE:     XML attribute class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

CXMLAttributeImpl::CXMLAttributeImpl(pugi::xml_attribute& attribute, bool usingIDs) : m_ID(INVALID_XML_ID), m_attribute(attribute)
{
    if (usingIDs)
        m_ID = CXMLArray::PopUniqueID(this);
}

CXMLAttributeImpl::~CXMLAttributeImpl()
{
    if (m_ID != INVALID_XML_ID)
        CXMLArray::PushUniqueID(this);
}

const std::string CXMLAttributeImpl::GetName() const
{
    return std::string(m_attribute.name());
}

const std::string CXMLAttributeImpl::GetValue() const
{
    return std::string(m_attribute.value());
}

void CXMLAttributeImpl::SetValue(const char* value)
{
    m_attribute.set_value(value);
}

void CXMLAttributeImpl::SetValue(const std::string& value)
{
    SetValue(value.c_str());
}

void CXMLAttributeImpl::SetValue(const bool value)
{
    if (value)
        SetValue("1");
    else
        SetValue("0");
}

void CXMLAttributeImpl::SetValue(int value)
{
    SetValue(std::to_string(value));
}

void CXMLAttributeImpl::SetValue(unsigned int value)
{
    SetValue(std::to_string(value));
}

void CXMLAttributeImpl::SetValue(float value)
{
    SetValue(std::to_string(value));
}
