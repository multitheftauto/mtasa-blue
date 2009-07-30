/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CFontSA.h
*  PURPOSE:		Header file for font handling and printing class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_FONT
#define __CGAMESA_FONT

#include <game/CFont.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define FUNC_CFont_PrintString			0x71A700
#define FUNC_CFont_SetScale             0x719380
#define FUNC_CFont_SetSlant             0x719420
#define FUNC_CFont_SetColor             0x719430
#define FUNC_CFont_SetFontStyle         0x719490
#define FUNC_CFont_SetCentreSize        0x7194e0
#define FUNC_CFont_SetRightJustifyWrap  0x7194f0
#define FUNC_CFont_SetAlphaFade         0x719500
#define FUNC_CFont_SetDropColor         0x719510
#define FUNC_CFont_SetEdge              0x719590
#define FUNC_CFont_SetProportional      0x7195b0
#define FUNC_CFont_SetBackground        0x7195c0
#define FUNC_CFont_SetBackgroundColor   0x7195e0
#define FUNC_CFont_SetJustify           0x719600
#define FUNC_CFont_SetOrientation       0x719610
#define FUNC_CFont_DrawFonts            0x71a210


class CFontSA : public CFont
{
public:
	void		PrintString         ( float fX, float fY, char * szString );
    void        SetScale            ( float fX, float fY );
    void        SetSlant            ( float fSlant );
    void        SetColor            ( unsigned long ABGR );
    void        SetFontStyle        ( unsigned char ucStyle );
    void        SetCentreSize       ( float fSize );
    void        SetRightJustifyWrap ( float fWrap );
    void        SetAlphaFade        ( float fAlphaFade );
    void        SetDropColor        ( unsigned long ABGR );
    void        SetEdge             ( char cEdge );
    void        SetProportional     ( unsigned char ucPropportional );
    void        SetBackground       ( unsigned char uc_1, unsigned char uc_2 );
    void        SetBackgroundColor  ( unsigned long ABGR );
    void        SetJustify          ( unsigned char ucJustify );
    void        SetOrientation      ( unsigned char ucOrientation );
    void        DrawFonts           ( void );
};

#endif