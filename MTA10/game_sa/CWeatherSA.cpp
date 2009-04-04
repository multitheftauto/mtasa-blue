/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeatherSA.cpp
*  PURPOSE:     Weather logic
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

unsigned char* CWeatherSA::VAR_CurrentWeather;
unsigned char* CWeatherSA::VAR_CurrentWeather_b;
unsigned char* CWeatherSA::VAR_CurrentWeather_c;
float* CWeatherSA::VAR_AmountOfRain;

unsigned long CWeatherSA::FUNC_IsRaining;

unsigned char CWeatherSA::Get ( void )
{
	DEBUG_TRACE("unsigned char CWeatherSA::Get ( void )");
	return (unsigned char)*VAR_CurrentWeather;
}

void CWeatherSA::Set ( unsigned char primary, unsigned char secondary )
{
    DEBUG_TRACE("void CWeatherSA::Set ( unsigned char primary, unsigned char secondary )");
    *VAR_CurrentWeather_b = static_cast < unsigned char > ( primary );
    *VAR_CurrentWeather_c = static_cast < unsigned char > ( secondary );
}

void CWeatherSA::SetPrimary ( unsigned char weather )
{
    DEBUG_TRACE("void CWeatherSA::SetPrimary ( unsigned char weather )");
    *VAR_CurrentWeather_b = static_cast < unsigned char > ( weather );
}

void CWeatherSA::SetSecondary ( unsigned char weather )
{
    DEBUG_TRACE("void CWeatherSA::SetSecondary ( unsigned char weather )");
    *VAR_CurrentWeather_c = static_cast < unsigned char > ( weather );
}

void CWeatherSA::ForceWeather ( unsigned char weather )
{
	DEBUG_TRACE("void CWeatherSA::Forcunsigned char ( unsigned char weather )");
	*VAR_CurrentWeather = static_cast < unsigned char > ( weather );
}

void CWeatherSA::ForceWeatherNow ( unsigned char weather )
{
	DEBUG_TRACE("void CWeatherSA::Forcunsigned charNow ( unsigned char weather )");

    unsigned char ucWeather = static_cast < unsigned char > ( weather );
	*VAR_CurrentWeather = ucWeather;
	*VAR_CurrentWeather_b = ucWeather;
	*VAR_CurrentWeather_c = ucWeather;
}

void CWeatherSA::Release ( void )
{
	DEBUG_TRACE("void CWeatherSA::Release ( void )");
	*VAR_CurrentWeather = 0xFF;	
}

bool CWeatherSA::IsRaining ( void )
{
	DEBUG_TRACE("bool CWeatherSA::IsRaining ( void )");
	DWORD dwFunc = FUNC_IsRaining;
	bool bReturn = false;
	_asm
	{
		call	dwFunc
		mov		bReturn, al
	}
	return bReturn;
}

float CWeatherSA::GetAmountOfRain ( void )
{
    return *VAR_AmountOfRain;
}

void CWeatherSA::SetAmountOfRain ( float fAmount )
{
    *VAR_AmountOfRain = fAmount;
}
