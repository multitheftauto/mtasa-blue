/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLAttributeImpl.h
 *  PURPOSE:     XML attribute class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <tinyxml.h>
#include <string>

#include <xml/CXMLAttribute.h>

class CXMLAttributesImpl;

class CXMLAttributeImpl : public CXMLAttribute
{
public:
    CXMLAttributeImpl(class CXMLAttributesImpl& Attributes, TiXmlElement& Node, const std::string& strName);
    CXMLAttributeImpl(class CXMLAttributesImpl& Attributes, TiXmlElement& Node, TiXmlAttribute& Attribute);
    virtual ~CXMLAttributeImpl();

    const std::string  GetName() const;
    const std::string& GetValue() const;

    void SetValue(const char* szValue);
    void SetValue(bool bValue);
    void SetValue(int iValue);
    void SetValue(unsigned int uiValue);
    void SetValue(float fValue);

    eXMLClass     GetClassType() { return CXML_ATTR; };
    unsigned long GetID() { return m_ulID; };
    bool          IsUsingIDs() { return m_bUsingIDs; };

    void DeleteWrapper();

private:
    unsigned long m_ulID;
    const bool    m_bUsingIDs;
    bool          m_bDeleteAttribute;

    CXMLAttributesImpl& m_Attributes;
    TiXmlElement&       m_Node;
    TiXmlAttribute&     m_Attribute;
};
