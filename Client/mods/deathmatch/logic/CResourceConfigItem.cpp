/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceConfigItem.cpp
 *  PURPOSE:     Resource configuration item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class represents a single resource config file.
// Config files can be accessed by other resources and used to
// configure them. They are always XML files, and access
// is provided by giving out the root xml node.

#include <StdInc.h>

CResourceConfigItem::CResourceConfigItem(CResource* resource, const char* szShortName, const char* szResourceFileName, uint uiDownloadSize,
                                         CChecksum serverChecksum)
    : CDownloadableResource(resource, CDownloadableResource::RESOURCE_FILE_TYPE_CLIENT_CONFIG, szResourceFileName, szShortName, uiDownloadSize, serverChecksum,
                            true)
{
    m_pXMLFile = NULL;
    m_pXMLRootNode = NULL;
}

CResourceConfigItem::~CResourceConfigItem()
{
    if (m_pXMLFile)
    {
        delete m_pXMLFile;
        m_pXMLFile = NULL;
    }
}

bool CResourceConfigItem::Start()
{
    // Does the file even exist?
    if (FileExists(GetName()))
    {
        // Already created?
        if (m_pXMLFile)
        {
            delete m_pXMLFile;
        }

        // Create the XML
        m_pXMLFile = g_pCore->GetXML()->CreateXML(GetName(), true);
        if (m_pXMLFile)
        {
            // Parse the XML
            std::vector<char> fileContents;
            if (m_pXMLFile->Parse(&fileContents))
            {
                if (CChecksum::GenerateChecksumFromBuffer(&fileContents.at(0), fileContents.size()) == GetServerChecksum())
                {
                    m_pXMLRootNode = m_pXMLFile->GetRootNode();
                    return true;
                }
            }
            else
            {
                CLogger::ErrorPrintf("Couldn't parse config %s in resource %s\n", GetShortName(), m_pResource->GetName());
            }

            // Delete the XML
            delete m_pXMLFile;
            m_pXMLFile = NULL;
        }
        else
        {
            CLogger::ErrorPrintf("Couldn't load config %s in resource %s\n", GetShortName(), m_pResource->GetName());
        }
    }
    else
    {
        CLogger::ErrorPrintf("Config %s in resource %s does not exist\n", GetShortName(), m_pResource->GetName());
    }

    return false;
}

bool CResourceConfigItem::Stop()
{
    if (m_pXMLFile)
    {
        delete m_pXMLFile;
        m_pXMLFile = NULL;
        m_pXMLRootNode = NULL;
    }

    return true;
}