/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CWeather.h
*  PURPOSE:     Game weather interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_WEATHER
#define __CGAME_WEATHER

#include "Common.h"

class CWeather
{
public:
    virtual unsigned char   Get                     ( void ) = 0;
    virtual void            Set                     ( unsigned char primary, unsigned char secondary ) = 0;
    virtual void            SetPrimary              ( unsigned char weather ) = 0;
    virtual void            SetSecondary            ( unsigned char weather ) = 0;

    virtual void            ForceWeather            ( unsigned char weather ) = 0;
    virtual void            ForceWeatherNow         ( unsigned char weather ) = 0;
    virtual void            Release                 ( void ) = 0;

    virtual bool            IsRaining               ( void ) = 0;
    virtual float           GetAmountOfRain         ( void ) = 0;
    virtual void            SetAmountOfRain         ( float fAmount ) = 0;
    virtual void            ResetAmountOfRain       ( void ) = 0;
};

#endif
