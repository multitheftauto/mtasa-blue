/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CLanguageLocale.h
*  PURPOSE:     Header file for language locale class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLANGUAGELOCALE_H
#define __CLANGUAGELOCALE_H

#include <windows.h>
#include <string>
#include "CSingleton.h"

enum LANGUAGETYPE
{
    LT_DEFAULT,
    LT_ENGLISH,
    LT_ARABIC,
    LT_FRENCH,
    LT_SPANISH,
    LT_GERMAN,
    LT_RUSSIAN,
    LT_ITALIAN,
    LT_CHINESE,
    LT_KOREAN
};

class CLanguageLocale : public CSingleton < CLanguageLocale >
{
    public:

                    CLanguageLocale                 ( );
                   ~CLanguageLocale                 ( );

    LANGUAGETYPE    GetLocalLanguageIdentifier      ( );
    VOID            SetDefaultLanguageIdentifier    ( LANGUAGETYPE Type );
    BOOL            GetString                       ( LANGUAGETYPE Type, std::string& strOut );
    BOOL            LoadStringTable                 ( );

    private:

    LANGUAGETYPE    m_CurrentLanguageIdentifier;

};

#endif
