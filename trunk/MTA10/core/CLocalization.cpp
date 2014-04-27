/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CLanguage.cpp
*  PURPOSE:     Automatically load required language and localize MTA text according to locale
*  DEVELOPERS:  Dan Chowdhury <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define MTA_LOCALE_DIR              "MTA/locale/"
#define MTA_LOCALE_TEXTDOMAIN       "client"

CLocalization::CLocalization ( SString strLocale, SString strLocalePath )
{
    strLocalePath = strLocalePath.empty() ? CalcMTASAPath ( MTA_LOCALE_DIR ) : strLocalePath;

    // Initialize our language
    if ( strLocale.empty() && !CVARS_GET("locale", strLocale) )
        strLocale = "en_US";
    
    WriteDebugEvent( SString("CLocalization::CLocalization Localization set to '%s'",strLocale.c_str()) );

    // Grab the nearest language based upon our setting, or revert to en_US
    Language Lang = Language::from_name(strLocale);
    Lang = Lang ? Lang : Language::from_name("en_US");

    // Update our locale setting with full country code, now that we've matched it
    strLocale = Lang.str();
    if ( g_pCore )
        CVARS_SET("locale", strLocale); 
    
    // Setup our dictionary manager
    m_DictManager.add_directory ( strLocalePath );
    
    // Grab our translation dictionary from this dir
    m_CurrentDict = m_DictManager.get_dictionary ( Lang, MTA_LOCALE_TEXTDOMAIN );

    m_pCurrentLang = new CLanguage ( m_CurrentDict, strLocale, Lang.get_name() );
}

CLocalization::~CLocalization ( void )
{
    delete m_pCurrentLang;
}

SString CLocalization::Translate ( const SString & strMessage )
{
    return m_pCurrentLang->Translate( strMessage ) ;
}

SString CLocalization::TranslateWithContext ( const SString& strContext, const SString & strMessage )
{
    return m_pCurrentLang->TranslateWithContext( strContext, strMessage );
}

SString CLocalization::TranslatePlural ( const SString& strSingular, const SString & strPlural, const int iNum )
{
    return m_pCurrentLang->TranslatePlural ( strSingular, strPlural, iNum );
}

SString CLocalization::TranslatePluralWithContext ( const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum )
{
    return m_pCurrentLang->TranslatePluralWithContext ( strContext, strSingular, strPlural, iNum );
}

SString CLocalization::GetTranslators ( )
{
    std::map<std::string,std::string> metaData = m_CurrentDict.get_metadata();
    if ( metaData.find("Translators") != metaData.end() )
    {
        SString strTranslatorsList = metaData["Translators"];
        return strTranslatorsList.Replace("; ","\n");
    }
    return "";
}

std::map<SString,SString> CLocalization::GetAvailableLanguages ( void )
{
    std::map<SString,SString> m_LanguageMap;
    const std::set<Language>& languages = m_DictManager.get_languages( MTA_LOCALE_TEXTDOMAIN );

    for (std::set<Language>::const_iterator i = languages.begin(); i != languages.end(); ++i)
         m_LanguageMap[i->get_name()] = i->str();
    
    return m_LanguageMap;
}

// Tell whether the client is translated
bool CLocalization::IsLocalized ( void )
{
    std::string strLocale;
    CVARS_GET("locale", strLocale);
    return strLocale != "en_US";
}

SString CLocalization::GetLanguageCode ( void )
{
    return m_pCurrentLang->GetCode();
}

SString CLocalization::GetLanguageName ( void )
{
    return m_pCurrentLang->GetName();
}

// Get the file directory of the current language
SString CLocalization::GetLanguageDirectory ( void )
{
    SString strFullPath = m_CurrentDict.get_filepath();
    
    // Replace all backslashes with forward slashes
    int idx = 0;
    while( (idx=strFullPath.find_first_of("\\", idx)) >= 0 )
        strFullPath.replace(idx, 1, "/");

    // Return everything up until (and including) the final forwardslash
    return strFullPath.substr( 0, strFullPath.find_last_of( '/' ) +1 );
}


///////////////////////////////////////////////////////
//
// Global interface
//
extern "C" _declspec(dllexport) CLocalizationInterface* __cdecl L10n_CreateLocalization ( SString strLocale )
{
    // Eventually create a localization interface
    if ( !g_pLocalization )
        g_pLocalization = new CLocalization ( strLocale );

    return g_pLocalization;
}
