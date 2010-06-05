/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLAttributeImpl.cpp
*  PURPOSE:     XML attribute class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define XML_ATTRIBUTE_VALUE_BUFFER  40

CXMLAttributeImpl::CXMLAttributeImpl ( CXMLAttributesImpl& Attributes, TiXmlElement& Node, const std::string& strName ) :
    m_Attributes ( Attributes ),
    m_Node ( Node ),
    m_Attribute ( *new TiXmlAttribute ( strName.c_str (), "" ) )
{
    // Init and link it to the node
    m_bDeleteAttribute = true;
    Node.AddAttribute ( m_Attribute );

    // Add us to parent list, if any
    m_Attributes.AddToList ( this );

    // Add to array over XML stuff
    m_ulID = CXMLArray::PopUniqueID ( this );
}

CXMLAttributeImpl::CXMLAttributeImpl ( CXMLAttributesImpl& Attributes, TiXmlElement& Node, TiXmlAttribute& Attribute ) :
    m_Attributes ( Attributes ),
    m_Node ( Node ),
    m_Attribute ( Attribute )
{
    // Init
    m_bDeleteAttribute = true;

    // Add us to parent list, if any
    m_Attributes.AddToList ( this );

    // Add to array over XML stuff
    m_ulID = CXMLArray::PopUniqueID ( this );
}


CXMLAttributeImpl::~CXMLAttributeImpl ( void )
{
    // Remove from array over XML stuff
    CXMLArray::PushUniqueID ( this );

    // Delete the attribute from the node aswell if we're supposed to
    if ( m_bDeleteAttribute )
    {
        m_Node.RemoveAttribute ( GetName () );
    }

    // Remove us from parent list
    m_Attributes.RemoveFromList ( this );
}


const std::string CXMLAttributeImpl::GetName ( void ) const
{
    return std::string ( m_Attribute.Name () );
}


const std::string& CXMLAttributeImpl::GetValue ( void ) const
{
    return m_Attribute.ValueStr ();
}


void CXMLAttributeImpl::SetValue ( const char* szValue )
{
    m_Attribute.SetValue ( szValue );
}


void CXMLAttributeImpl::SetValue ( bool bValue )
{
    // Convert to string and set it
    char szBuffer [2];
    szBuffer [1] = 0;

    if ( bValue )
    {
        szBuffer [0] = '1';
    }
    else
    {
        szBuffer [0] = '0';
    }

    SetValue ( szBuffer );
}


void CXMLAttributeImpl::SetValue ( int iValue )
{
    // Convert to string and set it
    char szBuffer [XML_ATTRIBUTE_VALUE_BUFFER];
    snprintf ( szBuffer, XML_ATTRIBUTE_VALUE_BUFFER - 1, "%i", iValue );
    SetValue ( szBuffer );
}


void CXMLAttributeImpl::SetValue ( unsigned int uiValue )
{
    // Convert to string and set it
    char szBuffer [XML_ATTRIBUTE_VALUE_BUFFER];
    snprintf ( szBuffer, XML_ATTRIBUTE_VALUE_BUFFER - 1, "%u", uiValue );
    SetValue ( szBuffer );
}


void CXMLAttributeImpl::SetValue ( float fValue )
{
    // Convert to string and set it
    char szBuffer [XML_ATTRIBUTE_VALUE_BUFFER];
    snprintf ( szBuffer, XML_ATTRIBUTE_VALUE_BUFFER - 1, "%f", fValue );
    SetValue ( szBuffer );
}


void CXMLAttributeImpl::DeleteWrapper ( void )
{
    // Delete us, but don't delete the attribute
    m_bDeleteAttribute = false;
    delete this;
}
