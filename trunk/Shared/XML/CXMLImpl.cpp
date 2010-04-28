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

CXMLImpl::CXMLImpl ( void )
{
    // Init array stuff
    CXMLArray::Initialize ();
}


CXMLImpl::~CXMLImpl ( void )
{

}


CXMLFile* CXMLImpl::CreateXML ( const char* szFilename )
{
    CXMLFile* xmlFile = new CXMLFileImpl ( szFilename );
    if ( xmlFile->IsValid( ) )
        return xmlFile;
    else
    {
        delete xmlFile;
        return NULL;
    }
}


void CXMLImpl::DeleteXML ( CXMLFile* pFile )
{
    delete pFile;
}


CXMLNode* CXMLImpl::CreateDummyNode ( void )
{
    CXMLNode* xmlNode = new CXMLNodeImpl ( NULL, NULL, *new TiXmlElement ( "dummy_storage" ) );
    if ( xmlNode->IsValid( ) )
        return xmlNode;
    else
    {
        delete xmlNode;
        return NULL;
    }
}


CXMLAttribute* CXMLImpl::GetAttrFromID ( unsigned long ulID )
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry ( ulID );
    if ( pCommon && pCommon->GetClassType () == CXML_ATTR )
    {
        return reinterpret_cast < CXMLAttribute* > ( pCommon );
    }

    // Doesn't exist or bad type
    return NULL;
}


CXMLFile* CXMLImpl::GetFileFromID ( unsigned long ulID )
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry ( ulID );
    if ( pCommon && pCommon->GetClassType () == CXML_FILE )
    {
        return reinterpret_cast < CXMLFile* > ( pCommon );
    }

    // Doesn't exist or bad type
    return NULL;
}


CXMLNode* CXMLImpl::GetNodeFromID ( unsigned long ulID )
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry ( ulID );
    if ( pCommon && pCommon->GetClassType () == CXML_NODE )
    {
        return reinterpret_cast < CXMLNode* > ( pCommon );
    }

    // Doesn't exist or bad type
    return NULL;
}
