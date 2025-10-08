/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceClientFileItem.cpp
 *  PURPOSE:     Resource client-side file item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class represents a single resource script item, being a .lua file
// It's task is to load and unload the script client side

#include "StdInc.h"
#include "CResourceClientFileItem.h"

extern CGame* g_pGame;

CResourceClientFileItem::CResourceClientFileItem(CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes,
                                                 bool bClientAutoDownload)
    : CResourceFile(resource, szShortName, szResourceFileName, xmlAttributes)
{
    m_type = RESOURCE_FILE_TYPE_CLIENT_FILE;
    m_bClientAutoDownload = bClientAutoDownload;
}

CResourceClientFileItem::~CResourceClientFileItem()
{
}

bool CResourceClientFileItem::Start()
{
    // start somehow
    return true;
}

bool CResourceClientFileItem::Stop()
{
    return true;
}
