/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLFileImpl.h
 *  PURPOSE:     XML file class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <tinyxml.h>
#include <iostream>
#include <fstream>
#include <xml/CXMLFile.h>

class CXMLFileImpl : public CXMLFile
{
    friend class CXMLNodeImpl;

public:
    CXMLFileImpl(const char* szFilename, bool bUseIDs, bool bReadOnly);
    ~CXMLFileImpl();

    const char* GetFilename();
    void        SetFilename(const char* szFilename);

    bool Parse(std::vector<char>* pOutFileContents = NULL);
    bool Write();
    void Clear();
    void Reset();

    CXMLNode* CreateRootNode(const std::string& strTagName);
    CXMLNode* GetRootNode();

    CXMLErrorCodes::Code GetLastError(std::string& strOut);
    void                 ResetLastError();

    // Private functions
    void SetLastError(CXMLErrorCodes::Code errCode, const std::string& strDescription);

    TiXmlDocument* GetDocument();

    eXMLClass     GetClassType() { return CXML_FILE; };
    unsigned long GetID()
    {
        dassert(m_bUsingIDs);
        return m_ulID;
    };
    bool IsValid() { return !m_bUsingIDs || m_ulID != INVALID_XML_ID; };
    bool IsUsingIDs() { return m_bUsingIDs; }

    static void InitFileRecovery(const char* szSaveFlagDirectory);
    void        FileRecoveryPreSave(const SString& strFilename);
    void        FileRecoveryPostSave();

private:
    bool BuildWrapperTree();
    bool BuildSubElements(class CXMLNodeImpl* pNode);
    void ClearWrapperTree();
    bool WriteSafer();

    std::string m_strFilename;

    CXMLErrorCodes::Code m_errLastError;
    std::string          m_strLastError;

    TiXmlDocument* m_pDocument;

    class CXMLNodeImpl* m_pRootNode;
    unsigned long       m_ulID;
    const bool          m_bUsingIDs;
    const bool          m_bReadOnly;

    static SString ms_strSaveFlagFile;
};
