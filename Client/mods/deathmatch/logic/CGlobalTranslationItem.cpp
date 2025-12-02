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
#include "CClientGame.h"

CGlobalTranslationItem::CGlobalTranslationItem(CResource* resource, const char* src, uint uiDownloadSize, CChecksum serverChecksum)
    : CResourceFile(resource, CDownloadableResource::RESOURCE_FILE_TYPE_GLOBAL_TRANSLATION, src, src, uiDownloadSize, serverChecksum, true)
{
    m_providerResourceName = src ? src : "";
}

bool CGlobalTranslationItem::Start()
{
    if (m_providerResourceName.empty())
    {
        CScriptDebugging* scriptDebugging = g_pClientGame->GetScriptDebugging();
        SLuaDebugInfo debugInfo;
        debugInfo.infoType = DEBUG_INFO_NONE;
        debugInfo.strShortSrc = SString("[Resource: %s]", m_pResource->GetName());
        
        scriptDebugging->LogError(debugInfo, 
            "Global translation: Empty provider resource name in meta.xml. "
            "Check your <global-translation src=\"...\"/> tag.");
        return false;
    }

    if (m_pResource->GetTranslationManager())
    {
        m_pResource->GetTranslationManager()->AddGlobalTranslationProvider(m_providerResourceName);
        return true;
    }
    
    CScriptDebugging* scriptDebugging = g_pClientGame->GetScriptDebugging();
    SLuaDebugInfo debugInfo;
    debugInfo.infoType = DEBUG_INFO_NONE;
    debugInfo.strShortSrc = SString("[Resource: %s]", m_pResource->GetName());
    
    scriptDebugging->LogError(debugInfo,
        "Global translation: Translation manager not available when adding provider '%s'. "
        "This is an internal error - please report this issue.",
        m_providerResourceName.c_str());
    return false;
}

bool CGlobalTranslationItem::Stop()
{
    return true;
}