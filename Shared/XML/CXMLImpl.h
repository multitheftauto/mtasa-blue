/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLImpl.h
 *  PURPOSE:     XML handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <tinyxml2.h>
#include <xml/CXML.h>

typedef struct SXMLStringImpl : SXMLString
{
    tinyxml2::XMLDocument* doc;
    SXMLStringImpl(tinyxml2::XMLDocument* d, CXMLNode* n) : doc(d) { node = n; };
    ~SXMLStringImpl()
    {
        delete node;
        delete doc;
    };
} SXMLStringImpl;

class CXMLImpl : public CXML
{
public:
    CXMLImpl();
    virtual ~CXMLImpl();

    CXMLFile*                   CreateXML(const char* szFilename, bool bUseIDs, bool bReadOnly);
    std::unique_ptr<SXMLString> ParseString(const char* strXmlContent);
    void                        DeleteXML(CXMLFile* pFile);

    CXMLNode* CreateDummyNode();

    CXMLAttribute* GetAttrFromID(unsigned long ulID);
    CXMLFile*      GetFileFromID(unsigned long ulID);
    CXMLNode*      GetNodeFromID(unsigned long ulID);
};
