/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLImpl.cpp
 *  PURPOSE:     XML handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
    else
    {
        delete xmlFile;
        return NULL;
    }
}

void CXMLImpl::DeleteXML(CXMLFile* pFile)
{
    delete pFile;
}

CXMLNode* CXMLImpl::ParseString(const char* strXmlContent)
{
    TiXmlElement    *xmlDocumentRoot;
    CXMLNodeImpl    *xmlBaseNode;
    CXMLNode        *xmlRootNode;
    
    TiXmlDocument*  xmlDoc = new TiXmlDocument();

    if (xmlDoc)
    {   
        if (xmlDoc->Parse(strXmlContent, 0, TIXML_ENCODING_UTF8))
        {
            xmlDocumentRoot = xmlDoc->RootElement();

            xmlBaseNode = new CXMLNodeImpl(NULL, NULL, *xmlDocumentRoot);

            xmlRootNode = CXMLImpl::BuildNode(xmlBaseNode, xmlDocumentRoot);

            return xmlRootNode;
        }
        else
            return NULL;
    }
    else
    {
        return NULL;
    }
}

CXMLNode* CXMLImpl::BuildNode(CXMLNodeImpl* xmlParent, TiXmlNode* xmlNode)
{
    TiXmlNode       *xmlChild;
    TiXmlElement    *xmlChildElement;
    CXMLNodeImpl    *xmlChildNode;

    xmlChild = 0;
    while (xmlChild = xmlNode->IterateChildren(xmlChild))
    {
        xmlChildElement = xmlChild->ToElement();

        xmlChildNode = new CXMLNodeImpl(NULL, xmlParent, *xmlChildElement);

        CXMLImpl::BuildNode(xmlChildNode, xmlChildElement);
    }

    return xmlParent;
}

CXMLNode* CXMLImpl::CreateDummyNode()
{
    CXMLNode* xmlNode = new CXMLNodeImpl(NULL, NULL, *new TiXmlElement("dummy_storage"));
    if (xmlNode->IsValid())
        return xmlNode;
    else
    {
        delete xmlNode;
        return NULL;
    }
}

CXMLAttribute* CXMLImpl::GetAttrFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_ATTR)
    {
        return reinterpret_cast<CXMLAttribute*>(pCommon);
    }

    // Doesn't exist or bad type
    return NULL;
}

CXMLFile* CXMLImpl::GetFileFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_FILE)
    {
        return reinterpret_cast<CXMLFile*>(pCommon);
    }

    // Doesn't exist or bad type
    return NULL;
}

CXMLNode* CXMLImpl::GetNodeFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_NODE)
    {
        return reinterpret_cast<CXMLNode*>(pCommon);
    }

    // Doesn't exist or bad type
    return NULL;
}
