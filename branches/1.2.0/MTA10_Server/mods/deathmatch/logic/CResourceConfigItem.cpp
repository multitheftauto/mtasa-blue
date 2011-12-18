/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceConfigItem.cpp
*  PURPOSE:     Resource server-side (XML) configuration file item class
*  DEVELOPERS:  Ed Lyons <>
*               Kevin Whiteside <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single resource config file.
// Config files can be accessed by other resources and used to
// configure them. They are always XML files, and access
// is provided by giving out the root xml node.

#include "StdInc.h"

extern CServerInterface* g_pServerInterface;

CResourceConfigItem::CResourceConfigItem ( CResource * resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes * xmlAttributes ) : CResourceFile ( resource, szShortName, szResourceFileName, xmlAttributes )
{
    m_pXMLFile = NULL;
    m_pXMLRootNode = NULL;
    m_bInvalid = true;
    
    m_type = RESOURCE_FILE_TYPE_CONFIG;
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
    // Already started?
    if ( m_pXMLFile )
    {
        return false;
    }

    // Create the XML
    m_pXMLFile = g_pServerInterface->GetXML ()->CreateXML ( m_strResourceFileName.c_str () );
    if ( m_pXMLFile )
    {
        // Parse it
        if ( m_pXMLFile->Parse () )
        {
            m_pXMLRootNode = m_pXMLFile->GetRootNode ();
            m_bInvalid = false;
            return true;
        }
        else
        {
            CLogger::ErrorPrintf ( "Couldn't parse config %s in resource %s\n", m_strShortName.c_str (), m_resource->GetName().c_str () );
        }

        // Delete the XML again, invalid
        delete m_pXMLFile;
        m_pXMLFile = NULL;
    }
    else
        CLogger::ErrorPrintf ( "Couldn't load config %s in resource %s\n", m_strShortName.c_str (), m_resource->GetName().c_str () );

    return !m_bInvalid;
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
