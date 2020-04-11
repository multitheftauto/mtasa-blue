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
    CXMLFileImpl(const std::string& filename, bool useIDs = false, bool readOnly = false);
    CXMLFileImpl(const std::string& filename, CXMLNode* node, bool readOnly = false);
    ~CXMLFileImpl();

    inline const std::string& GetFilename() const override { return m_filename; }
    inline void               SetFilename(const std::string& filename) override { m_filename = filename; }

    bool Parse(std::vector<char>* outFileContents = nullptr) override;
    bool Write() override;
    void Reset() override;

    class CXMLNode* CreateRootNode(const std::string& tagName) override;
    class CXMLNode* GetRootNode() const override;

    CXMLErrorCodes::Code GetLastError(std::string& out) override;

    inline eXMLClass     GetClassType() const override { return eXMLClass::CXML_FILE; }
    inline unsigned long GetID() const override { return m_ID; }

    inline bool IsValid() const { return !m_usingIDs || m_ID != INVALID_XML_ID; };

private:
    void                          BuildWrapperTree(bool usingIDs);
    std::unique_ptr<CXMLNodeImpl> WrapperTreeWalker(pugi::xml_node* node, bool usingIDs);

    std::unique_ptr<class CXMLNodeImpl> m_root;
    std::unique_ptr<pugi::xml_document> m_document;
    pugi::xml_parse_result              m_parserResult;
    std::string                         m_filename;
    unsigned long                       m_ID;
    const bool                          m_usingIDs = false;
    const bool                          m_readOnly = false;
};
