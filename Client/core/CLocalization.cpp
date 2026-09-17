/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CLanguage.cpp
 *  PURPOSE:     Automatically load required language and localize MTA text according to locale
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../../vendor/tinygettext/log.hpp"
#define MTA_LOCALE_TEXTDOMAIN "client"
// TRANSLATORS: Replace with your language native name
#define NATIVE_LANGUAGE_NAME _td("English")

struct NativeLanguageName
{
    std::string locale;
    std::string name;
} g_nativeLanguageNames[] = {
#include "languages.generated.h"
};

CLocalization::CLocalization(const SString& strLocale, const SString& strLocalePath)
{
    m_pCurrentLang = NULL;

    // Set log callbacks so we can record problems
    Log::set_log_info_callback(NULL);
    Log::set_log_warning_callback(LogCallback);
    Log::set_log_error_callback(LogCallback);

    // Setup our dictionary manager
    m_DictManager.add_directory(strLocalePath.empty() ? CalcMTASAPath(MTA_LOCALE_DIR) : strLocalePath);

    // Initialize our language
    SetCurrentLanguage(strLocale);
}

CLocalization::~CLocalization()
{
    m_pCurrentLang = nullptr;
}

//
// Ensure supplied locale is valid. Uses settings if input is empty
//
SString CLocalization::ValidateLocale(SString strLocale)
{
    if (strLocale.empty() && (CClientVariables::GetSingletonPtr() == nullptr || !CVARS_GET("locale", strLocale)))
        strLocale = "en_US";

    // NOTE(patrik): Convert old two letter language to locale to fix any issues with flags
    if (strLocale == "en")
        strLocale = "en_US";
    else if (strLocale == "fi")
        strLocale = "fi_FI";
    else if (strLocale == "az")
        strLocale = "az_AZ";
    else if (strLocale == "ka")
        strLocale = "ka_GE";

    Language Lang = Language::from_name(strLocale);
    Lang = Lang ? Lang : Language::from_name("en_US");
    return Lang.str();
}

//
// Switch current language to supplied locale
//
void CLocalization::SetCurrentLanguage(SString strLocale)
{
    strLocale = ValidateLocale(strLocale);
    WriteDebugEvent(SString("Localization set to '%s'", strLocale.c_str()));

    // Update our locale setting with full country code, now that we've matched it
    if (g_pCore)
    {
        SString strCurrentLocale = CVARS_GET_VALUE<SString>("locale");
        if (strCurrentLocale != strLocale)
            CVARS_SET("locale", strLocale);
    }

    SString strStoredLocale = GetApplicationSetting("locale");
    if (strStoredLocale != strLocale)
        SetApplicationSetting("locale", strLocale);

    if (m_pCurrentLang && m_pCurrentLang->GetCode() == strLocale)
        return;

    m_pCurrentLang = GetLanguage(strLocale);
}

CLanguage* CLocalization::GetLanguage(SString strLocale)
{
    strLocale = ValidateLocale(strLocale);
    auto iter = m_LanguageMap.find(strLocale);
    if (iter != m_LanguageMap.end())
    {
        return iter->second.get();
    }

    Language Lang = Language::from_name(strLocale);
    Lang = Lang ? Lang : Language::from_name("en_US");

    try
    {
        std::unique_ptr<CLanguage> pLanguage =
            std::make_unique<CLanguage>(m_DictManager.get_dictionary(Lang, MTA_LOCALE_TEXTDOMAIN), Lang.str(), Lang.get_name());
        CLanguage* pLanguagePtr = pLanguage.get();
        m_LanguageMap.emplace(strLocale, std::move(pLanguage));
        return pLanguagePtr;
    }
    catch (const std::exception& ex)
    {
        WriteDebugEvent(SString("Localization failed to load dictionary for '%s': %s", strLocale.c_str(), ex.what()));
        return (strLocale != "en_US") ? GetLanguage("en_US") : nullptr;
    }
    catch (...)
    {
        WriteDebugEvent(SString("Localization failed to load dictionary for '%s': unknown error", strLocale.c_str()));
        return (strLocale != "en_US") ? GetLanguage("en_US") : nullptr;
    }
}

