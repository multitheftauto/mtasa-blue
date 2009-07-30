/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CFontSA.cpp
*  PURPOSE:		Font handling and printing
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CFontSA::PrintString ( float fX, float fY, char * szString )
{
	DWORD dwFunc = FUNC_CFont_PrintString;
	_asm
	{
		push	szString
		push	fY
		push	fX
		call	dwFunc
		add		esp, 12
	}
}


void CFontSA::SetScale ( float fX, float fY )
{
    DWORD dwFunc = FUNC_CFont_SetScale;
	_asm
	{
		push	fY
		push	fX
		call	dwFunc
		add		esp, 8
	}
}


void CFontSA::SetSlant ( float fSlant )
{
    DWORD dwFunc = FUNC_CFont_SetSlant;
	_asm
	{
		push	fSlant
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetColor ( unsigned long ABGR )
{
    DWORD dwFunc = FUNC_CFont_SetColor;
	_asm
	{
		push	ABGR
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetFontStyle ( unsigned char ucStyle )
{
    DWORD dwFunc = FUNC_CFont_SetFontStyle;
	_asm
	{
		push	ucStyle
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetCentreSize ( float fSize )
{
    DWORD dwFunc = FUNC_CFont_SetCentreSize;
	_asm
	{
		push	fSize
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetRightJustifyWrap ( float fWrap )
{
    DWORD dwFunc = FUNC_CFont_SetRightJustifyWrap;
	_asm
	{
		push	fWrap
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetAlphaFade ( float fFade )
{
    DWORD dwFunc = FUNC_CFont_SetAlphaFade;
	_asm
	{
		push	fFade
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetDropColor ( unsigned long ABGR )
{
    DWORD dwFunc = FUNC_CFont_SetDropColor;
	_asm
	{
		push	ABGR
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetEdge ( char cEdge )
{
    DWORD dwFunc = FUNC_CFont_SetEdge;
	_asm
	{
		push	cEdge
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetProportional ( unsigned char ucProportional )
{
    DWORD dwFunc = FUNC_CFont_SetProportional;
	_asm
	{
		push	ucProportional
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetBackground ( unsigned char uc_1, unsigned char uc_2 )
{
    DWORD dwFunc = FUNC_CFont_SetBackground;
	_asm
	{
		push	uc_2
        push    uc_1
		call	dwFunc
		add		esp, 8
	}
}


void CFontSA::SetBackgroundColor ( unsigned long ABGR )
{
    DWORD dwFunc = FUNC_CFont_SetBackgroundColor;
	_asm
	{
		push	ABGR
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetJustify ( unsigned char ucJustify )
{
    DWORD dwFunc = FUNC_CFont_SetJustify;
	_asm
	{
		push	ucJustify
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::SetOrientation ( unsigned char ucOrientation )
{
    DWORD dwFunc = FUNC_CFont_SetOrientation;
	_asm
	{
		push	ucOrientation
		call	dwFunc
		add		esp, 4
	}
}


void CFontSA::DrawFonts ( void )
{
    DWORD dwFunc = FUNC_CFont_DrawFonts;
	_asm call	dwFunc
}