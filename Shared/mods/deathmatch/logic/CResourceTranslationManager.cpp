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
#include "CGlobalTranslationManager.h"
#include "dictionary_manager.hpp"
#include "language.hpp"
#include "po_parser.hpp"
#include "CLogger.h"
#include "CScriptDebugging.h"
#include "../../../vendor/tinygettext/log.hpp"
#include <thread>
#include <mutex>
#ifdef MTA_CLIENT
    #include "CClientGame.h"
    #include "CResourceManager.h"
    #include "CResource.h"
#else
    #include "CGame.h"
    #include "CResourceManager.h"
    #include "CResource.h"
#endif
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <set>

static bool s_tinyGetTextLoggingInitialized = false;
static std::mutex s_loggingMutex;

CResourceTranslationManager::CResourceTranslationManager(const std::string& resourceName, const std::string& charset)
    : m_resourceName(resourceName)
{
    std::lock_guard<std::mutex> lock(s_loggingMutex);
    if (!s_tinyGetTextLoggingInitialized)
    {
        SetupTinyGetTextLogging();
        s_tinyGetTextLoggingInitialized = true;
    }
}

CResourceTranslationManager::~CResourceTranslationManager()
{
    Clear();
}

bool CResourceTranslationManager::ValidatePoFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return false;
    
    std::streampos fileSize = file.tellg();
    if (fileSize == 0)
        return false;
    
    file.seekg(-1, std::ios::end);
    char lastChar;
    file.read(&lastChar, 1);
    
    if (lastChar != '\n')
        return false;
    
    return true;
}

bool CResourceTranslationManager::LoadTranslation(const std::string& filePath, bool isPrimary)
{
    m_lastError.clear();
    
    std::string extractedLanguage = ExtractLanguageFromPath(filePath);
    if (extractedLanguage.empty())
    {
        m_lastError = "Could not determine language from file path: " + filePath;
        return false;
    }

    std::string validatedLanguage = ValidateLanguageWithCore(extractedLanguage);
    if (validatedLanguage.empty())
    {
        m_lastError = "Invalid language code '" + extractedLanguage + "'";
        return false;
    }
    
    std::string language = validatedLanguage;

    if (!std::filesystem::exists(filePath))
    {
        m_lastError = "Translation file not found: " + filePath;
        return false;
    }

    if (!ValidatePoFile(filePath))
        return false;

    try {
        auto dictionary = std::make_unique<tinygettext::Dictionary>();
        
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open())
        {
            m_lastError = "Could not open translation file: " + filePath;
            return false;
        }
        
        tinygettext::POParser::parse(filePath, file, *dictionary);
        
        m_translationFiles[language] = filePath;
        m_dictionaries[language] = dictionary.release();
        
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
        m_lastError = "Exception loading translation file '" + filePath + "': " + e.what();
        return false;
    }
}

std::string CResourceTranslationManager::GetTranslation(const std::string& msgid, const std::string& language) const
{
    if (msgid.empty())
        return msgid;

    if (!m_globalProviders.empty())
        return GetTranslationWithGlobalFallback(msgid, language);
    
    return GetLocalTranslation(msgid, language);
}

std::string CResourceTranslationManager::GetLocalTranslation(const std::string& msgid, const std::string& language) const
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
        
    std::string translation = dictIt->second->translate_silent(msgid);
    return translation.empty() ? msgid : translation;
}

