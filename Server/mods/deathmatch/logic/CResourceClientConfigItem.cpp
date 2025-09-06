/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceClientConfigItem.cpp
 *  PURPOSE:     Resource client-side (XML) configuration file item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class represents a single resource config file.
// Config files can be accessed by other resources and used to
// configure them. They are always XML files, and access
// is provided by giving out the root xml node.

#include "StdInc.h"
#include "CResourceClientConfigItem.h"

extern CServerInterface* g_pServerInterface;
extern CGame*            g_pGame;

CResourceClientConfigItem::CResourceClientConfigItem(CResource* resource, const char* szShortName, const char* szResourceFileName,
                                                     CXMLAttributes* xmlAttributes)
    : CResourceFile(resource, szShortName, szResourceFileName, xmlAttributes)
{
    m_pXMLFile = NULL;
    m_pXMLRootNode = NULL;
    m_bInvalid = false;
    m_type = RESOURCE_FILE_TYPE_CLIENT_CONFIG;
}

CResourceClientConfigItem::~CResourceClientConfigItem()
{
}

bool CResourceClientConfigItem::Start()
{
    return true;
}

bool CResourceClientConfigItem::Stop()
{
    return true;
}
