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
#include <map>
#include <memory>
#include <unordered_map>
#include "../../../vendor/tinygettext/dictionary.hpp"

namespace tinygettext
{
    class DictionaryManager;
}

class CResourceTranslationManager
{
public:
    explicit CResourceTranslationManager(const std::string& resourceName, const std::string& charset = "UTF-8");
    ~CResourceTranslationManager();

    bool LoadTranslation(const std::string& filePath, bool isPrimary = false);
    std::string GetTranslation(const std::string& msgid, const std::string& language = "") const;
    void SetPlayerLanguage(void* player, const std::string& language);
    std::string GetPlayerLanguage(void* player) const;
    void RemovePlayer(void* player);
    
    void SetClientLanguage(const std::string& language);
    std::string GetClientLanguage() const;
    std::string GetPrimaryLanguage() const noexcept { return m_primaryLanguage; }
    std::vector<std::string> GetAvailableLanguages() const;
    void Clear();
    
    bool HasTranslations() const noexcept { return !m_translationFiles.empty(); }
    
    // Global translation support
    void AddGlobalTranslationProvider(const std::string& providerResourceName);
    std::vector<std::string> GetGlobalProviders() const noexcept { return m_globalProviders; }
    void RemoveAllGlobalProviders() noexcept;
    bool IsGlobalProvider() const noexcept { return m_isGlobalProvider; }
    void SetAsGlobalProvider(bool isProvider = true) noexcept { m_isGlobalProvider = isProvider; }
    
    // Enhanced lookup with global fallback
    std::string GetLocalTranslation(const std::string& msgid, const std::string& language) const;
    std::string GetTranslationWithGlobalFallback(const std::string& msgid, const std::string& language) const;
    
    // Error handling
    std::string GetLastError() const noexcept { return m_lastError; }

private:
    std::string ExtractLanguageFromPath(const std::string& filePath) const;
    std::string ValidateLanguageWithCore(const std::string& language) const;
    bool ValidatePoFile(const std::string& filePath);
    
    // TinyGetText logging callbacks
    static std::string ConformTranslationPath(const std::string& message);
    static void TinyGetTextErrorCallback(const std::string& message);
    static void TinyGetTextWarningCallback(const std::string& message);
    static void SetupTinyGetTextLogging();
    static void CleanupTinyGetTextLogging();

private:
    std::string m_primaryLanguage;
    std::string m_clientLanguage;
    std::map<std::string, std::string> m_translationFiles;
    std::map<std::string, tinygettext::Dictionary*> m_dictionaries;
    std::string m_resourceName;
    
    std::unordered_map<void*, std::string> m_playerLanguages;
    
    // Global translation support
    bool m_isGlobalProvider = false;
    std::vector<std::string> m_globalProviders;
    
    // Error handling
    mutable std::string m_lastError;
};
