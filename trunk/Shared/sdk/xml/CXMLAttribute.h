/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/xml/CXMLAttribute.h
*  PURPOSE:     XML attribute interface
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLATTRIBUTE_H
#define __CXMLATTRIBUTE_H

#include "CXMLCommon.h"
#include <string>

class CXMLAttribute: public CXMLCommon
{
public:
    virtual                     ~CXMLAttribute          ( void ) {};

    virtual const std::string   GetName                 ( void ) const = 0;
    virtual const std::string&  GetValue                ( void ) const = 0;

    virtual void                SetValue                ( const char* szValue ) = 0;
    virtual void                SetValue                ( bool bValue ) = 0;
    virtual void                SetValue                ( int iValue ) = 0;
    virtual void                SetValue                ( unsigned int uiValue ) = 0;
    virtual void                SetValue                ( float fValue ) = 0;
    
    virtual void                DeleteWrapper           ( void ) = 0;
};

#endif
