/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLFileImpl.cpp
 *  PURPOSE:     XML file class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
SString CXMLFileImpl::ms_strSaveFlagFile;

CXMLFileImpl::CXMLFileImpl(const char* szFilename, bool bUseIDs, bool bReadOnly) : m_ulID(INVALID_XML_ID), m_bUsingIDs(bUseIDs), m_bReadOnly(bReadOnly)
{
    // Init
    m_pDocument = NULL;
    m_pRootNode = NULL;
    ResetLastError();

    // Create the document
    m_pDocument = new TiXmlDocument;

    // Set the filename
    if (szFilename)
    {
        m_strFilename = szFilename;
    }

    // Add to array over XML stuff
    if (m_bUsingIDs)
        m_ulID = CXMLArray::PopUniqueID(this);
}

CXMLFileImpl::~CXMLFileImpl()
{
    // Remove from array over XML stuff
    if (m_bUsingIDs)
        CXMLArray::PushUniqueID(this);

    // Delete our wrappers
    ClearWrapperTree();

    // Delete the document and the builder
    delete m_pDocument;
}

const char* CXMLFileImpl::GetFilename()
{
    return m_strFilename.c_str();
}

void CXMLFileImpl::SetFilename(const char* szFilename)
{
    // Valid?
    if (szFilename)
        m_strFilename = szFilename;
    else
        m_strFilename = "";
}

bool CXMLFileImpl::Parse(std::vector<char>* pOutFileContents)
{
    // Do we have a filename?
    if (m_strFilename != "" && FileExists(m_strFilename))
    {
        // Reset previous file
        Reset();

        // Parse from the current file
        FILE* file;
        if (m_pDocument->LoadFile(m_strFilename.c_str(), TIXML_DEFAULT_ENCODING, &file))
        {
            // Also read the file bytes to a buffer if requested
            if (pOutFileContents)
            {
                fseek(file, 0, SEEK_END);
                long size = ftell(file);
                fseek(file, 0, SEEK_SET);
                if (size > 0)
                {
                    pOutFileContents->resize(size);
                    fread(&pOutFileContents->at(0), 1, size, file);
                }
            }
            fclose(file);

            // Build our wrapper
            if (BuildWrapperTree())
            {
                ResetLastError();
                return true;
            }
            else
            {
                SetLastError(CXMLErrorCodes::OtherError, "Out of Elements");
                return false;
            }
        }

        SString strErrorDesc;
        if (m_pDocument->Error())
            strErrorDesc = SString("Line %d: %s", m_pDocument->ErrorRow(), m_pDocument->ErrorDesc());
        else
            strErrorDesc = "Invalid file";
        SetLastError(CXMLErrorCodes::OtherError, strErrorDesc);
        return false;
    }

    // No filename specified
    SetLastError(CXMLErrorCodes::NoFileSpecified, "No file specified");
    return false;
}

bool CXMLFileImpl::Write()
{
    if (m_bReadOnly)
        return false;

    // We have a filename?
    if (m_strFilename != "")
    {
        // Try a safe method of saving first
        if (WriteSafer())
        {
            return true;
        }
        if (m_pDocument->SaveFile(m_strFilename.c_str()))
        {
            return true;
        }
    }

    return false;
}

bool CXMLFileImpl::WriteSafer()
{
    // We have a filename?
    if (m_strFilename != "")
    {
        SString strFilename = m_strFilename;
        SString strTemp = strFilename + "_new_";
        SString strBackup = strFilename + "_old_";

        // Save to temp
        if (!m_pDocument->SaveFile(strTemp))
        {
            SetLastError(CXMLErrorCodes::OtherError, "Could not save temporary file");
            return false;
        }

        // Delete any leftover backup
        File::Delete(strBackup);

        // Save filename being saved
        FileRecoveryPreSave(strFilename);

        // Rename current to backup
        File::Rename(strFilename, strBackup);

        // Rename temp to current
        if (File::Rename(strTemp, strFilename))
        {
            SetLastError(CXMLErrorCodes::OtherError, "Could not rename temporary to current");
            return false;
        }

        // Unsave filename being saved
        FileRecoveryPostSave();

        // Delete backup
        File::Delete(strBackup);

        return true;
    }

    return false;
}

void CXMLFileImpl::Clear()
{
    if (m_pRootNode)
    {
        delete m_pRootNode;
        m_pRootNode = NULL;
    }
}

void CXMLFileImpl::Reset()
{
    // Clear our wrapper tree
    ClearWrapperTree();

    // Delete our document and recreate it
    delete m_pDocument;
    m_pDocument = new TiXmlDocument;
}

