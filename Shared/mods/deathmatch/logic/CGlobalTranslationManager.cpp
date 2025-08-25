/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CGlobalTranslationManager.h"
#include "CResourceTranslationManager.h"
#include "CLogger.h"
#include "CScriptDebugging.h"
#ifdef MTA_CLIENT
    #include "CClientGame.h"
#else
    #include "CGame.h"
#endif
#include <algorithm>
#include <set>
#include <thread>

CGlobalTranslationManager& CGlobalTranslationManager::GetSingleton()
{
    static CGlobalTranslationManager instance;
    return instance;
}

bool CGlobalTranslationManager::RegisterProvider(const std::string& resourceName, CResourceTranslationManager* translationManager)
{
    if (resourceName.empty() || !translationManager)
        return false;

    std::lock_guard<std::mutex> lock(m_providerMutex);
    
    if (m_providers.find(resourceName) != m_providers.end())
    {
        LogWarning("Global translation provider '" + resourceName + "' is already registered, replacing with new instance. "
                   "This might indicate a resource restart or duplicate provider registration.");
    }
    
    m_providers[resourceName] = translationManager;
    return true;
}

void CGlobalTranslationManager::UnregisterProvider(const std::string& resourceName)
{
    if (resourceName.empty())
        return;

    std::lock_guard<std::mutex> lock(m_providerMutex);
    
    auto it = m_providers.find(resourceName);
    if (it != m_providers.end())
    {
        m_providers.erase(it);
    }
}

std::string CGlobalTranslationManager::GetGlobalTranslation(const std::vector<std::string>& providers, const std::string& msgid, const std::string& language) const
{
    if (msgid.empty() || providers.empty())
        return msgid;

    thread_local std::set<std::string> visitedProviders;
    
    std::lock_guard<std::mutex> lock(m_providerMutex);
    
    for (const auto& providerName : providers)
    {
        if (visitedProviders.find(providerName) != visitedProviders.end())
            continue;
            
        auto it = m_providers.find(providerName);
        if (it != m_providers.end() && it->second)
        {
            visitedProviders.insert(providerName);
            
            struct VisitGuard {
                std::set<std::string>& visited;
                std::string providerName;
                VisitGuard(std::set<std::string>& v, const std::string& name) : visited(v), providerName(name) {}
                ~VisitGuard() { visited.erase(providerName); }
            } guard(visitedProviders, providerName);
            
            std::string translation = it->second->GetLocalTranslation(msgid, language);
            if (!translation.empty() && translation != msgid)
                return translation;
        }
    }
    
    return msgid;
}

std::vector<std::string> CGlobalTranslationManager::GetAvailableProviders() const
{
    std::lock_guard<std::mutex> lock(m_providerMutex);
    
    std::vector<std::string> providers;
    providers.reserve(m_providers.size());
    
    for (const auto& pair : m_providers)
    {
        providers.push_back(pair.first);
    }
    
    std::sort(providers.begin(), providers.end());
    return providers;
}

bool CGlobalTranslationManager::IsProviderAvailable(const std::string& resourceName) const
{
    if (resourceName.empty())
        return false;

    std::lock_guard<std::mutex> lock(m_providerMutex);
    return m_providers.find(resourceName) != m_providers.end();
}

void CGlobalTranslationManager::Clear()
{
    std::lock_guard<std::mutex> lock(m_providerMutex);
    m_providers.clear();
}

void CGlobalTranslationManager::LogWarning(const std::string& message) const
{
#ifdef MTA_CLIENT
    CScriptDebugging* pScriptDebugging = g_pClientGame->GetScriptDebugging();
#else
    CScriptDebugging* pScriptDebugging = g_pGame->GetScriptDebugging();
#endif
    
    SLuaDebugInfo debugInfo;
    debugInfo.infoType = DEBUG_INFO_NONE;
    debugInfo.strShortSrc = "[GlobalTranslation]";
    
    pScriptDebugging->LogWarning(debugInfo, "%s", message.c_str());
}