std::string CResourceTranslationManager::GetTranslationWithGlobalFallback(const std::string& msgid, const std::string& language) const
{
    thread_local std::set<std::string> currentResourceStack;
    
    if (currentResourceStack.find(m_resourceName) != currentResourceStack.end())
        return msgid;
    
    currentResourceStack.insert(m_resourceName);
    
    struct StackGuard {
        std::set<std::string>& stack;
        std::string resourceName;
        StackGuard(std::set<std::string>& s, const std::string& r) : stack(s), resourceName(r) {}
        ~StackGuard() { stack.erase(resourceName); }
    } guard(currentResourceStack, m_resourceName);
    
    std::string localTranslation = GetLocalTranslation(msgid, language);
    if (!localTranslation.empty() && localTranslation != msgid)
        return localTranslation;
    
    std::string globalTranslation = CGlobalTranslationManager::GetSingleton()
        .GetGlobalTranslation(m_globalProviders, msgid, language);
    if (!globalTranslation.empty() && globalTranslation != msgid)
        return globalTranslation;
    
    if (!language.empty() && language != m_primaryLanguage)
    {
        std::string primaryTranslation = GetLocalTranslation(msgid, m_primaryLanguage);
        if (!primaryTranslation.empty() && primaryTranslation != msgid)
            return primaryTranslation;
    }
    
    return msgid;
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

    std::string validatedLanguage = ValidateLanguageWithCore(language);
    if (validatedLanguage.empty())
    {
#ifdef MTA_CLIENT
        CScriptDebugging* pScriptDebugging = g_pClientGame->GetScriptDebugging();
#else
        CScriptDebugging* pScriptDebugging = g_pGame->GetScriptDebugging();
#endif
        SLuaDebugInfo debugInfo;
        debugInfo.infoType = DEBUG_INFO_NONE;
        debugInfo.strShortSrc = SString("[Resource: %s]", m_resourceName.c_str());
        
        pScriptDebugging->LogError(debugInfo, "Invalid language '%s' - use standard locale format (e.g., en_US, es_ES)", 
                                  language.c_str());
        return;
    }

    if (m_translationFiles.find(validatedLanguage) != m_translationFiles.end())
    {
        m_playerLanguages[player] = validatedLanguage;
    }
    else
    {
#ifdef MTA_CLIENT
        CScriptDebugging* pScriptDebugging = g_pClientGame->GetScriptDebugging();
#else
        CScriptDebugging* pScriptDebugging = g_pGame->GetScriptDebugging();
#endif
        SLuaDebugInfo debugInfo;
        debugInfo.infoType = DEBUG_INFO_NONE;
        debugInfo.strShortSrc = SString("[Resource: %s]", m_resourceName.c_str());
        
        pScriptDebugging->LogWarning(debugInfo, "Language '%s' not available for player, using primary language '%s'", 
                                    validatedLanguage.c_str(), m_primaryLanguage.c_str());
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
        if (!m_translationFiles.empty())
            return m_translationFiles.begin()->first;
        return "";
    }

    auto it = m_playerLanguages.find(player);
    if (it != m_playerLanguages.end())
        return it->second;

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
#ifdef MTA_CLIENT
        CScriptDebugging* pScriptDebugging = g_pClientGame->GetScriptDebugging();
#else
        CScriptDebugging* pScriptDebugging = g_pGame->GetScriptDebugging();
#endif
        SLuaDebugInfo debugInfo;
        debugInfo.infoType = DEBUG_INFO_NONE;
        debugInfo.strShortSrc = SString("[Resource: %s]", m_resourceName.c_str());
        
        pScriptDebugging->LogWarning(debugInfo, "setCurrentTranslationLanguage called with empty language code");
        return;
    }

    std::string validatedLanguage = ValidateLanguageWithCore(language);
    if (validatedLanguage.empty())
    {
#ifdef MTA_CLIENT
        CScriptDebugging* pScriptDebugging = g_pClientGame->GetScriptDebugging();
#else
        CScriptDebugging* pScriptDebugging = g_pGame->GetScriptDebugging();
#endif
        SLuaDebugInfo debugInfo;
        debugInfo.infoType = DEBUG_INFO_NONE;
        debugInfo.strShortSrc = SString("[Resource: %s]", m_resourceName.c_str());
        
        pScriptDebugging->LogError(debugInfo, "Invalid language '%s' - use standard locale format (e.g., en_US, es_ES)", 
                                  language.c_str());
        return;
    }

    if (m_translationFiles.find(validatedLanguage) != m_translationFiles.end())
    {
        m_clientLanguage = validatedLanguage;
    }
    else 
    {
#ifdef MTA_CLIENT
        CScriptDebugging* pScriptDebugging = g_pClientGame->GetScriptDebugging();
#else
        CScriptDebugging* pScriptDebugging = g_pGame->GetScriptDebugging();
#endif
        SLuaDebugInfo debugInfo;
        debugInfo.infoType = DEBUG_INFO_NONE;
        debugInfo.strShortSrc = SString("[Resource: %s]", m_resourceName.c_str());
        
        pScriptDebugging->LogWarning(debugInfo, "Language '%s' not available, falling back to primary language '%s'", 
                                    validatedLanguage.c_str(), m_primaryLanguage.c_str());
        if (!m_primaryLanguage.empty())
        {
            m_clientLanguage = m_primaryLanguage;
        }
    }
}

std::string CResourceTranslationManager::GetClientLanguage() const
{
    if (!m_clientLanguage.empty())
        return m_clientLanguage;
    
    if (!m_primaryLanguage.empty())
        return m_primaryLanguage;
    
    if (!m_translationFiles.empty())
        return m_translationFiles.begin()->first;
    
    return "";
}

