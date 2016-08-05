/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CLanguage.h
*  PURPOSE:     Class to abstract a translation file to translated strings
*  DEVELOPERS:  Dan Chowdhury <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

using namespace tinygettext;

#ifndef __CLANGUAGE_H
#define __CLANGUAGE_H

class CLanguage
{
public:
                                CLanguage                   ( Dictionary Dict, const SString& strLocale = "", const SString& strLangName = "" );
                                CLanguage                   ( const SString& strPOPath );
                                ~CLanguage                  ( void );

    SString                     Translate                   ( const SString& strMessage );
    SString                     TranslateWithContext        ( const SString& strContext, const SString& strMessage );
    SString                     TranslatePlural             ( const SString& strSingular, const SString& strPlural, int iNum );
    SString                     TranslatePluralWithContext  ( const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum );

    SString                     GetCode                     ( void )    {   return m_strCode; }
    SString                     GetName                     ( void )    {   return m_strName; }

private:
    Dictionary                  m_Dict; 
    SString                     m_strCode;  // Language code
    SString                     m_strName;  // Human readable name
};




#endif