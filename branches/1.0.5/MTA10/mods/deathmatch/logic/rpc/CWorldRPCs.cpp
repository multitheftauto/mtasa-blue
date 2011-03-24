/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CWorldRPCs.cpp
*  PURPOSE:     World remote procedure calls
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CWorldRPCs.h"

void CWorldRPCs::LoadFunctions ( void )
{
    AddHandler ( SET_TIME, SetTime, "SetTime" );
    AddHandler ( SET_WEATHER, SetWeather, "SetWeather" );
    AddHandler ( SET_WEATHER_BLENDED, SetWeatherBlended, "SetWeatherBlended" );
    AddHandler ( SET_MINUTE_DURATION, SetMinuteDuration, "SetMinuteDuration" );
    AddHandler ( SET_GRAVITY, SetGravity, "SetGravity" );
    AddHandler ( SET_GAME_SPEED, SetGameSpeed, "SetGameSpeed" );
    AddHandler ( SET_WAVE_HEIGHT, SetWaveHeight, "SetWaveHeight" );
    AddHandler ( SET_SKY_GRADIENT, SetSkyGradient, "SetSkyGradient" );
    AddHandler ( RESET_SKY_GRADIENT, ResetSkyGradient, "ResetSkyGradient" );
    AddHandler ( SET_BLUR_LEVEL, SetBlurLevel, "SetBlurLevel" );
    AddHandler ( SET_WANTED_LEVEL, SetWantedLevel, "SetWantedLevel" );
    AddHandler ( RESET_MAP_INFO, ResetMapInfo, "ResetMapInfo" );
    AddHandler ( SET_FPS_LIMIT, SetFPSLimit, "SetFPSLimit" );
    AddHandler ( SET_GARAGE_OPEN, SetGarageOpen, "SetGarageOpen" );
    AddHandler ( SET_GLITCH_ENABLED, SetGlitchEnabled, "SetGlitchEnabled" );
    AddHandler ( SET_CLOUDS_ENABLED, SetCloudsEnabled, "SetCloudsEnabled" );
}


void CWorldRPCs::SetTime ( NetBitStreamInterface& bitStream )
{
    // Read out the time
    unsigned char ucHour;
    unsigned char ucMin;
    if ( bitStream.Read ( ucHour ) &&
         bitStream.Read ( ucMin ) )
    {
        // Check that its within range
        if ( ucHour < 24 && ucMin < 60 )
        {
            g_pGame->GetClock ()->Set ( ucHour, ucMin );
        }
    }
}


void CWorldRPCs::SetWeather ( NetBitStreamInterface& bitStream )
{
    // Read out the weather to apply
    unsigned char ucWeather;
    if ( bitStream.Read ( ucWeather ) )
    {
        // Check that its within range
        if ( ucWeather <= MAX_VALID_WEATHER )
        {
            m_pBlendedWeather->SetWeather ( ucWeather );
        }
    }
}


void CWorldRPCs::SetWeatherBlended ( NetBitStreamInterface& bitStream )
{
    // Read out the weather to apply and the hour it should begin applying at
    unsigned char ucWeather;
    unsigned char ucHour;
    if ( bitStream.Read ( ucWeather ) &&
         bitStream.Read ( ucHour ) )
    {
        // Check that its within range
        if ( ucWeather <= MAX_VALID_WEATHER )
        {
            m_pBlendedWeather->SetWeatherBlended ( ucWeather, ucHour );
        }
    }
}


void CWorldRPCs::SetMinuteDuration ( NetBitStreamInterface& bitStream )
{
    unsigned long ulDuration;
    if ( bitStream.Read ( ulDuration ) )
    {
        m_pClientGame->SetMinuteDuration ( ulDuration );
    }
}


void CWorldRPCs::SetGravity ( NetBitStreamInterface& bitStream )
{
    float fGravity;
    if ( bitStream.Read ( fGravity ) )
    {
        g_pMultiplayer->SetGlobalGravity ( fGravity );
    }
}


