/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLFileImpl.h
*  PURPOSE:     XML file class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLFILEIMPL_H
#define __CXMLFILEIMPL_H

#include <tinyxml.h>
#include <iostream>
#include <fstream>
#include <xml/CXMLFile.h>

class CXMLFileImpl : public CXMLFile
{
    friend class CXMLNodeImpl;

public:
                                    CXMLFileImpl        ( const char* szFilename );
                                    ~CXMLFileImpl       ( void );

    const char*                     GetFilename         ( void );
    void                            SetFilename         ( const char* szFilename );

    bool                            Parse               ( void );
    bool                            Write               ( void );
    void                            Clear               ( void );
    void                            Reset               ( void );

    CXMLNode*                       CreateRootNode      ( const std::string& strTagName );
    CXMLNode*                       GetRootNode         ( void );

    CXMLErrorCodes::Code            GetLastError        ( std::string& strOut );
    void                            ResetLastError      ( void );

    // Private functions
    void                            SetLastError        ( CXMLErrorCodes::Code errCode, const std::string& strDescription );

	TiXmlDocument*                  GetDocument         ( void );

    eXMLClass                       GetClassType        ( void )    { return CXML_FILE; };
    unsigned long                   GetID               ( void )    { return m_ulID; };
    bool                            IsValid             ( void )    { return m_ulID != INVALID_XML_ID; };

private:
    void                            BuildWrapperTree    ( void );
    void                            BuildSubElements    ( class CXMLNodeImpl* pNode );
    void                            ClearWrapperTree    ( void );

    std::string                     m_strFilename;

    CXMLErrorCodes::Code            m_errLastError;
    std::string                     m_strLastError;

	TiXmlDocument*                  m_pDocument;

    class CXMLNodeImpl*             m_pRootNode;
    unsigned long                   m_ulID;
};

#endif
