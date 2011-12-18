/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CClockSA.h
*  PURPOSE:     Header file for game clock class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CLOCK
#define __CGAMESA_CLOCK

#include <game/CClock.h>
#include "Common.h"

#define VAR_TimeMinutes                 0xB70152
#define VAR_TimeHours                   0xB70153
#define VAR_TimeOfLastMinuteChange      0xB70158

class CClockSA : public CClock
{
public:
    VOID            Set ( BYTE bHour, BYTE bMinute );
    VOID            Get ( BYTE * bHour, BYTE * bMinute );
};

#endif
