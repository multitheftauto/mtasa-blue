/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLAttributesImpl.cpp
*  PURPOSE:     XML attributes container class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CXMLAttributesImpl::CXMLAttributesImpl ( TiXmlElement& Node ) :
    m_Node ( Node )
{
    // Init
    m_bCanRemoveFromList = true;

    // Create the attributes
    CreateAttributes ();
}


CXMLAttributesImpl::~CXMLAttributesImpl ( void )
{
    // Delete all attribute wrappers (not from the xml tree)
    DeleteAttributes ();
}


unsigned int CXMLAttributesImpl::Count ( void )
{
    return static_cast < unsigned int > ( m_Attributes.size () );
}


CXMLAttribute* CXMLAttributesImpl::Find ( const char* szName )
{
    // Find the attribute in our attributelist
    std::list < CXMLAttribute* > ::const_iterator iter = m_Attributes.begin ();
    for ( ; iter != m_Attributes.end (); iter++ )
    {
        // Names match?
        if ( strcmp ( szName, (*iter)->GetName ().c_str () ) == 0 )
        {
            return *iter;
        }
    }

    // Couldn't find it
    return NULL;
}


CXMLAttribute* CXMLAttributesImpl::Get ( unsigned int uiIndex )
{
    // Find the attribute in our attributelist
    unsigned int uiCurrentIndex = 0;
    std::list < CXMLAttribute* > ::const_iterator iter = m_Attributes.begin ();
    for ( ; iter != m_Attributes.end (); iter++ )
    {
        // Index matches?
        if ( uiIndex == uiCurrentIndex++ )
        {
            return *iter;
        }
    }

    // Couldn't find it
    return NULL;
}


CXMLAttribute* CXMLAttributesImpl::Create ( const char* szName )
{
    // Does it already exist? Return it
    CXMLAttribute* pAttribute = Find ( szName );
    if ( pAttribute ) return pAttribute;

    // If not, create it
    return new CXMLAttributeImpl ( *this, m_Node, szName );
}


CXMLAttribute* CXMLAttributesImpl::Create ( const CXMLAttribute& Copy )
{
    CXMLAttribute* pTemp = Create ( Copy.GetName ().c_str () );
    pTemp->SetValue ( Copy.GetValue ().c_str () );
    return pTemp;
}


void CXMLAttributesImpl::DeleteAll ( void )
{
    // Delete each attribute
    m_bCanRemoveFromList = false;
    std::list < CXMLAttribute* > ::const_iterator iter = m_Attributes.begin ();
    for ( ; iter != m_Attributes.end (); iter++ )
    {
        delete *iter;
    }

    m_Attributes.clear ();
    m_bCanRemoveFromList = true;
}


TiXmlElement& CXMLAttributesImpl::GetNode ( void )
{
    return m_Node;
}


CXMLAttribute* CXMLAttributesImpl::AddToList ( CXMLAttribute* pAttribute )
{
    m_Attributes.push_back ( pAttribute );
    return pAttribute;
}


void CXMLAttributesImpl::RemoveFromList ( CXMLAttribute* pAttribute )
{
    if ( m_bCanRemoveFromList )
    {
        if ( !m_Attributes.empty() ) m_Attributes.remove ( pAttribute );
    }
}


void CXMLAttributesImpl::CreateAttributes ( void )
{
    // Grab the first attribute and iterate from there
    TiXmlAttribute* pAttrib = m_Node.FirstAttribute ();
    if ( pAttrib )
    {
        do
        {
            // Add it
            new CXMLAttributeImpl ( *this, m_Node, *pAttrib );
        }
        while ( pAttrib = pAttrib->Next () );
    }
}


void CXMLAttributesImpl::DeleteAttributes ( void )
{
    // Deleted every item in the list
    m_bCanRemoveFromList = false;
    std::list < CXMLAttribute* > ::const_iterator iter = m_Attributes.begin ();
    for ( ; iter != m_Attributes.end (); iter++ )
    {
        (*iter)->DeleteWrapper ();
    }

    // Clear the list
    m_Attributes.clear ();
    m_bCanRemoveFromList = true;
}