void CWorldRPCs::SetGameSpeed ( NetBitStreamInterface& bitStream )
{
    float fSpeed;
    if ( bitStream.Read ( fSpeed ) )
    {
        m_pClientGame->SetGameSpeed ( fSpeed );
    }
}


void CWorldRPCs::SetWaveHeight ( NetBitStreamInterface& bitStream )
{
    float fHeight;
    if ( bitStream.Read ( fHeight ) )
    {
        g_pGame->GetWaterManager ()->SetWaveLevel ( fHeight );
    }
}


void CWorldRPCs::SetSkyGradient ( NetBitStreamInterface& bitStream )
{
    unsigned char ucTopRed = 0, ucTopGreen = 0, ucTopBlue = 0;
    unsigned char ucBottomRed = 0, ucBottomGreen = 0, ucBottomBlue = 0;
    if ( bitStream.Read ( ucTopRed ) && bitStream.Read ( ucTopGreen ) &&
         bitStream.Read ( ucTopBlue ) && bitStream.Read ( ucBottomRed ) &&
         bitStream.Read ( ucBottomGreen ) && bitStream.Read ( ucBottomBlue ) )
    {
        g_pMultiplayer->SetSkyColor ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue );
    }
}


void CWorldRPCs::ResetSkyGradient ( NetBitStreamInterface& bitStream )
{
    g_pMultiplayer->ResetSky ();
}


void CWorldRPCs::SetBlurLevel ( NetBitStreamInterface& bitStream )
{
    unsigned char ucLevel;
    if ( bitStream.Read ( ucLevel ) )
    {
        g_pGame->SetBlurLevel ( ucLevel );
    }
}


void CWorldRPCs::SetWantedLevel ( NetBitStreamInterface& bitStream )
{
    unsigned char ucWantedLevel;

    if ( bitStream.Read ( ucWantedLevel ) )
    {
        g_pGame->GetPlayerInfo()->GetWanted()->SetWantedLevel ( ucWantedLevel );
    }
}


void CWorldRPCs::ResetMapInfo ( NetBitStreamInterface& bitStream )
{
    m_pClientGame->ResetMapInfo ();
}


void CWorldRPCs::SetFPSLimit ( NetBitStreamInterface& bitStream )
{
    short sFPSLimit;
    bitStream.Read ( sFPSLimit );

    int iVal;
    g_pCore->GetCVars ()->Get ( "fps_limit", iVal );

    if ( sFPSLimit > 0 && iVal > sFPSLimit || iVal == 0 )
        // For some reason it needs that kind of hacky precision
        g_pGame->SetFramelimiter ( (unsigned long) ( (float)sFPSLimit * 1.333f ) );
    else
        g_pGame->SetFramelimiter ( (unsigned long) ( (float)iVal * 1.3f ) );

}

void CWorldRPCs::SetGarageOpen ( NetBitStreamInterface& bitStream )
{
    unsigned char ucGarageID;
    unsigned char ucIsOpen;

    if ( bitStream.Read ( ucGarageID ) && bitStream.Read ( ucIsOpen ) )
    {
        CGarage* pGarage = g_pCore->GetGame()->GetGarages()->GetGarage ( ucGarageID );
        if ( pGarage )
        {
            pGarage->SetOpen ( ( ucIsOpen == 1 ) );
        }
    }
}

void CWorldRPCs::SetGlitchEnabled ( NetBitStreamInterface& bitStream )
{
    unsigned char eGlitch;
    unsigned char ucIsEnabled;
    bitStream.Read ( eGlitch );
    bitStream.Read ( ucIsEnabled );
    g_pClientGame->SetGlitchEnabled ( eGlitch, ( ucIsEnabled == 1 ) );
}

void CWorldRPCs::SetCloudsEnabled ( NetBitStreamInterface& bitStream )
{
    unsigned char ucIsEnabled;
    bitStream.Read ( ucIsEnabled );
    bool bEnabled = (ucIsEnabled == 1);
    g_pMultiplayer->SetCloudsEnabled ( bEnabled );
    g_pClientGame->SetCloudsEnabled( bEnabled );
}
