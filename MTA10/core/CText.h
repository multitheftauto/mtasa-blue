/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CText.h
*  PURPOSE:     Header file for renderable text class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTEXT_H
#define __CTEXT_H

#include <windows.h>
#include <tchar.h>

// Size of string to allocate.
#define CTEXT_MAX_STRING_SIZE 256

class CText
{
    public:
            
                    CText       ( const TCHAR * wszRenderableString, SIZE * pSizeOfTextRect, DWORD x = 0, DWORD y = 0 );
                   ~CText       ( );

    const TCHAR *   GetString   ( ) const;
    void            SetString   ( const TCHAR * wszRenderableString, SIZE * pSizeIn );

    DWORD           GetX        ( ) const;
    DWORD           GetY        ( ) const;
    void            SetX        ( DWORD dwX );
    void            SetY        ( DWORD dwY );
   
    DWORD           GetColor    ( ) const;
    void            SetColor    ( DWORD dwColor );

    void            FlagDeleted ( bool bDeleted );
    bool            GetDeleted  ( ) const;

    void            GetRectSize ( SIZE * pSizeOut ) const;
    void            SetRectSize ( SIZE * pSizeIn );

    private:

    bool        m_bDeleted;
    DWORD       m_dwColor;
    DWORD       m_dwX;
    DWORD       m_dwY;
    TCHAR       m_wszRenderableString [ CTEXT_MAX_STRING_SIZE ];
    SIZE        m_Size;
};

#endif