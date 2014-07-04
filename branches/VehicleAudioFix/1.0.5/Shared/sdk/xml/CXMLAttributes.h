/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/xml/CXMLAttributes.h
*  PURPOSE:     XML attributes container interface
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLATTRIBUTES_H
#define __CXMLATTRIBUTES_H

#include <list>
#include "SharedUtil.h"

class CXMLAttribute;

class CXMLAttributes
{
public:
    virtual unsigned int                Count           ( void ) = 0;
    virtual CXMLAttribute*              Find            ( const char* szName ) = 0;
    virtual CXMLAttribute*              Get             ( unsigned int uiIndex ) = 0;

    virtual CXMLAttribute*              Create          ( const char* szName ) = 0;
    virtual CXMLAttribute*              Create          ( const CXMLAttribute& Copy ) = 0;
    virtual void                        DeleteAll       ( void ) = 0;

    virtual std::list < CXMLAttribute* >::iterator
                                        ListBegin ( void ) = 0;
    virtual std::list < CXMLAttribute* >::iterator
                                        ListEnd   ( void ) = 0;
};

#endif
