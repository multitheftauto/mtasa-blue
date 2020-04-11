/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
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
#include <xml/CXMLCommon.h>

class CXMLAttributeImpl : public CXMLAttribute
{
public:
    CXMLAttributeImpl(pugi::xml_attribute& attribute, bool usingIDs);
    ~CXMLAttributeImpl();

    const std::string GetName() const override;
    const std::string GetValue() const override;

    void SetValue(const std::string& value) override;
    void SetValue(const char* value) override;
    void SetValue(bool value) override;
    void SetValue(int value) override;
    void SetValue(unsigned int value) override;
    void SetValue(float value) override;

    inline eXMLClass     GetClassType() const override { return eXMLClass::CXML_ATTR; };
    inline unsigned long GetID() const override { return m_ID; };

private:
    unsigned long       m_ID;
    pugi::xml_attribute m_attribute;
};
