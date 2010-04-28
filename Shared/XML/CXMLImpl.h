/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLImpl.h
*  PURPOSE:     XML handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLIMPL_H
#define __CXMLIMPL_H

#include <xml/CXML.h>

class CXMLImpl : public CXML
{
public:
                        CXMLImpl            ( void );
                        ~CXMLImpl           ( void );

    CXMLFile*           CreateXML           ( const char* szFilename );
    void                DeleteXML           ( CXMLFile* pFile );

    CXMLNode*           CreateDummyNode     ( void );

    CXMLAttribute*      GetAttrFromID       ( unsigned long ulID );
    CXMLFile*           GetFileFromID       ( unsigned long ulID );
    CXMLNode*           GetNodeFromID       ( unsigned long ulID );
};

#endif
