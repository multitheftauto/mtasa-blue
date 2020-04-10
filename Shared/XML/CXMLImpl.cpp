/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLImpl.cpp
 *  PURPOSE:     XML handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

CXMLImpl::CXMLImpl(void)
{
    // Init array stuff
    CXMLArray::Initialize();
}

CXMLFile* CXMLImpl::CreateXML(const std::string& szFilename, bool bUseIDs, bool bReadOnly)
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

CXMLFile* CXMLImpl::CopyXML(const std::string& strFilename, CXMLNode* pNode, bool bReadOnly)
{
    return new CXMLFileImpl(strFilename, pNode, bReadOnly);
}

CXMLNode* CXMLImpl::CreateDummyNode()
{
    pugi::xml_node node;
    node.set_name("dummy_storage");
    return new CXMLNodeImpl(node, false);
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
    return nullptr;
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
    return nullptr;
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
    return nullptr;
}

CXMLNode* CXMLImpl::ParseString(const char* strXmlContent)
{
    // TODO
    return nullptr;
}

CXMLNode* CXMLImpl::BuildNode(CXMLNodeImpl* xmlParent, pugi::xml_node* xmlNode)
{
    // TODO
    return nullptr;
}
