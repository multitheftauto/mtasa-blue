/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLImpl.cpp
 *  PURPOSE:     XML handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#define ALLOC_STATS_MODULE_NAME "xml"
#include <sys/stat.h>
#ifndef MAX_PATH
    #define MAX_PATH 260
#endif
#include "SharedUtil.hpp"

CXMLImpl::CXMLImpl()
{
    // Init array stuff
    CXMLArray::Initialize();
}

CXMLImpl::~CXMLImpl()
{
}

CXMLFile* CXMLImpl::CreateXML(const char* szFilename, bool bUseIDs, bool bReadOnly)
{
    CXMLFile* xmlFile = new CXMLFileImpl(szFilename, bUseIDs, bReadOnly);
    if (xmlFile->IsValid())
        return xmlFile;
    delete xmlFile;
    return nullptr;
}

void CXMLImpl::DeleteXML(CXMLFile* pFile)
{
    delete pFile;
}

std::unique_ptr<SXMLString> CXMLImpl::ParseString(const char* strXmlContent)
{
    tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
    if (xmlDoc)
    {
        xmlDoc->Parse(strXmlContent);
        if (!xmlDoc->Error())
        {
            tinyxml2::XMLElement* xmlDocumentRoot = xmlDoc->RootElement();

            if (xmlDocumentRoot)
            {
                CXMLNodeImpl* xmlBaseNode = new CXMLNodeImpl(nullptr, nullptr, *xmlDocumentRoot);

                if (xmlBaseNode && xmlBaseNode->IsValid())
                {
                    xmlBaseNode->BuildFromDocument();
                    return std::unique_ptr<SXMLString>(new SXMLStringImpl(xmlDoc, xmlBaseNode));
                }
            }
        }
    }
    return nullptr;
}

CXMLNode* CXMLImpl::CreateDummyNode()
{
    // Create a document to own the dummy element. CXMLNodeImpl will take
    // ownership of the document and clean it up on destruction.
    auto*         doc = new tinyxml2::XMLDocument();
    auto*         elem = doc->NewElement("dummy_storage");
    CXMLNodeImpl* xmlNode = new CXMLNodeImpl(nullptr, nullptr, *elem);
    xmlNode->m_standaloneDocument.reset(doc);
    if (xmlNode->IsValid())
        return xmlNode;
    delete xmlNode;
    return nullptr;
}

CXMLAttribute* CXMLImpl::GetAttrFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_ATTR)
        return reinterpret_cast<CXMLAttribute*>(pCommon);

    // Doesn't exist or bad type
    return nullptr;
}

CXMLFile* CXMLImpl::GetFileFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_FILE)
        return reinterpret_cast<CXMLFile*>(pCommon);

    // Doesn't exist or bad type
    return nullptr;
}

CXMLNode* CXMLImpl::GetNodeFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_NODE)
        return reinterpret_cast<CXMLNode*>(pCommon);

    // Doesn't exist or bad type
    return nullptr;
}
