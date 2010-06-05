/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLAttributeImpl.h
*  PURPOSE:     XML attribute class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLATTRIBUTEIMPL_H
#define __CXMLATTRIBUTEIMPL_H

#include <tinyxml.h>
#include <string>

#include <xml/CXMLAttribute.h>

class CXMLAttributesImpl;

class CXMLAttributeImpl : public CXMLAttribute
{
public:
                            CXMLAttributeImpl           ( class CXMLAttributesImpl& Attributes, TiXmlElement& Node, const std::string& strName );
                            CXMLAttributeImpl           ( class CXMLAttributesImpl& Attributes, TiXmlElement& Node, TiXmlAttribute& Attribute );
	virtual					~CXMLAttributeImpl          ( void );

    const std::string       GetName                     ( void ) const;
    const std::string&      GetValue                    ( void ) const;

    void                    SetValue                    ( const char* szValue );
    void                    SetValue                    ( bool bValue );
    void                    SetValue                    ( int iValue );
    void                    SetValue                    ( unsigned int uiValue );
    void                    SetValue                    ( float fValue );

    eXMLClass               GetClassType                ( void )    { return CXML_ATTR; };
    unsigned long           GetID                       ( void )    { return m_ulID; };

    void                    DeleteWrapper               ( void );

private:
    CXMLAttributesImpl&         m_Attributes;
    TiXmlElement&               m_Node;
    TiXmlAttribute&             m_Attribute;

    unsigned long               m_ulID;
    bool                        m_bDeleteAttribute;
};

#endif
