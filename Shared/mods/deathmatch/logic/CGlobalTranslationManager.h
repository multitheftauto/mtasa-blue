/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

class CResourceTranslationManager;

class CGlobalTranslationManager
{
public:
    static CGlobalTranslationManager& GetSingleton();
    
    bool RegisterProvider(const std::string& resourceName, CResourceTranslationManager* translationManager);
    void UnregisterProvider(const std::string& resourceName);
    
    std::string GetGlobalTranslation(const std::vector<std::string>& providers, const std::string& msgid, const std::string& language) const;
    std::vector<std::string> GetAvailableProviders() const;
    bool IsProviderAvailable(const std::string& resourceName) const;
    
    void Clear();

private:
    CGlobalTranslationManager() = default;
    ~CGlobalTranslationManager() = default;
    CGlobalTranslationManager(const CGlobalTranslationManager&) = delete;
    CGlobalTranslationManager& operator=(const CGlobalTranslationManager&) = delete;
    
    void LogWarning(const std::string& message) const;

private:
    std::unordered_map<std::string, CResourceTranslationManager*> m_providers;
    mutable std::mutex m_providerMutex;
};