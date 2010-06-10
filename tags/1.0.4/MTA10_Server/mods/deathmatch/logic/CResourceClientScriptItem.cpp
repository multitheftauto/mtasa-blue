/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceClientScriptItem.cpp
*  PURPOSE:     Resource client-side script item class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single resource script item, being a .lua file
// It's task is to load and unload the script client side

#include "StdInc.h"

extern CGame* g_pGame;

CResourceClientScriptItem::CResourceClientScriptItem ( CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes ) : CResourceFile ( resource, szShortName, szResourceFileName, xmlAttributes )
{
    m_type = RESOURCE_FILE_TYPE_CLIENT_SCRIPT;
}

CResourceClientScriptItem::~CResourceClientScriptItem ( void )
{

}

bool CResourceClientScriptItem::Start ( void )
{
    return true;
}

bool CResourceClientScriptItem::Stop ( void )
{
    return true;
}
