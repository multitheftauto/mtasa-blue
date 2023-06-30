/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CClockSA.h
 *  PURPOSE:     Header file for game clock class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CClock.h>

class CClockSA : public CClock
{
public:
    void Set(BYTE bHour, BYTE bMinute);
    void Get(BYTE* bHour, BYTE* bMinute);
};
