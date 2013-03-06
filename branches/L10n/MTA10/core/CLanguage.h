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
                                CLanguage                   ( Dictionary Dict );
                                CLanguage                   ( SString strPOPath );
                                ~CLanguage                  ( void );

    SString                     Translate                   ( SString strMessage );
    SString                     TranslateWithContext        ( SString strContext, SString strMessage );
    SString                     TranslatePlural             ( SString strSingular, SString strPlural, int iNum );
    SString                     TranslatePluralWithContext  ( SString strContext, SString strSingular, SString strPlural, int iNum );

private:
    Dictionary                  m_Dict; 
};




#endif