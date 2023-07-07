/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CLanguage.cpp
 *  PURPOSE:     Automatically load required language and localize MTA text according to locale
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../../vendor/tinygettext/log.hpp"
#define MTA_LOCALE_TEXTDOMAIN       "client"
// TRANSLATORS: Replace with your language native name
#define NATIVE_LANGUAGE_NAME _td("English")

CLocalization::CLocalization(const SString& strLocale, const SString& strLocalePath)
{
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
    for (auto iter : m_LanguageMap)
    {
        delete iter.second;
    }
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
        CVARS_SET("locale", strLocale);

    m_pCurrentLang = GetLanguage(strLocale);
}

CLanguage* CLocalization::GetLanguage(SString strLocale)
{
    strLocale = ValidateLocale(strLocale);
    CLanguage* pLanguage = MapFindRef(m_LanguageMap, strLocale);
    if (!pLanguage)
    {
        Language Lang = Language::from_name(strLocale);
        Lang = Lang ? Lang : Language::from_name("en_US");
        pLanguage = new CLanguage(m_DictManager.get_dictionary(Lang, MTA_LOCALE_TEXTDOMAIN), Lang.str(), Lang.get_name());
        MapSet(m_LanguageMap, strLocale, pLanguage);
    }
    return pLanguage;
}

//
// Get translated language name
//
SString CLocalization::GetLanguageNativeName(SString strLocale)
{
    strLocale = ValidateLocale(strLocale);
    SString strNativeName = GetLanguage(strLocale)->Translate(NATIVE_LANGUAGE_NAME);
    if (strNativeName == "English" && strLocale != "en_US")
    {
        // If native name not available, use English version
        strNativeName = GetLanguage(strLocale)->GetName();
    }
    return strNativeName;
}

SString CLocalization::Translate(const SString& strMessage)
{
    return m_pCurrentLang->Translate(strMessage);
}

SString CLocalization::TranslateWithContext(const SString& strContext, const SString& strMessage)
{
    return m_pCurrentLang->TranslateWithContext(strContext, strMessage);
}

SString CLocalization::TranslatePlural(const SString& strSingular, const SString& strPlural, const int iNum)
{
    return m_pCurrentLang->TranslatePlural(strSingular, strPlural, iNum);
}

SString CLocalization::TranslatePluralWithContext(const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum)
{
    return m_pCurrentLang->TranslatePluralWithContext(strContext, strSingular, strPlural, iNum);
}

SString CLocalization::GetTranslators()
{
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
    return m_pCurrentLang->GetCode();
}

SString CLocalization::GetLanguageName()
{
    return m_pCurrentLang->GetName();
}

// Get the file directory of the current language
SString CLocalization::GetLanguageDirectory(CLanguage* pLanguage)
{
    CLanguage* pSelectLang = pLanguage != nullptr ? pLanguage : m_pCurrentLang;
    SString    strFullPath = pSelectLang->GetDictionary().get_filepath();

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
