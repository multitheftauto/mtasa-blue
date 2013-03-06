/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CLanguage.cpp
*  PURPOSE:     Class to abstract a translation file to translated strings
*  DEVELOPERS:  Dan Chowdhury <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "po_parser.hpp"

CLanguage::CLanguage ( Dictionary Dict )
{ 
    m_Dict = Dict;
}

CLanguage::CLanguage ( SString strPOPath )
{ 
    std::ifstream in( strPOPath );
    POParser::parse( strPOPath, in, m_Dict );
    in.close();
}


CLanguage::~CLanguage ( void )
{
}

SString CLanguage::Translate ( SString strMessage )
{
    return SString(m_Dict.translate ( strMessage ));;
}

SString CLanguage::TranslateWithContext ( SString strContext, SString strMessage )
{
    return SString(m_Dict.translate_ctxt ( strContext, strMessage ));
}

SString CLanguage::TranslatePlural ( SString strSingular, SString strPlural, int iNum )
{
    return SString(m_Dict.translate_plural ( strSingular, strPlural, iNum ));
}

SString CLanguage::TranslatePluralWithContext ( SString strContext, SString strSingular, SString strPlural, int iNum )
{
    return SString(m_Dict.translate_ctxt_plural ( strContext, strSingular, strPlural, iNum ));
}
