/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLFileImpl.cpp
*  PURPOSE:     XML file class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CXMLFileImpl::CXMLFileImpl ( const char* szFilename, bool bUseIDs ) :
    m_ulID ( INVALID_XML_ID ),
    m_bUsingIDs ( bUseIDs )
{
    // Init
    m_pDocument = NULL;
    m_pRootNode = NULL;
    ResetLastError ();

    // Create the document
    m_pDocument = new TiXmlDocument;

    // Set the filename
    if ( szFilename )
    {
        m_strFilename = szFilename;
    }

    // Add to array over XML stuff
    if ( m_bUsingIDs )
        m_ulID = CXMLArray::PopUniqueID ( this );
}


CXMLFileImpl::~CXMLFileImpl ( void )
{
    // Remove from array over XML stuff
    if ( m_bUsingIDs )
        CXMLArray::PushUniqueID ( this );

    // Delete our wrappers
    ClearWrapperTree ();

    // Delete the document and the builder
    delete m_pDocument;
}


const char* CXMLFileImpl::GetFilename ( void )
{
    return m_strFilename.c_str ();
}


void CXMLFileImpl::SetFilename ( const char* szFilename )
{
    // Valid?
    if ( szFilename )
        m_strFilename = szFilename;
    else
        m_strFilename = "";
}


bool CXMLFileImpl::Parse ( std::vector < char >* pOutFileContents )
{
    // Do we have a filename?
    if ( m_strFilename != "" )
    {
        // Reset previous file
        Reset ();

        // Parse from the current file
        if ( m_pDocument->LoadFile ( m_strFilename.c_str (), TIXML_DEFAULT_ENCODING, pOutFileContents ) )
        {
            // Build our wrapper
            if ( BuildWrapperTree () )
            {
                ResetLastError ();
                return true;
            }
            else
            {
                SetLastError ( CXMLErrorCodes::OtherError, "Out of Elements" );
                return false;
            }
        }

        // Bad XML file
        SetLastError ( CXMLErrorCodes::OtherError, "Invalid file" );
        return false;
    }

    // No filename specified
    SetLastError ( CXMLErrorCodes::NoFileSpecified, "No file specified" );
    return false;
}


bool CXMLFileImpl::Write ( void )
{
    // We have a filename?
    if ( m_strFilename != "" )
    {
        // Try a safe method of saving first
        if ( WriteSafer () )
        {
            return true;
        }
        if ( m_pDocument->SaveFile ( m_strFilename.c_str () ) )
        {
            return true;
        }
    }

    return false;
}


bool CXMLFileImpl::WriteSafer ( void )
{
    // We have a filename?
    if ( m_strFilename != "" )
    {
        SString strFilename = m_strFilename;
        SString strTemp     = strFilename + "_new_";
        SString strBackup   = strFilename + "_old_";

        // Save to temp
        if ( !m_pDocument->SaveFile ( strTemp ) )
        {
            SetLastError ( CXMLErrorCodes::OtherError, "Could not save temporary file" );
            return false;
        }

        // Delete any leftover backup
        unlink ( strBackup );

        // Rename current to backup
        rename ( strFilename, strBackup );

        // Rename temp to current
        if ( rename ( strTemp, strFilename ) )
        {
            SetLastError ( CXMLErrorCodes::OtherError, "Could not rename temporary to current" );
            return false;
        }

        // Delete backup
        unlink ( strBackup );

        return true;
    }

    return false;
}



void CXMLFileImpl::Clear ( void )
{
    if ( m_pRootNode )
    {
        delete m_pRootNode;
        m_pRootNode = NULL;
    }
}


void CXMLFileImpl::Reset ( void )
{
    // Clear our wrapper tree
    ClearWrapperTree ();

    // Delete our document and recreate it
    delete m_pDocument;
    m_pDocument = new TiXmlDocument;
}


CXMLNode* CXMLFileImpl::CreateRootNode ( const std::string& strTagName )
{
    // Make sure we always have a root node
    if ( !m_pRootNode )
    {
        // Grab the document's root, create it if neccessary
        TiXmlElement* pRootNode = m_pDocument->RootElement ();
        if ( !pRootNode )
        {
            pRootNode = new TiXmlElement ( strTagName );
            m_pDocument->LinkEndChild ( pRootNode );
        }

        m_pRootNode = new CXMLNodeImpl ( this, NULL, *pRootNode );
    }

    // We have a root node now. Make sure ith as the correct name.
    m_pRootNode->SetTagName ( strTagName );
    return m_pRootNode;
}


CXMLNode* CXMLFileImpl::GetRootNode ( void )
{
    // Return it
    return m_pRootNode;
}


CXMLErrorCodes::Code CXMLFileImpl::GetLastError ( std::string& strOut )
{
    // Copy out the last error string and return the last error
    strOut = m_strLastError;
    return m_errLastError;
}


void CXMLFileImpl::ResetLastError ( void )
{
    // Set the code and the string
    m_errLastError = CXMLErrorCodes::NoError;
    m_strLastError = "";
}


void CXMLFileImpl::SetLastError ( CXMLErrorCodes::Code errCode, const std::string& strDescription )
{
    // Set the code and the string
    m_errLastError = errCode;
    m_strLastError = strDescription;
}


TiXmlDocument* CXMLFileImpl::GetDocument ( void )
{
    return m_pDocument;
}


bool CXMLFileImpl::BuildWrapperTree ( void )
{
    // Clear the previous tree
    ClearWrapperTree ();

    // Grab the root element
    TiXmlElement* pRootNode = m_pDocument->RootElement ();
    if ( pRootNode )
    {
        // Create an XML node for it
        m_pRootNode = new CXMLNodeImpl ( this, NULL, *pRootNode );

        // And build all sub-nodes
        if ( !BuildSubElements ( m_pRootNode ) )
        {
            Reset ( );
            return false;
        }
        return true;
    }
    return false;
}


bool CXMLFileImpl::BuildSubElements ( CXMLNodeImpl* pNode )
{
    // Grab the node
    TiXmlElement* pRawNode = pNode->GetNode ();
    if ( pRawNode )
    {
        // Iterate the children
        TiXmlNode* pChild = NULL;
        TiXmlElement* pElement;
        while ( ( pChild = pRawNode->IterateChildren ( pChild ) ) )
        {
            // If it's not a comment or something else, build it to our tree
            // TODO: Support comments
            if ( ( pElement = pChild->ToElement () ) )
            {
                // Create the child and build its subnodes again
                CXMLNodeImpl* pTempNode = new CXMLNodeImpl ( this, pNode, *pElement );
                if ( pTempNode->IsValid ( ) )
                {
                    if ( !BuildSubElements ( pTempNode ) )
                    {
                        delete pTempNode;
                        return false;
                    }
                }
                else
                {
                    delete pTempNode;
                    return false;
                }
            }
        }
    }
    return true;
}


void CXMLFileImpl::ClearWrapperTree ( void )
{
    // Delete the previous wrapper tree
    if ( m_pRootNode )
    {
        m_pRootNode->DeleteWrapper ();
        m_pRootNode = NULL;
    }
}
