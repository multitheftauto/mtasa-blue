/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceConfigItem.cpp
*  PURPOSE:     Resource configuration item class
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single resource config file.
// Config files can be accessed by other resources and used to
// configure them. They are always XML files, and access
// is provided by giving out the root xml node.

#include <StdInc.h>

CResourceConfigItem::CResourceConfigItem ( CResource * resource, const char * szShortName, const char * szResourceFileName, CChecksum serverChecksum ) :
    CDownloadableResource ( CDownloadableResource::RESOURCE_FILE_TYPE_CLIENT_CONFIG, szResourceFileName, szShortName, serverChecksum, true )
{
    m_pXMLFile = NULL;
    m_pXMLRootNode = NULL;
    m_pResource = resource;
}

CResourceConfigItem::~CResourceConfigItem ( void )
{
    if ( m_pXMLFile )
    {
        delete m_pXMLFile;
        m_pXMLFile = NULL;
    }
}

bool CResourceConfigItem::Start ( void )
{
    // Does the file even exist?
    if ( FileExists( GetName() ) )
    {
        // Already created?
        if ( m_pXMLFile )
        {
            delete m_pXMLFile;
        }

        // Create the XML
        m_pXMLFile = g_pCore->GetXML ()->CreateXML ( GetName (), true );
        if ( m_pXMLFile )
        {
            // Parse the XML
            if ( m_pXMLFile->Parse () )
            {
                m_pXMLRootNode = m_pXMLFile->GetRootNode ();
                return true;
            }
            else
            {
                CLogger::ErrorPrintf ( "Couldn't parse config %s in resource %s\n", GetShortName (), m_pResource->GetName() );
            }

            // Delete the XML
            delete m_pXMLFile;
            m_pXMLFile = NULL;
        }
        else
        {
            CLogger::ErrorPrintf ( "Couldn't load config %s in resource %s\n", GetShortName (), m_pResource->GetName() );
        }
    }
    else
    {
        CLogger::ErrorPrintf ( "Config %s in resource %s does not exist\n", GetShortName (), m_pResource->GetName() );
    }

    return false;
}

bool CResourceConfigItem::Stop ( void )
{
    if ( m_pXMLFile )
    {
        delete m_pXMLFile;
        m_pXMLFile = NULL;
    }

    return true;
}
