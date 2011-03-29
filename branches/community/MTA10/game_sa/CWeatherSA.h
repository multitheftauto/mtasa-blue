/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeatherSA.h
*  PURPOSE:     Header file for weather class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_WEATHER
#define __CGAMESA_WEATHER

#include <game/CWeather.h>
#include "Common.h"

class CWeatherSA : public CWeather
{
    friend class COffsets;

public:
    unsigned char                    Get                     ( void );
    void                        Set                     ( unsigned char primary, unsigned char secondary );
    void                        SetPrimary              ( unsigned char weather );
    void                        SetSecondary            ( unsigned char weather );

    void                        ForceWeather            ( unsigned char weather );
    void                        ForceWeatherNow         ( unsigned char weather );
    void                        Release                 ( void );

    bool                        IsRaining               ( void );
    float                       GetAmountOfRain         ( void );
    void                        SetAmountOfRain         ( float fAmount );
    void                        ResetAmountOfRain       ( void );

private:
    static unsigned char*       VAR_CurrentWeather;
    static unsigned char*       VAR_CurrentWeather_b;
    static unsigned char*       VAR_CurrentWeather_c;
    static float*               VAR_AmountOfRain;

    static unsigned long        FUNC_IsRaining;
};

#endif
