/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLAttributesImpl.h
 *  PURPOSE:     XML attributes container class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <tinyxml.h>
#include <xml/CXMLAttributes.h>

#include <list>

class CXMLAttributesImpl : public CXMLAttributes
{
public:
    CXMLAttributesImpl(TiXmlElement& Node, bool bUseIDs);
    ~CXMLAttributesImpl();

    unsigned int         Count();
    class CXMLAttribute* Find(const char* szName);
    class CXMLAttribute* Get(unsigned int uiIndex);

    class CXMLAttribute* Create(const char* szName);
    class CXMLAttribute* Create(const class CXMLAttribute& Copy);
    bool                 Delete(const char* szName);
    void                 DeleteAll();

    TiXmlElement& GetNode();

    class CXMLAttribute* AddToList(class CXMLAttribute* pAttribute);
    void                 RemoveFromList(class CXMLAttribute* pAttribute);

    std::list<CXMLAttribute*>::iterator ListBegin() { return m_Attributes.begin(); }
    std::list<CXMLAttribute*>::iterator ListEnd() { return m_Attributes.end(); }

    bool IsUsingIDs() const { return m_bUsingIDs; }

private:
    void CreateAttributes();
    void DeleteAttributes();

    const bool    m_bUsingIDs;
    bool          m_bCanRemoveFromList;
    TiXmlElement& m_Node;

    std::list<CXMLAttribute*> m_Attributes;
};
