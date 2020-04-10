/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLAttributeImpl.h
 *  PURPOSE:     XML attribute class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <pugixml.hpp>
#include <xml/CXMLAttribute.h>

class CXMLAttributeImpl : public CXMLAttribute
{
public:
    CXMLAttributeImpl(pugi::xml_attribute& attribute, bool bUsingIDs);
    virtual ~CXMLAttributeImpl();

    const std::string GetName() const;
    const std::string GetValue() const;

    void SetValue(const std::string& strValue);
    void SetValue(const char* szValue);
    void SetValue(bool bValue);
    void SetValue(int iValue);
    void SetValue(unsigned int uiValue);
    void SetValue(float fValue);

    eXMLClass     GetClassType() { return CXML_ATTR; };
    unsigned long GetID() { return m_ulID; };

private:
    unsigned long       m_ulID;
    pugi::xml_attribute m_Attribute;
};
