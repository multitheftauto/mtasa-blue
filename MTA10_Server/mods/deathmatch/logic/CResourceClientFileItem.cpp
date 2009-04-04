/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceClientFileItem.cpp
*  PURPOSE:     Resource client-side file item class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single resource script item, being a .lua file
// It's task is to load and unload the script client side

#include "StdInc.h"

extern CGame* g_pGame;

CResourceClientFileItem::CResourceClientFileItem ( CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes ) : CResourceFile ( resource, szShortName, szResourceFileName, xmlAttributes )
{
    m_type = RESOURCE_FILE_TYPE_CLIENT_FILE;
}

CResourceClientFileItem::~CResourceClientFileItem ( void )
{

}

bool CResourceClientFileItem::Start ( void )
{
    // start somehow
    return true;
}

bool CResourceClientFileItem::Stop ( void )
{
    return true;
}
