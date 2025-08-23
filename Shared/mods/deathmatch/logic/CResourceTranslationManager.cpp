/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CResourceTranslationManager.h"
#include "dictionary_manager.hpp"
#include "language.hpp"
#include "po_parser.hpp"
#include "CLogger.h"
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <set>

CResourceTranslationManager::CResourceTranslationManager(const std::string& resourceName, const std::string& charset)
    : m_resourceName(resourceName)
{
}

CResourceTranslationManager::~CResourceTranslationManager()
{
    Clear();
}

bool CResourceTranslationManager::LoadTranslation(const std::string& filePath, bool isPrimary)
{
    std::string language = ExtractLanguageFromPath(filePath);
    if (language.empty())
    {
        LogError("Could not determine language from file path: " + filePath);
        return false;
    }

    if (!std::filesystem::exists(filePath))
    {
        LogError("Translation file not found: " + filePath);
        return false;
    }

    tinygettext::Language lang = tinygettext::Language::from_name(language);
    if (!lang)
    {
        LogError("Invalid language code '" + language + "' extracted from: " + filePath);
        return false;
    }

    try {
        // Create a new separate dictionary for each language instead of reusing the same one
        auto dictionary = std::make_unique<tinygettext::Dictionary>();
        
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open())
        {
            LogError("Could not open translation file: " + filePath);
            return false;
        }
        
        tinygettext::POParser::parse(filePath, file, *dictionary);
        
        m_translationFiles[language] = filePath;
        m_dictionaries[language] = dictionary.release(); // Transfer ownership
        
        if (isPrimary)
        {
            m_primaryLanguage = language;
        }
        else if (m_translationFiles.size() == 1)
        {
            if (m_primaryLanguage.empty())
            {
                m_primaryLanguage = language;
            }
        }

        return true;
    }
    catch (const std::exception& e)
    {
        LogError("Exception loading translation file '" + filePath + "': " + e.what());
        return false;
    }
}

std::string CResourceTranslationManager::GetTranslation(const std::string& msgid, const std::string& language) const
{
    if (msgid.empty())
        return msgid;

    std::string targetLanguage = language.empty() ? m_primaryLanguage : language;
    
    if (targetLanguage.empty())
        return msgid;
    
    if (m_translationFiles.find(targetLanguage) == m_translationFiles.end())
    {
        if (!m_primaryLanguage.empty() && targetLanguage != m_primaryLanguage && 
            m_translationFiles.find(m_primaryLanguage) != m_translationFiles.end())
        {
            targetLanguage = m_primaryLanguage;
        }
        else
        {
            return msgid;
        }
    }

    auto dictIt = m_dictionaries.find(targetLanguage);
    if (dictIt == m_dictionaries.end() || !dictIt->second)
        return msgid;
        
    std::string translation = dictIt->second->translate(msgid);
    return translation.empty() ? msgid : translation;
}

std::vector<std::string> CResourceTranslationManager::GetAvailableLanguages() const
{
    std::vector<std::string> languages;
    languages.reserve(m_translationFiles.size());
    
    for (const auto& pair : m_translationFiles)
    {
        languages.push_back(pair.first);
    }
    
    std::sort(languages.begin(), languages.end());
    
    return languages;
}

void CResourceTranslationManager::SetPlayerLanguage(void* player, const std::string& language)
{
    if (!player || language.empty())
        return;

    if (m_translationFiles.find(language) != m_translationFiles.end())
    {
        m_playerLanguages[player] = language;
    }
    else
    {
        LogWarning("Language '" + language + "' not available for player, using primary language '" + m_primaryLanguage + "'");
        if (!m_primaryLanguage.empty())
            m_playerLanguages[player] = m_primaryLanguage;
    }
}

std::string CResourceTranslationManager::GetPlayerLanguage(void* player) const
{
    if (!player)
    {
        if (!m_primaryLanguage.empty())
            return m_primaryLanguage;
        // If no primary language, return first available language
        if (!m_translationFiles.empty())
            return m_translationFiles.begin()->first;
        return "";
    }

    auto it = m_playerLanguages.find(player);
    if (it != m_playerLanguages.end())
        return it->second;

    // Fallback to primary language or first available language
    if (!m_primaryLanguage.empty())
        return m_primaryLanguage;
    if (!m_translationFiles.empty())
        return m_translationFiles.begin()->first;
    return "";
}

void CResourceTranslationManager::RemovePlayer(void* player)
{
    if (player)
        m_playerLanguages.erase(player);
}

void CResourceTranslationManager::SetClientLanguage(const std::string& language)
{
    if (language.empty())
    {
        LogWarning("SetClientLanguage called with empty language");
        return;
    }

    LogWarning("SetClientLanguage called with: '" + language + "'");
    LogWarning("Available languages: " + std::to_string(m_translationFiles.size()));
    for (const auto& pair : m_translationFiles)
    {
        LogWarning("  - " + pair.first);
    }

    if (m_translationFiles.find(language) != m_translationFiles.end())
    {
        m_clientLanguage = language;
        LogWarning("Client language set to: '" + m_clientLanguage + "'");
    }
    else 
    {
        LogWarning("Language '" + language + "' not found, falling back to primary: '" + m_primaryLanguage + "'");
        if (!m_primaryLanguage.empty())
        {
            m_clientLanguage = m_primaryLanguage;
        }
        else
        {
            LogWarning("No primary language set, keeping current: '" + m_clientLanguage + "'");
        }
    }
}

std::string CResourceTranslationManager::GetClientLanguage() const
{
    if (!m_clientLanguage.empty())
        return m_clientLanguage;
    
    if (!m_primaryLanguage.empty())
        return m_primaryLanguage;
    
    // If no primary language, return first available language
    if (!m_translationFiles.empty())
        return m_translationFiles.begin()->first;
    
    return "";
}

void CResourceTranslationManager::Clear()
{
    m_translationFiles.clear();
    
    // Clean up owned dictionaries
    for (auto& pair : m_dictionaries)
    {
        delete pair.second;
    }
    m_dictionaries.clear();
    
    m_primaryLanguage.clear();
    m_clientLanguage.clear();
    m_playerLanguages.clear();
}

std::string CResourceTranslationManager::ExtractLanguageFromPath(const std::string& filePath) const
{
    std::filesystem::path path(filePath);
    return path.stem().string();
}

void CResourceTranslationManager::LogWarning(const std::string& message) const
{
    CLogger::LogPrintf("[%s] %s\n", m_resourceName.c_str(), message.c_str());
}

void CResourceTranslationManager::LogError(const std::string& message) const
{
    CLogger::ErrorPrintf("[%s] %s\n", m_resourceName.c_str(), message.c_str());
}
