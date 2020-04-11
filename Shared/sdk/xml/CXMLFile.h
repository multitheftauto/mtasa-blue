/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/xml/CXMLFile.h
 *  PURPOSE:     XML file interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CXMLCommon.h"
#include "CXMLErrorCodes.h"
#include "CXMLNode.h"
#include <string>
#include <vector>

class CXMLFile : public CXMLCommon
{
public:
    virtual ~CXMLFile() = default;

    virtual const std::string& GetFilename() const = 0;
    virtual void               SetFilename(const std::string& filename) = 0;

    virtual bool Parse(std::vector<char>* outFileContents = nullptr) = 0;
    virtual bool Write() = 0;
    virtual void Reset() = 0;

    virtual CXMLNode* CreateRootNode(const std::string& tagName) = 0;
    virtual CXMLNode* GetRootNode() const = 0;

    virtual CXMLErrorCodes::Code GetLastError(std::string& out) = 0;

    virtual bool IsValid() const = 0;
};
