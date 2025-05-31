/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CClockSA.h
 *  PURPOSE:     Header file for game clock class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CClock.h>

#define VAR_TimeMinutes                 0xB70152
#define VAR_TimeHours                   0xB70153
#define VAR_TimeOfLastMinuteChange      0xB70158

class CClockSA : public CClock
{
public:
    void Set(BYTE bHour, BYTE bMinute);
    void Get(BYTE* bHour, BYTE* bMinute);

    bool SetTimeFrozen(bool value) noexcept;
    bool IsTimeFrozen() const noexcept { return m_bTimeCycleFrozen; };
    bool ResetTimeFrozen() noexcept;

private:
    bool m_bTimeCycleFrozen;
};