//
// Get translated language name
//
SString CLocalization::GetLanguageNativeName(SString strLocale)
{
    strLocale = ValidateLocale(strLocale);

    // Try to find the native language name in our sorted compile-time array first, using binary search.
    auto begin = g_nativeLanguageNames;
    auto end = g_nativeLanguageNames + _countof(g_nativeLanguageNames);
    auto iter = std::lower_bound(begin, end, strLocale, [](const NativeLanguageName& a, const std::string& b) { return a.locale < b; });

    if (iter != end && iter->locale == strLocale)
        return iter->name;

    // If not found, we fall back to the loading the language file and using the name from there.
    CLanguage* pLanguage = GetLanguage(strLocale);
    if (!pLanguage)
        return SString();

    SString strNativeName = pLanguage->Translate(NATIVE_LANGUAGE_NAME);
    if (strNativeName == "English" && strLocale != "en_US")
    {
        // If native name not available, use English version
        strNativeName = pLanguage->GetName();
    }

    return strNativeName;
}

SString CLocalization::Translate(const SString& strMessage)
{
    if (!m_pCurrentLang)
        return strMessage;

    return m_pCurrentLang->Translate(strMessage);
}

SString CLocalization::TranslateWithContext(const SString& strContext, const SString& strMessage)
{
    if (!m_pCurrentLang)
        return strMessage;

    return m_pCurrentLang->TranslateWithContext(strContext, strMessage);
}

SString CLocalization::TranslatePlural(const SString& strSingular, const SString& strPlural, const int iNum)
{
    if (!m_pCurrentLang)
        return (iNum == 1 ? strSingular : strPlural);

    return m_pCurrentLang->TranslatePlural(strSingular, strPlural, iNum);
}

SString CLocalization::TranslatePluralWithContext(const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum)
{
    if (!m_pCurrentLang)
        return (iNum == 1 ? strSingular : strPlural);

    return m_pCurrentLang->TranslatePluralWithContext(strContext, strSingular, strPlural, iNum);
}

SString CLocalization::GetTranslators()
{
    if (!m_pCurrentLang)
        return "";

    std::map<std::string, std::string> metaData = m_pCurrentLang->GetDictionary().get_metadata();
    if (metaData.find("Translators") != metaData.end())
    {
        SString strTranslatorsList = metaData["Translators"];
        return strTranslatorsList.Replace("; ", "\n");
    }
    return "";
}

std::vector<SString> CLocalization::GetAvailableLocales()
{
    std::vector<SString> localeList = {"en_US"};
    for (const auto& language : m_DictManager.get_languages(MTA_LOCALE_TEXTDOMAIN))
        // To avoid duplicates
        if (std::find(localeList.begin(), localeList.end(), language.str()) == localeList.end())
            localeList.push_back(language.str());
    // Alpha sort
    std::sort(localeList.begin(), localeList.end());
    return localeList;
}

// Tell whether the client is translated
bool CLocalization::IsLocalized()
{
    std::string strLocale;
    CVARS_GET("locale", strLocale);
    return strLocale != "en_US";
}

SString CLocalization::GetLanguageCode()
{
    return m_pCurrentLang ? m_pCurrentLang->GetCode() : SString();
}

SString CLocalization::GetLanguageName()
{
    return m_pCurrentLang ? m_pCurrentLang->GetName() : SString();
}

// Get the file directory of the current language
SString CLocalization::GetLanguageDirectory(CLanguage* pLanguage)
{
    CLanguage* pSelectLang = pLanguage != nullptr ? pLanguage : m_pCurrentLang;
    if (!pSelectLang)
        return SString();

    SString strFullPath = pSelectLang->GetDictionary().get_filepath();

    // Replace all backslashes with forward slashes
    int idx = 0;
    while ((idx = strFullPath.find_first_of("\\", idx)) >= 0)
        strFullPath.replace(idx, 1, "/");

    // Return everything up until (and including) the final forwardslash
    return strFullPath.substr(0, strFullPath.find_last_of('/') + 1);
}

void CLocalization::LogCallback(const std::string& str)
{
    WriteDebugEvent((SStringX("Localization: ") + str).TrimEnd("\n"));
}

///////////////////////////////////////////////////////
//
// Global interface
//
MTAEXPORT CLocalizationInterface* __cdecl L10n_CreateLocalization(SString strLocale)
{
    // Eventually create a localization interface
    if (!g_pLocalization)
        g_pLocalization = new CLocalization(strLocale);

    return g_pLocalization;
}
