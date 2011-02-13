/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceScriptItem.cpp
*  PURPOSE:     Resource server-side script item class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single resource script item, being a .lua file
// It's task is to load the script into the VM. It can't be unloaded without
// unloading all the script items, as a script cannot be removed from
// the VM once its been added

#include "StdInc.h"

CResourceScriptItem::CResourceScriptItem ( CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes ) : CResourceFile ( resource, szShortName, szResourceFileName, xmlAttributes )
{
    m_type = RESOURCE_FILE_TYPE_SCRIPT;
}

CResourceScriptItem::~CResourceScriptItem ( void )
{

}

bool CResourceScriptItem::Start ( void )
{
    m_pVM = m_resource->GetVirtualMachine();

    // Load the file
    std::vector < char > buffer;
    FileLoad ( m_strResourceFileName, buffer );

    //UTF-8 BOM?  Compare by checking the standard UTF-8 BOM of 3 characters (in signed format, hence negative)
    if ( buffer[0] != -0x11 || buffer[1] != -0x45 || buffer[2] != -0x41 ) //Not UTF-8
        // Load the resource text
        m_pVM->LoadScriptFromBuffer ( &buffer.at ( 0 ), buffer.size (), m_strResourceFileName.c_str() );
    else // Load ignoring the first 3 bytes
        m_pVM->LoadScriptFromBuffer ( &buffer.at ( 3 ), buffer.size ()-3, m_strResourceFileName.c_str() );

    return true;
}

bool CResourceScriptItem::Stop ( void )
{
    return true;
}
