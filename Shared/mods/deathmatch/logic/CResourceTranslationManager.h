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

private:
    std::string ExtractLanguageFromPath(const std::string& filePath) const;
    void LogWarning(const std::string& message) const;
    void LogError(const std::string& message) const;

private:
    std::string m_primaryLanguage;
    std::string m_clientLanguage;
    std::map<std::string, std::string> m_translationFiles;
    std::map<std::string, tinygettext::Dictionary*> m_dictionaries;
    std::string m_resourceName;
    
    std::unordered_map<void*, std::string> m_playerLanguages;
};
