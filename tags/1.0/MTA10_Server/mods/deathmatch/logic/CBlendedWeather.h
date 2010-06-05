/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBlendedWeather.h
*  PURPOSE:     Weather blend manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CBLENDEDWEATHER_H
#define __CBLENDEDWEATHER_H

#include "CClock.h"

class CBlendedWeather
{
public:
                            CBlendedWeather             ( CClock* pClock );

    void                    SetWeather                  ( unsigned char ucWeather );
    void                    SetWeatherBlended           ( unsigned char ucWeather, unsigned char ucHour );

    unsigned char           GetWeather                  ( void );
    inline unsigned char    GetWeatherBlendingTo        ( void )    { return m_ucSecondaryBlendedWeather; };
    inline unsigned char    GetBlendedStartHour         ( void )    { return m_ucBlendStartHour; };
    inline unsigned char    GetBlendedStopHour          ( void )    { return m_ucBlendStopHour; };

private:
    void                    Update                      ( void );

    unsigned char           m_ucPrimaryWeather;
    unsigned char           m_ucSecondaryWeather;
    unsigned char           m_ucPrimaryBlendedWeather;
    unsigned char           m_ucSecondaryBlendedWeather;

    unsigned char           m_ucBlendStartHour;
    unsigned char           m_ucBlendStopHour;

    CClock*                 m_pClock;
};

#endif
