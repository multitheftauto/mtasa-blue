/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceScriptItem.cpp
 *  PURPOSE:     Resource server-side script item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class represents a single resource script item, being a .lua file
// It's task is to load the script into the VM. It can't be unloaded without
// unloading all the script items, as a script cannot be removed from
// the VM once its been added

#include "StdInc.h"
#include "CResourceScriptItem.h"
#include "CResource.h"
#include "lua/CLuaMain.h"

CResourceScriptItem::CResourceScriptItem(CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes)
    : CResourceFile(resource, szShortName, szResourceFileName, xmlAttributes)
{
    m_type = RESOURCE_FILE_TYPE_SCRIPT;
}

CResourceScriptItem::~CResourceScriptItem()
{
}

bool CResourceScriptItem::Start()
{
    m_pVM = m_resource->GetVirtualMachine();

    // Load the file
    std::vector<char> buffer;
    FileLoad(m_strResourceFileName, buffer);
    unsigned int iSize = buffer.size();

    if (iSize > 0)
    {
        m_pVM->LoadScriptFromBuffer(&buffer.at(0), iSize, m_strResourceFileName.c_str());
    }

    return true;
}

bool CResourceScriptItem::Stop()
{
    return true;
}
