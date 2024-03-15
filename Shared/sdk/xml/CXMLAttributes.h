/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/xml/CXMLAttributes.h
 *  PURPOSE:     XML attributes container interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <list>

class CXMLAttribute;

class CXMLAttributes
{
public:
    virtual std::uint32_t   Count() = 0;
    virtual CXMLAttribute* Find(const char* szName) = 0;
    virtual CXMLAttribute* Get(std::uint32_t uiIndex) = 0;

    virtual CXMLAttribute* Create(const char* szName) = 0;
    virtual CXMLAttribute* Create(const CXMLAttribute& Copy) = 0;
    virtual bool           Delete(const char* szName) = 0;
    virtual void           DeleteAll() = 0;

    virtual std::list<CXMLAttribute*>::iterator ListBegin() = 0;
    virtual std::list<CXMLAttribute*>::iterator ListEnd() = 0;
};
