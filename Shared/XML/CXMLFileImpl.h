/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

#include "CXMLNodeImpl.h"
#include <xml/CXMLErrorCodes.h>
#include <xml/CXMLFile.h>

class CXMLFileImpl : public CXMLFile
{
public:
    CXMLFileImpl(const std::string &strFilename, bool bUseIDs);
    CXMLFileImpl(const std::string &strFilename, CXMLNode *pNode);
    ~CXMLFileImpl();

    const std::string& GetFilename()                 override { return m_strFilename; }
    void SetFilename(const std::string& strFilename) override { m_strFilename = strFilename; } 

    bool Parse() override;
    bool Write() override;
    void Reset() override;

    class CXMLNode *CreateRootNode(const std::string &strTagName) override;
    class CXMLNode *GetRootNode   () override;

    CXMLErrorCodes::Code GetLastError(std::string &strOut) override;

    eXMLClass GetClassType() override { return CXML_FILE; }
    unsigned long GetID()    override { return m_ulID; }

private:
    void BuildWrapperTree(bool bUsingIDs);
    std::unique_ptr<CXMLNodeImpl> WrapperTreeWalker(pugi::xml_node* node, bool bUsingIDs);

private:
    std::unique_ptr<class CXMLNodeImpl> m_pRoot;
    std::unique_ptr<pugi::xml_document> m_pDocument;
    pugi::xml_parse_result  m_parserResult;
    std::string             m_strFilename;
    unsigned long           m_ulID;
};
