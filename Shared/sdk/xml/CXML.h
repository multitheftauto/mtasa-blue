/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/xml/CXML.h
 *  PURPOSE:     XML handler interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CXMLAttribute.h"
#include "CXMLFile.h"
#include "CXMLNode.h"

class CXML
{
public:
    virtual CXMLFile* CreateXML(const std::string& filename, bool useIDs = false, bool readOnly = false) = 0;
    virtual void      DeleteXML(CXMLFile* file) = 0;
    virtual CXMLFile* CopyXML(const std::string& filename, CXMLNode* node, bool readOnly = false) = 0;

    virtual CXMLNode* CreateDummyNode() = 0;

    virtual CXMLAttribute* GetAttrFromID(unsigned long ID) const = 0;
    virtual CXMLFile*      GetFileFromID(unsigned long ID) const = 0;
    virtual CXMLNode*      GetNodeFromID(unsigned long ID) const = 0;

    virtual CXMLNode* ParseString(const char* xmlContent) = 0;
};