CXMLNode* CXMLFileImpl::CreateRootNode(const std::string& strTagName)
{
    // Make sure we always have a root node
    if (!m_pRootNode)
    {
        // Grab the document's root, create it if neccessary
        TiXmlElement* pRootNode = m_pDocument->RootElement();
        if (!pRootNode)
        {
            pRootNode = new TiXmlElement(strTagName);
            m_pDocument->LinkEndChild(pRootNode);
        }

        m_pRootNode = new CXMLNodeImpl(this, NULL, *pRootNode);
    }

    // We have a root node now. Make sure ith as the correct name.
    m_pRootNode->SetTagName(strTagName);
    return m_pRootNode;
}

CXMLNode* CXMLFileImpl::GetRootNode()
{
    // Return it
    return m_pRootNode;
}

CXMLErrorCodes::Code CXMLFileImpl::GetLastError(std::string& strOut)
{
    // Copy out the last error string and return the last error
    strOut = m_strLastError;
    return m_errLastError;
}

void CXMLFileImpl::ResetLastError()
{
    // Set the code and the string
    m_errLastError = CXMLErrorCodes::NoError;
    m_strLastError = "";
}

void CXMLFileImpl::SetLastError(CXMLErrorCodes::Code errCode, const std::string& strDescription)
{
    // Set the code and the string
    m_errLastError = errCode;
    m_strLastError = strDescription;
}

TiXmlDocument* CXMLFileImpl::GetDocument()
{
    return m_pDocument;
}

bool CXMLFileImpl::BuildWrapperTree()
{
    // Clear the previous tree
    ClearWrapperTree();

    // Grab the root element
    TiXmlElement* pRootNode = m_pDocument->RootElement();
    if (pRootNode)
    {
        // Create an XML node for it
        m_pRootNode = new CXMLNodeImpl(this, NULL, *pRootNode);

        // And build all sub-nodes
        if (!BuildSubElements(m_pRootNode))
        {
            Reset();
            return false;
        }
        return true;
    }
    return false;
}

bool CXMLFileImpl::BuildSubElements(CXMLNodeImpl* pNode)
{
    // Grab the node
    TiXmlElement* pRawNode = pNode->GetNode();
    if (pRawNode)
    {
        // Iterate the children
        TiXmlNode*    pChild = NULL;
        TiXmlElement* pElement;
        while ((pChild = pRawNode->IterateChildren(pChild)))
        {
            // If it's not a comment or something else, build it to our tree
            // TODO: Support comments
            if ((pElement = pChild->ToElement()))
            {
                // Create the child and build its subnodes again
                CXMLNodeImpl* pTempNode = new CXMLNodeImpl(this, pNode, *pElement);
                if (pTempNode->IsValid())
                {
                    if (!BuildSubElements(pTempNode))
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

void CXMLFileImpl::ClearWrapperTree()
{
    // Delete the previous wrapper tree
    if (m_pRootNode)
    {
        m_pRootNode->DeleteWrapper();
        m_pRootNode = NULL;
    }
}

//
// Initialize and do any file recovery as necessary
//
void CXMLFileImpl::InitFileRecovery(const char* szSaveFlagDirectory)
{
    if (!szSaveFlagDirectory)
        return;
    ms_strSaveFlagFile = PathJoin(szSaveFlagDirectory, "_xml_save.info");

    // Check if recover is required
    SString strFilename;
    FileLoad(ms_strSaveFlagFile, strFilename);
    if (strFilename.empty())
        return;

    if (!FileExists(strFilename))
    {
        // Try to recover from new file
        SString strTemp = strFilename + "_new_";
        if (FileExists(strTemp))
        {
            File::Rename(strTemp, strFilename);
        }
    }

    if (!FileExists(strFilename))
    {
        // Try to recover from old file
        SString strBackup = strFilename + "_old_";
        if (FileExists(strBackup))
        {
            File::Rename(strBackup, strFilename);
        }
    }
    FileDelete(ms_strSaveFlagFile);
}

// Store filename in case of problems during save
void CXMLFileImpl::FileRecoveryPreSave(const SString& strFilename)
{
    if (!ms_strSaveFlagFile.empty())
        FileSave(ms_strSaveFlagFile, strFilename);
}

// Unstore filename in case of problems during save
void CXMLFileImpl::FileRecoveryPostSave()
{
    if (!ms_strSaveFlagFile.empty())
        FileDelete(ms_strSaveFlagFile);
}
