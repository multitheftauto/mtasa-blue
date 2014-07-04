/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLAttributesImpl.h
*  PURPOSE:     XML attributes container class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLATTRIBUTESIMPL_H
#define __CXMLATTRIBUTESIMPL_H

#include <tinyxml.h>
#include <xml/CXMLAttributes.h>

#include <list>

class CXMLAttributesImpl : public CXMLAttributes
{
public:
                                    CXMLAttributesImpl          ( TiXmlElement& Node );
                                    ~CXMLAttributesImpl         ( void );

    unsigned int                    Count                       ( void );
    class CXMLAttribute*            Find                        ( const char* szName );
    class CXMLAttribute*            Get                         ( unsigned int uiIndex );

    class CXMLAttribute*            Create                      ( const char* szName );
    class CXMLAttribute*            Create                      ( const class CXMLAttribute& Copy );
    void                            DeleteAll                   ( void );

    TiXmlElement&                   GetNode                     ( void );

    class CXMLAttribute*            AddToList                   ( class CXMLAttribute* pAttribute );
    void                            RemoveFromList              ( class CXMLAttribute* pAttribute );

    std::list < CXMLAttribute* > ::iterator
                                    ListBegin                   ( void )    { return m_Attributes.begin (); }
    std::list < CXMLAttribute* > ::iterator
                                    ListEnd                     ( void )    { return m_Attributes.end (); }

private:
    void                            CreateAttributes            ( void );
    void                            DeleteAttributes            ( void );

    TiXmlElement&                   m_Node;

    std::list < CXMLAttribute* >    m_Attributes;
    bool                            m_bCanRemoveFromList;
};

#endif
