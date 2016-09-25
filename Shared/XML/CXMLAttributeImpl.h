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
#include <xml/CXMLAttribute.h>

class CXMLAttributeImpl : public CXMLAttribute
{
  public:
    CXMLAttributeImpl(pugi::xml_attribute &attribute, bool bUsingIDs);
    virtual ~CXMLAttributeImpl(void);

    const std::string GetName(void) const;
    const std::string GetValue(void) const;

    void SetValue(const std::string &strValue);
    void SetValue(const char *szValue);
    void SetValue(bool bValue);
    void SetValue(int iValue);
    void SetValue(unsigned int uiValue);
    void SetValue(float fValue);

    eXMLClass GetClassType(void) { return CXML_ATTR; };
    unsigned long GetID(void) { return m_ulID; };

  private:
    unsigned long m_ulID;
    pugi::xml_attribute m_Attribute;
};
