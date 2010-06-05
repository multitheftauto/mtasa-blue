/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClock.cpp
*  PURPOSE:     Game clock class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CClock::CClock ( void )
{
    // Default to 12:00
	m_ulMinuteDuration = 1000;
    Set ( 12, 0 );
}


void CClock::Get ( unsigned char& ucHour, unsigned char& ucMinute )
{
    // Grab how many seconds midnight was from now
    unsigned long ulSecondsSinceMidnight = ( GetTime () - m_ulMidnightTime ) / m_ulMinuteDuration;

    // Grab how many hours that is
    ucMinute = static_cast < unsigned char > ( ulSecondsSinceMidnight % 60 );
    ulSecondsSinceMidnight /= 60;

    // What's left is the number of hours, limit it to 24 and return it
    ucHour = static_cast < unsigned char > ( ulSecondsSinceMidnight % 24 );
}


void CClock::Set ( unsigned char ucHour, unsigned char ucMinute )
{
    // Calculate number of seconds from midnight
    unsigned long ulSecondsFromMidnight = ucHour * 60 + ucMinute;

    // Set the midnight time to our current time minus number of seconds since midnight according to the time we were given
    m_ulMidnightTime = GetTime () - ulSecondsFromMidnight * m_ulMinuteDuration;
}


unsigned long CClock::GetMinuteDuration ( void )
{
	return m_ulMinuteDuration;
}


void CClock::SetMinuteDuration ( unsigned long ulDuration )
{
	// We gotta reset the time with the new minute duration
	// so getTime returns the right values
	unsigned char ucHour;
	unsigned char ucMinute;
	Get ( ucHour, ucMinute );
	m_ulMinuteDuration = ulDuration;
	Set ( ucHour, ucMinute );
}
