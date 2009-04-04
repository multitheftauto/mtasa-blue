/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CMapInfoPacket.h
*  PURPOSE:     Map/game information packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*               Alberto Alonso <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CMapInfoPacket::CMapInfoPacket ( unsigned char ucWeather,
                                 unsigned char ucWeatherBlendingTo,
                                 unsigned char ucBlendedWeatherHour,
                                 unsigned char ucClockHour,
                                 unsigned char ucClockMin,
                                 unsigned long ulMinuteDuration,
                                 bool bShowNametags,
                                 bool bShowRadar,
                                 float fGravity,
                                 float fGameSpeed,
                                 float fWaveHeight,
                                 bool bHasSkyGradient,
                                 bool* pbGarageStates,
                                 unsigned char ucSkyGradientTR,
                                 unsigned char ucSkyGradientTG,
                                 unsigned char ucSkyGradientTB,
                                 unsigned char ucSkyGradientBR,
                                 unsigned char ucSkyGradientBG,
                                 unsigned char ucSkyGradientBB,
								 unsigned short usFPSLimit )
{
    m_ucWeather = ucWeather;
    m_ucWeatherBlendingTo = ucWeatherBlendingTo;
    m_ucBlendedWeatherHour = ucBlendedWeatherHour;
    m_ucClockHour = ucClockHour;
    m_ucClockMin = ucClockMin;
    m_ulMinuteDuration = ulMinuteDuration;
    m_bShowNametags = bShowNametags;
    m_bShowRadar = bShowRadar;
    m_fGravity = fGravity;
    m_fGameSpeed = fGameSpeed;
    m_fWaveHeight = fWaveHeight;
    m_bHasSkyGradient = bHasSkyGradient;
    m_pbGarageStates = pbGarageStates;
    m_ucSkyGradientTR = ucSkyGradientTR;
    m_ucSkyGradientTG = ucSkyGradientTG;
    m_ucSkyGradientTB = ucSkyGradientTB;
    m_ucSkyGradientBR = ucSkyGradientBR;
    m_ucSkyGradientBG = ucSkyGradientBG;
    m_ucSkyGradientBB = ucSkyGradientBB;
	m_usFPSLimit = usFPSLimit;
}


bool CMapInfoPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Write the map weather
    BitStream.Write ( m_ucWeather );
    BitStream.Write ( m_ucWeatherBlendingTo );
    BitStream.Write ( m_ucBlendedWeatherHour );

    BitStream.Write ( ( unsigned char ) ( ( m_bHasSkyGradient ) ? 1 : 0 ) );
    if ( m_bHasSkyGradient )
    {
        BitStream.Write ( m_ucSkyGradientTR );
        BitStream.Write ( m_ucSkyGradientTG );
        BitStream.Write ( m_ucSkyGradientTB );
        BitStream.Write ( m_ucSkyGradientBR );
        BitStream.Write ( m_ucSkyGradientBG );
        BitStream.Write ( m_ucSkyGradientBB );
    }

    // Write the map hour
    BitStream.Write ( m_ucClockHour );
    BitStream.Write ( m_ucClockMin );

    BitStream.Write ( m_ulMinuteDuration );

    // Gather the map flags
    unsigned char ucFlags = 0;
    ucFlags |= m_bShowNametags ? 1:0;
    ucFlags |= m_bShowRadar << 1;

    // Write the map flags
    BitStream.Write ( ucFlags );

    // Write any other world conditions
    BitStream.Write ( m_fGravity );
    BitStream.Write ( m_fGameSpeed );
    BitStream.Write ( m_fWaveHeight );

	BitStream.Write ( m_usFPSLimit );

    // Write the garage states
    for ( unsigned char i = 0 ; i < MAX_GARAGES ; i++ )
    {
        BitStream.Write( (unsigned char)m_pbGarageStates[i] );
    }

    return true;
}
