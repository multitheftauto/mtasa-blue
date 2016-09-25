/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLImpl.cpp
*  PURPOSE:     XML handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
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

CXMLFile *CXMLImpl::CreateXML(const std::string& strFilename, bool bUseIDs)
{
    return new CXMLFileImpl(strFilename, bUseIDs);
}

void CXMLImpl::DeleteXML(CXMLFile *pFile) 
{ 
    delete pFile; 
}

CXMLFile *CXMLImpl::CopyXML(const std::string& strFilename, CXMLNode *pNode)
{
    return new CXMLFileImpl(strFilename, pNode);
}

CXMLNode *CXMLImpl::CreateDummyNode()
{
    pugi::xml_node node;
    node.set_name("dummy_storage");
    return new CXMLNodeImpl(node, false);
}

CXMLAttribute *CXMLImpl::GetAttrFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon *pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_ATTR)
    {
        return reinterpret_cast<CXMLAttribute *>(pCommon);
    }

    // Doesn't exist or bad type
    return nullptr;
}

CXMLFile *CXMLImpl::GetFileFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon *pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_FILE)
    {
        return reinterpret_cast<CXMLFile *>(pCommon);
    }

    // Doesn't exist or bad type
    return nullptr;
}

CXMLNode *CXMLImpl::GetNodeFromID(unsigned long ulID)
{
    // Grab it and verify the type
    CXMLCommon *pCommon = CXMLArray::GetEntry(ulID);
    if (pCommon && pCommon->GetClassType() == CXML_NODE)
    {
        return reinterpret_cast<CXMLNode *>(pCommon);
    }

    // Doesn't exist or bad type
    return nullptr;
}
