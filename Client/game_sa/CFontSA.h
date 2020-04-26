/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFontSA.h
 *  PURPOSE:     Header file for font handling and printing class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CFont.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define FUNC_PrintString            0x71A700

class CFontSA : public CFont
{
public:
    void PrintString(float fX, float fY, char* szString);
};
