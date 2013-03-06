/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CLocalizationInterface.h
*  PURPOSE:     Dynamic command manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLOCALIZATIONINTERFACE_H
#define __CLOCALIZATIONINTERFACE_H

#ifdef _
#undef _
#endif

#define _       g_pCore->GetLocalization()->Translate
#define _c      g_pCore->GetLocalization()->TranslateWithContext
#define _n      g_pCore->GetLocalization()->TranslatePlural
#define _cn     g_pCore->GetLocalization()->TranslatePluralWithContext

class CLocalizationInterface
{
public:
    virtual SString             Translate                   ( SString strMessage ) = 0;
    virtual SString             TranslateWithContext        ( SString strContext, SString strMessage ) = 0;
    virtual SString             TranslatePlural             ( SString strSingular, SString strPlural, int iNum ) = 0;
    virtual SString             TranslatePluralWithContext  ( SString strContext, SString strSingular, SString strPlural, int iNum ) = 0;

    virtual std::map<SString,SString>   GetAvailableLanguages       ( void ) = 0;
    virtual bool                        IsLocalized                 ( void ) = 0;
    virtual SString                     GetLanguageDirectory        ( void ) = 0;
};

#endif
