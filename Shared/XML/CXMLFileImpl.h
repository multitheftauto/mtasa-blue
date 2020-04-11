/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLFileImpl.h
 *  PURPOSE:     XML file class
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
    CXMLFileImpl(const std::string& strFilename, bool bUseIDs = false, bool bReadOnly = false);
    CXMLFileImpl(const std::string& strFilename, CXMLNode* pNode, bool bReadOnly = false);
    ~CXMLFileImpl();

    inline const std::string& GetFilename() override { return m_strFilename; }
    inline void               SetFilename(const std::string& strFilename) override { m_strFilename = strFilename; }

    bool Parse(std::vector<char>* pOutFileContents = nullptr) override;
    bool Write() override;
    void Reset() override;

    class CXMLNode* CreateRootNode(const std::string& strTagName) override;
    class CXMLNode* GetRootNode() override;

    CXMLErrorCodes::Code GetLastError(std::string& strOut) override;

    inline eXMLClass     GetClassType() const override { return eXMLClass::CXML_FILE; }
    inline unsigned long GetID() const override { return m_ID; }

    inline bool IsValid() { return !m_bUsingIDs || m_ulID != INVALID_XML_ID; };

private:
    void                          BuildWrapperTree(bool bUsingIDs);
    std::unique_ptr<CXMLNodeImpl> WrapperTreeWalker(pugi::xml_node* node, bool bUsingIDs);

    std::unique_ptr<class CXMLNodeImpl> m_pRoot;
    std::unique_ptr<pugi::xml_document> m_pDocument;
    pugi::xml_parse_result              m_parserResult;
    std::string                         m_strFilename;
    unsigned long                       m_ulID;
    const bool                          m_bUsingIDs = false;
    const bool                          m_bReadOnly = false;
};
