/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CFont.h
*  PURPOSE:		Game font interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_FONT
#define __CGAME_FONT

class CFont
{
public:
	virtual void		PrintString         ( float fX, float fY, char * szString ) = 0;
    virtual void        SetScale            ( float fX, float fY ) = 0;
    virtual void        SetSlant            ( float fSlant ) = 0;
    virtual void        SetColor            ( unsigned long ABGR ) = 0;
    virtual void        SetFontStyle        ( unsigned char ucStyle ) = 0;
    virtual void        SetCentreSize       ( float fSize ) = 0;
    virtual void        SetRightJustifyWrap ( float fWrap ) = 0;
    virtual void        SetAlphaFade        ( float fAlphaFade ) = 0;
    virtual void        SetDropColor        ( unsigned long ABGR ) = 0;
    virtual void        SetEdge             ( char cEdge ) = 0;
    virtual void        SetProportional     ( unsigned char ucPropportional ) = 0;
    virtual void        SetBackground       ( unsigned char uc_1, unsigned char uc_2 ) = 0;
    virtual void        SetBackgroundColor  ( unsigned long ABGR ) = 0;
    virtual void        SetJustify          ( unsigned char ucJustify ) = 0;
    virtual void        SetOrientation      ( unsigned char ucOrientation ) = 0;
    virtual void        DrawFonts           ( void ) = 0;
};

#endif