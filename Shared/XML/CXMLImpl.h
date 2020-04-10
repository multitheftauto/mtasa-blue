/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLImpl.h
 *  PURPOSE:     XML handler class
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

    CXMLFile* CreateXML(const std::string& strFilename, bool bUseIDs = false, bool bReadOnly = false) override;
    void      DeleteXML(CXMLFile* pFile) override;
    CXMLFile* CopyXML(const std::string& strFilename, CXMLNode* pNode, bool bReadOnly = false) override;

    CXMLNode* CreateDummyNode() override;

    CXMLAttribute* GetAttrFromID(unsigned long ulID) override;
    CXMLFile*      GetFileFromID(unsigned long ulID) override;
    CXMLNode*      GetNodeFromID(unsigned long ulID) override;

    CXMLNode* ParseString(const char* strXmlContent) override;
    CXMLNode* BuildNode(CXMLNodeImpl* xmlParent, pugi::xml_node* xmlNode);
};
