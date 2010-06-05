/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CText.cpp
*  PURPOSE:     Renderable text container class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <CText.h>

CText::CText ( const TCHAR * wszRenderableString, SIZE * pSizeOfTextRect, DWORD x, DWORD y )
{
    // Copy in the string argument to our internal member variable.
    _tcsncpy ( m_wszRenderableString, wszRenderableString, sizeof ( m_wszRenderableString )-1 );

    // Set the default color.
    m_dwColor = 0xFFFFFFFF; // WHITE

    // Save our local position.
    m_dwX = x;
    m_dwY = y;
    
    m_bDeleted = false;
    
    memset ( &m_Size, 0, sizeof ( SIZE ) );
}

CText::~CText ( )
{
    // Destroy anything that needs to be.
}

const TCHAR * CText::GetString ( ) const
{
    return m_wszRenderableString;
}

void CText::SetString ( const TCHAR * wszString, SIZE *pSizeIn )
{
    // Copy in the string argument.
    _tcsncpy ( m_wszRenderableString, wszString, sizeof ( m_wszRenderableString )-1 );
}

DWORD CText::GetX ( ) const
{
    return m_dwX;
}

DWORD CText::GetY ( ) const
{
    return m_dwY;
}

void CText::SetX ( DWORD dwX )
{
    m_dwX = dwX;
}

void CText::SetY ( DWORD dwY )
{
    m_dwY = dwY;
}

void CText::SetColor ( DWORD dwColor )
{
    m_dwColor = dwColor;
}

DWORD CText::GetColor ( ) const
{
    return m_dwColor;
}

void CText::FlagDeleted ( bool bDeleted ) 
{
    m_bDeleted = bDeleted;
}

bool CText::GetDeleted ( ) const
{
    return m_bDeleted;
}

void CText::GetRectSize ( SIZE * pSizeOut ) const
{
    memcpy ( pSizeOut, &m_Size, sizeof ( SIZE ) );
}

void CText::SetRectSize ( SIZE * pSizeIn )
{
    memcpy ( &m_Size, pSizeIn, sizeof ( SIZE ) );
}

