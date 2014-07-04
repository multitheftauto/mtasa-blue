/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CLanguageLocale.cpp
*  PURPOSE:     Language locale for handling different languages
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::string;

template<> CLanguageLocale * CSingleton< CLanguageLocale >::m_pSingleton = NULL;

CLanguageLocale::CLanguageLocale ( )
{
}

CLanguageLocale::~CLanguageLocale ( )
{
}

BOOL CLanguageLocale::GetString ( LANGUAGETYPE Type, string &strOut )
{
    // RETURNS: TRUE/FALSE indicating successful retrieval of string
    //          into strOut.

    return FALSE;
}

VOID CLanguageLocale::SetDefaultLanguageIdentifier ( LANGUAGETYPE Type )
{
    // Set our internal variable to the requested language.
    m_CurrentLanguageIdentifier = Type;
}

LANGUAGETYPE CLanguageLocale::GetLocalLanguageIdentifier ( )
{
    return m_CurrentLanguageIdentifier;
}

