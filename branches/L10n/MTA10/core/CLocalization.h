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
                                CLocalization               ( SString strLocalePath = "" );
                                ~CLocalization              ( void );

    SString                     Translate                   ( SString strMessage );
    SString                     TranslateWithContext        ( SString strContext, SString strMessage );
    SString                     TranslatePlural             ( SString strSingular, SString strPlural, int iNum );
    SString                     TranslatePluralWithContext  ( SString strContext, SString strSingular, SString strPlural, int iNum );

    std::map<SString,SString>   GetAvailableLanguages       ( void );
    bool                        IsLocalized                 ( void );
    SString                     GetLanguageDirectory        ( void );

private:
    Dictionary                  m_CurrentDict; 
    DictionaryManager           m_DictManager;

    CLanguage*                  m_pCurrentLang;

};




#endif