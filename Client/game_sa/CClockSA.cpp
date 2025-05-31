/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CClockSA.cpp
 *  PURPOSE:     Game clock
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClockSA.h"
#include "CGameSA.h"

extern CGameSA* pGame;

void CClockSA::Set(BYTE bHour, BYTE bMinute)
{
    if (bMinute >= 0 && bMinute <= 60 && bHour >= 0 && bHour <= 23)
    {
        MemPutFast<BYTE>(VAR_TimeMinutes, bMinute);
        MemPutFast<BYTE>(VAR_TimeHours, bHour);
        MemPutFast<DWORD>(VAR_TimeOfLastMinuteChange, pGame->GetSystemTime());
    }
}

void CClockSA::Get(BYTE* bHour, BYTE* bMinute)
{
    *bMinute = *(BYTE*)VAR_TimeMinutes;
    *bHour = *(BYTE*)VAR_TimeHours;
}

bool CClockSA::SetTimeFrozen(bool value) noexcept
{
    if (value)
        MemSet((void*)0x53BFBD, 0x90, 5);
    else
        MemCpy((void*)0x53BFBD, "\xE8\x4E\x0F\xFF\xFF", 5);

    m_bTimeCycleFrozen = value;
    return true;
}

bool CClockSA::ResetTimeFrozen() noexcept
{
    return SetTimeFrozen(false);
}
