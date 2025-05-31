/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CClock.h
 *  PURPOSE:     Game clock interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CClock
{
public:
    virtual void Set(BYTE bHour, BYTE bMinute) = 0;
    virtual void Get(BYTE* bHour, BYTE* bMinute) = 0;

    virtual bool SetTimeFrozen(bool value) noexcept = 0;
    virtual bool IsTimeFrozen() const noexcept = 0;
    virtual bool ResetTimeFrozen() noexcept = 0;
};