void CResourceTranslationManager::Clear()
{
    m_translationFiles.clear();
    
    for (auto& pair : m_dictionaries)
        delete pair.second;
    
    m_dictionaries.clear();
    m_primaryLanguage.clear();
    m_clientLanguage.clear();
    m_playerLanguages.clear();
    
    RemoveAllGlobalProviders();
}

void CResourceTranslationManager::AddGlobalTranslationProvider(const std::string& providerResourceName)
{
    if (providerResourceName.empty())
        return;
    
    auto it = std::find(m_globalProviders.begin(), m_globalProviders.end(), providerResourceName);
    if (it != m_globalProviders.end())
        return;
    
    m_globalProviders.push_back(providerResourceName);
}

void CResourceTranslationManager::RemoveAllGlobalProviders() noexcept
{
    m_globalProviders.clear();
}

std::string CResourceTranslationManager::ExtractLanguageFromPath(const std::string& filePath) const
{
    std::filesystem::path path(filePath);
    return path.stem().string();
}

std::string CResourceTranslationManager::ValidateLanguageWithCore(const std::string& language) const
{
    if (language.empty())
        return "";
    
    std::string normalizedLanguage = language;
    
    if (normalizedLanguage == "en")
        normalizedLanguage = "en_US";
    else if (normalizedLanguage == "fi")
        normalizedLanguage = "fi_FI";
    else if (normalizedLanguage == "az")
        normalizedLanguage = "az_AZ";
    else if (normalizedLanguage == "ka")
        normalizedLanguage = "ka_GE";
    
    tinygettext::Language lang = tinygettext::Language::from_name(normalizedLanguage);
    if (!lang)
    {
        lang = tinygettext::Language::from_name("en_US");
        if (!lang)
        {
            return "";
        }
        
        return "";
    }
    
    std::string result = lang.str();
    
    return result;
}


std::string CResourceTranslationManager::ConformTranslationPath(const std::string& message)
{
    std::string result = message;
    
    size_t resourcesPos = result.find("/resources/");
    if (resourcesPos == std::string::npos)
    {
        resourcesPos = result.find("\\resources\\");
        if (resourcesPos == std::string::npos)
            return result;
    }
    
    size_t startPos = resourcesPos + 11;
    if (startPos >= result.length())
        return result;
    
    std::string resourceRelativePath = result.substr(startPos);
    
    for (char& c : resourceRelativePath)
    {
        if (c == '\\')
            c = '/';
    }
    
    return resourceRelativePath;
}

void CResourceTranslationManager::TinyGetTextErrorCallback(const std::string& message)
{
#ifdef MTA_CLIENT
    CScriptDebugging* pScriptDebugging = g_pClientGame->GetScriptDebugging();
#else
    CScriptDebugging* pScriptDebugging = g_pGame->GetScriptDebugging();
#endif
    
    SLuaDebugInfo debugInfo;
    debugInfo.infoType = DEBUG_INFO_NONE;
    debugInfo.strShortSrc = "[TinyGetText]";
    
    std::string cleanMessage = message;
    if (!cleanMessage.empty() && cleanMessage.back() == '\n')
        cleanMessage.pop_back();
    
    cleanMessage = ConformTranslationPath(cleanMessage);
    
    pScriptDebugging->LogError(debugInfo, "%s", cleanMessage.c_str());
}

void CResourceTranslationManager::TinyGetTextWarningCallback(const std::string& message)
{
#ifdef MTA_CLIENT
    CScriptDebugging* pScriptDebugging = g_pClientGame->GetScriptDebugging();
#else
    CScriptDebugging* pScriptDebugging = g_pGame->GetScriptDebugging();
#endif
    
    SLuaDebugInfo debugInfo;
    debugInfo.infoType = DEBUG_INFO_NONE;
    debugInfo.strShortSrc = "[TinyGetText]";
    
    std::string cleanMessage = message;
    if (!cleanMessage.empty() && cleanMessage.back() == '\n')
        cleanMessage.pop_back();
    
    cleanMessage = ConformTranslationPath(cleanMessage);
    
    pScriptDebugging->LogWarning(debugInfo, "%s", cleanMessage.c_str());
}

void CResourceTranslationManager::SetupTinyGetTextLogging()
{
    tinygettext::Log::set_log_info_callback(nullptr);
    tinygettext::Log::set_log_warning_callback(TinyGetTextWarningCallback);
    tinygettext::Log::set_log_error_callback(TinyGetTextErrorCallback);
}

void CResourceTranslationManager::CleanupTinyGetTextLogging()
{
    tinygettext::Log::set_log_info_callback(nullptr);
    tinygettext::Log::set_log_warning_callback(nullptr);
    tinygettext::Log::set_log_error_callback(nullptr);
}
