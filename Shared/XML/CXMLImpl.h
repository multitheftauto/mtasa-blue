/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

#include <xml/CXML.h>

class CXMLImpl : public CXML
{
public:
    CXMLImpl();
    
    CXMLFile *      CreateXML(const std::string& strFilename, bool bUseIDs) override;
    void            DeleteXML(CXMLFile *pFile) override;
    CXMLFile *      CopyXML  (const std::string& strFilename, CXMLNode *pNode) override;

    CXMLNode *      CreateDummyNode() override;

    CXMLAttribute * GetAttrFromID(unsigned long ulID) override;
    CXMLFile *      GetFileFromID(unsigned long ulID) override;
    CXMLNode *      GetNodeFromID(unsigned long ulID) override;
};
