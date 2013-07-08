/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CLocalization.h
*  PURPOSE:     Automatically load required language and localize MTA text according to locale
*  DEVELOPERS:  Dan Chowdhury <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

using namespace tinygettext;

#include <core/CLocalizationInterface.h>
#include "CLanguage.h"

#ifndef __CLOCALIZATION_H
#define __CLOCALIZATION_H

class CLocalization : public CLocalizationInterface
{
public:
                                CLocalization               ( SString strLocale = "", SString strLocalePath = "" );
                                ~CLocalization              ( void );

    SString                     Translate                   ( const SString& strMessage );
    SString                     TranslateWithContext        ( const SString& strContext, const SString& strMessage );
    SString                     TranslatePlural             ( const SString& strSingular, const SString& strPlural, int iNum );
    SString                     TranslatePluralWithContext  ( const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum );

    SString                     GetTranslators              ( void );
    std::map<SString,SString>   GetAvailableLanguages       ( void );
    bool                        IsLocalized                 ( void );
    SString                     GetLanguageDirectory        ( void );

private:
    Dictionary                  m_CurrentDict; 
    DictionaryManager           m_DictManager;

    CLanguage*                  m_pCurrentLang;

};




#endif