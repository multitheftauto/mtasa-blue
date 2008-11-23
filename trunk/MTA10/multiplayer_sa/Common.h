/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/Common.h
*  PURPOSE:     Common multiplayer module defines
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/



#ifndef __CMULTIPLAYERCOMMONSA
#define __CMULTIPLAYERCOMMONSA

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct eControlSet
{
	DWORD dwFrontPad;
	WORD wKeys1[19];
	DWORD dwFrontPad2;
	WORD wKeys2[19];
	WORD wTurnLeftRightAnalog[10];
	BYTE bytePadding1[138];
	BYTE byteCrouchAnalog[5];
	BYTE byteIncrementer;
	BYTE bytePadding2[15];
	DWORD dwKeyHeld;
} eControlSet;

typedef struct _GTA_CONTROLSET
{
	DWORD dwFrontPad;
	WORD wKeys1[19];
	DWORD dwFrontPad2;
	WORD wKeys2[19];
	WORD wTurnLeftRightAnalog[10];
	BYTE bytePadding1[138];
	BYTE byteCrouchAnalog[5];
	BYTE byteIncrementer;
	BYTE bytePadding2[15];
	DWORD dwKeyHeld;
} GTA_CONTROLSET;

#endif