/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceScriptItem.cpp
 *  PURPOSE:     Resource server-side script item class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class represents a single resource script item, being a .lua file
// It's task is to load the script into the VM. It can't be unloaded without
// unloading all the script items, as a script cannot be removed from
// the VM once its been added

#include "StdInc.h"

CResourceScriptItem::CResourceScriptItem(CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes,
                                         eScriptLanguage scriptLanguage)
    : CResourceFile(resource, szShortName, szResourceFileName, xmlAttributes)
{
    m_type = RESOURCE_FILE_TYPE_SCRIPT;
    m_language = scriptLanguage;
}

CResourceScriptItem::~CResourceScriptItem()
{
}

bool CResourceScriptItem::Start()
{
    std::vector<char> buffer;
    FileLoad(m_strResourceFileName, buffer);
    unsigned int iSize = buffer.size();

    m_pVM = m_resource->GetVirtualMachine();
    if (iSize > 0)
    {
        CV8Base*    v8 = g_pServerInterface->GetV8();
        switch (m_language)
        {
            case eScriptLanguage::LUA:
                m_pVM->LoadScriptFromBuffer(&buffer.at(0), iSize, m_strResourceFileName.c_str());
                break;
            case eScriptLanguage::JAVASCRIPT:
                v8->CreateIsolate(std::string(buffer.begin(), buffer.end()), m_strResourceFileName);
                break;
            case eScriptLanguage::JAVASCRIPT_MODULE:
                v8->CreateIsolate(std::string(buffer.begin(), buffer.end()), m_strResourceFileName, true);
                break;
        }
    }

    return true;
}

bool CResourceScriptItem::Stop()
{
    return true;
}
