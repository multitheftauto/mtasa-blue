/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClock.h
 *  PURPOSE:     Game clock class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CClock
{
public:
    CClock();

    void Get(unsigned char& ucHour, unsigned char& ucMinute);
    void Set(unsigned char ucHour, unsigned char ucMinute);

    void          SetMinuteDuration(unsigned long ulDuration);
    unsigned long GetMinuteDuration();

private:
    unsigned long long m_ullMidnightTime;
    unsigned long      m_ulMinuteDuration;
};
