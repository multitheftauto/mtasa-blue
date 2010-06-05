/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/xml/CXMLFile.h
*  PURPOSE:     XML file interface
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLFILE_H
#define __CXMLFILE_H

#include "CXMLCommon.h"
#include "CXMLErrorCodes.h"

#include <string>

class CXMLNode;

class CXMLFile: public CXMLCommon
{
public:
    virtual                         ~CXMLFile           ( void ) {};

    virtual const char*             GetFilename         ( void ) = 0;
    virtual void                    SetFilename         ( const char* szFilename ) = 0;

    virtual bool                    Parse               ( void ) = 0;
    virtual bool                    Write               ( void ) = 0;
    virtual void                    Clear               ( void ) = 0;
    virtual void                    Reset               ( void ) = 0;

    virtual CXMLNode*               CreateRootNode      ( const std::string& strTagName ) = 0;
    virtual CXMLNode*               GetRootNode         ( void ) = 0;

    virtual CXMLErrorCodes::Code    GetLastError        ( std::string& strOut ) = 0;
    virtual void                    ResetLastError      ( void ) = 0;
};

#endif
