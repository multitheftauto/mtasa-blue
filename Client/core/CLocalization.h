/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CLocalization.h
 *  PURPOSE:     Automatically load required language and localize MTA text according to locale
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

using namespace tinygettext;

#include <core/CLocalizationInterface.h>
#include "CLanguage.h"
#define MTA_LOCALE_DIR  "MTA/locale/"

#pragma once

class CLocalization : public CLocalizationInterface
{
public:
    CLocalization(const SString& strLocale = "", const SString& strLocalePath = "");
    ~CLocalization(void);

    SString Translate(const SString& strMessage);
    SString TranslateWithContext(const SString& strContext, const SString& strMessage);
    SString TranslatePlural(const SString& strSingular, const SString& strPlural, int iNum);
    SString TranslatePluralWithContext(const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum);

    SString              GetTranslators(void);
    std::vector<SString> GetAvailableLocales(void);
    bool                 IsLocalized(void);
    SString              GetLanguageDirectory(void);
    SString              GetLanguageCode(void);
    SString              GetLanguageName(void);
    SString              ValidateLocale(SString strLocale);
    void                 SetCurrentLanguage(SString strLocale = "");
    CLanguage*           GetLanguage(SString strLocale = "");
    SString              GetLanguageNativeName(SString strLocale = "");

    static void LogCallback(const std::string& str);

private:
    DictionaryManager             m_DictManager;
    std::map<SString, CLanguage*> m_LanguageMap;
    CLanguage*                    m_pCurrentLang;
};
