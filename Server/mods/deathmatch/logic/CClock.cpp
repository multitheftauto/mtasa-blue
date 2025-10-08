/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClock.cpp
 *  PURPOSE:     Game clock class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClock.h"

CClock::CClock()
{
    // Default to 12:00
    m_ulMinuteDuration = 1000;
    Set(12, 0);
}

void CClock::Get(unsigned char& ucHour, unsigned char& ucMinute)
{
    // Grab how many seconds midnight was from now
    unsigned long long ullSecondsSinceMidnight = (GetTickCount64_() - m_ullMidnightTime) / m_ulMinuteDuration;

    // Grab how many hours that is
    ucMinute = static_cast<unsigned char>(ullSecondsSinceMidnight % 60);
    ullSecondsSinceMidnight /= 60;

    // What's left is the number of hours, limit it to 24 and return it
    ucHour = static_cast<unsigned char>(ullSecondsSinceMidnight % 24);
}

void CClock::Set(unsigned char ucHour, unsigned char ucMinute)
{
    // Calculate number of seconds from midnight
    unsigned long ulSecondsFromMidnight = ucHour * 60 + ucMinute;

    // Set the midnight time to our current time minus number of seconds since midnight according to the time we were given
    m_ullMidnightTime = GetTickCount64_() - ulSecondsFromMidnight * m_ulMinuteDuration;
}

unsigned long CClock::GetMinuteDuration()
{
    return m_ulMinuteDuration;
}

void CClock::SetMinuteDuration(unsigned long ulDuration)
{
    // We gotta reset the time with the new minute duration
    // so getTime returns the right values
    unsigned char ucHour;
    unsigned char ucMinute;
    Get(ucHour, ucMinute);
    m_ulMinuteDuration = ulDuration;
    Set(ucHour, ucMinute);
}
