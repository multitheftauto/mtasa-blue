/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CGlobalTranslationItem.h"
#include "CResource.h"
#include "CResourceTranslationManager.h"
#include "CScriptDebugging.h"
#include "CGame.h"

CGlobalTranslationItem::CGlobalTranslationItem(CResource* resource, const char* src, CXMLAttributes* xmlAttributes)
    : CResourceFile(resource, src, src, xmlAttributes)
{
    m_type = RESOURCE_FILE_TYPE_GLOBAL_TRANSLATION;
    m_providerResourceName = src ? src : "";
}

bool CGlobalTranslationItem::Start()
{
    if (m_providerResourceName.empty())
    {
        CScriptDebugging* scriptDebugging = g_pGame->GetScriptDebugging();
        SLuaDebugInfo debugInfo;
        debugInfo.infoType = DEBUG_INFO_NONE;
        debugInfo.strShortSrc = SString("[Resource: %s]", m_resource->GetName().c_str());
        
        scriptDebugging->LogError(debugInfo, 
            "Global translation: Empty provider resource name in meta.xml for resource '%s'. "
            "Check your <global-translation src=\"...\"/> tag.", 
            m_resource->GetName().c_str());
        return false;
    }

    if (m_resource->GetTranslationManager())
    {
        m_resource->GetTranslationManager()->AddGlobalTranslationProvider(m_providerResourceName);
        return true;
    }
    
    CScriptDebugging* scriptDebugging = g_pGame->GetScriptDebugging();
    SLuaDebugInfo debugInfo;
    debugInfo.infoType = DEBUG_INFO_NONE;
    debugInfo.strShortSrc = SString("[Resource: %s]", m_resource->GetName().c_str());
    
    scriptDebugging->LogError(debugInfo,
        "Global translation: Translation manager not available for resource '%s' when adding provider '%s'. "
        "This is an internal error - please report this issue.",
        m_resource->GetName().c_str(), m_providerResourceName.c_str());
    return false;
}

bool CGlobalTranslationItem::Stop()
{
    return true;
}