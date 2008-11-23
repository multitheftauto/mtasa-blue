/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBlendedWeather.cpp
*  PURPOSE:     Game weather blending
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CBlendedWeather::CBlendedWeather ( void )
{
    // Init
    m_ucPrimaryWeather = 0;
    m_ucSecondaryWeather = 0;
    m_ucPrimaryBlendedWeather = 0;
    m_ucSecondaryBlendedWeather = 0;

    m_ucBlendStartHour = 0xFF;
    m_ucBlendStopHour = 0xFF;

    m_pClock = g_pGame->GetClock ();
    m_pWeather = g_pGame->GetWeather ();
}


void CBlendedWeather::DoPulse ( void )
{
    // Grab the current time
    unsigned char ucHour, ucMinute;
    m_pClock->Get ( &ucHour, &ucMinute );

    // Eventually get new weather values for blending
    if ( m_ucBlendStartHour != 0xFF )
    {
        if ( m_ucBlendStartHour == ucHour )
        {
            m_ucPrimaryWeather = m_ucPrimaryBlendedWeather;
            m_ucSecondaryWeather = m_ucSecondaryBlendedWeather;
            m_ucBlendStartHour = 0xFF;
        }
    }
    else if ( m_ucBlendStopHour != 0xFF )
    {
        if ( m_ucBlendStopHour == ucHour )
        {
            m_ucPrimaryWeather = m_ucSecondaryBlendedWeather;
            m_ucBlendStopHour = 0xFF;
        }
    }

    // Force the weather
    m_pWeather->Set ( static_cast < unsigned char > ( m_ucPrimaryWeather ), static_cast < unsigned char > ( m_ucSecondaryWeather ) );
}


void CBlendedWeather::SetWeather ( unsigned char ucWeather )
{
    // Set the primary and secondary weather instantly and stop any blending we might've been doing
    m_ucPrimaryWeather = ucWeather;
    m_ucSecondaryWeather = ucWeather;
    m_ucBlendStartHour = 0xFF;
    m_ucBlendStopHour = 0xFF;
}


void CBlendedWeather::SetWeatherBlended ( unsigned char ucWeather, unsigned char ucHour )
{
    // Store the weather we blend to for the timing
    m_ucPrimaryBlendedWeather = m_ucSecondaryWeather;
    m_ucSecondaryBlendedWeather = ucWeather;

    // Set the time we start/stop blending
    m_ucBlendStartHour = ucHour;
    m_ucBlendStopHour = ucHour + 1;
    if ( m_ucBlendStopHour > 23 )
    {
        m_ucBlendStopHour = 0;
    }
}


unsigned char CBlendedWeather::GetWeather ( void )
{
    // If we're blending the weather, return the weather we started blending from
    if ( m_ucBlendStopHour != 0xFF )
    {
        return m_ucPrimaryBlendedWeather;
    }

    // Return the current weather
    return m_ucPrimaryWeather;
}
