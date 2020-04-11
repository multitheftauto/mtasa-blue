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

    CXMLFile* CreateXML(const std::string& filename, bool useIDs = false, bool readOnly = false) override;
    void      DeleteXML(CXMLFile* file) override;
    CXMLFile* CopyXML(const std::string& filename, CXMLNode* node, bool readOnly = false) override;

    CXMLNode* CreateDummyNode() override;

    CXMLAttribute* GetAttrFromID(unsigned long ID) const override;
    CXMLFile*      GetFileFromID(unsigned long ID) const override;
    CXMLNode*      GetNodeFromID(unsigned long ID) const override;

    CXMLNode* ParseString(const char* xmlContent) override;
    CXMLNode* BuildNode(CXMLNodeImpl* xmlParent, pugi::xml_node* xmlNode);
};
