/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CWorldRPCs.cpp
*  PURPOSE:     World remote procedure calls
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
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
    AddHandler ( SET_HEAT_HAZE, SetHeatHaze, "SetHeatHaze" );
    AddHandler ( RESET_HEAT_HAZE, ResetHeatHaze, "ResetHeatHaze" );
    AddHandler ( SET_BLUR_LEVEL, SetBlurLevel, "SetBlurLevel" );
    AddHandler ( SET_WANTED_LEVEL, SetWantedLevel, "SetWantedLevel" );
    AddHandler ( RESET_MAP_INFO, ResetMapInfo, "ResetMapInfo" );
    AddHandler ( SET_FPS_LIMIT, SetFPSLimit, "SetFPSLimit" );
    AddHandler ( SET_GARAGE_OPEN, SetGarageOpen, "SetGarageOpen" );
    AddHandler ( SET_GLITCH_ENABLED, SetGlitchEnabled, "SetGlitchEnabled" );
    AddHandler ( SET_CLOUDS_ENABLED, SetCloudsEnabled, "SetCloudsEnabled" );
    AddHandler ( SET_TRAFFIC_LIGHT_STATE, SetTrafficLightState, "SetTrafficLightState" );
    AddHandler ( SET_JETPACK_MAXHEIGHT, SetJetpackMaxHeight, "SetJetpackMaxHeight" );
    AddHandler ( SET_AIRCRAFT_MAXHEIGHT, SetAircraftMaxHeight, "SetAircraftMaxHeight" );

    AddHandler ( SET_INTERIOR_SOUNDS_ENABLED, SetInteriorSoundsEnabled, "SetInteriorSoundsEnabled" );
    AddHandler ( SET_RAIN_LEVEL, SetRainLevel, "SetRainLevel" );
    AddHandler ( SET_SUN_SIZE, SetSunSize, "SetSunSize" );
    AddHandler ( SET_SUN_COLOR, SetSunColor, "SetSunColor" );
    AddHandler ( SET_WIND_VELOCITY, SetWindVelocity, "SetWindVelocity" );
    AddHandler ( SET_FAR_CLIP_DISTANCE, SetFarClipDistance, "SetFarClipDistance" );
    AddHandler ( SET_FOG_DISTANCE, SetFogDistance, "SetFogDistance" );
    AddHandler ( RESET_RAIN_LEVEL, ResetRainLevel, "ResetRainLevel" );
    AddHandler ( RESET_SUN_SIZE, ResetSunSize, "ResetSunSize" );
    AddHandler ( RESET_SUN_COLOR, ResetSunColor, "ResetSunColor" );
    AddHandler ( RESET_WIND_VELOCITY, ResetWindVelocity, "ResetWindVelocity" );
    AddHandler ( RESET_FAR_CLIP_DISTANCE, ResetFarClipDistance, "ResetFarClipDistance" );
    AddHandler ( RESET_FOG_DISTANCE, ResetFogDistance, "ResetFogDistance" );
    AddHandler ( SET_WEAPON_PROPERTY, SetWeaponProperty, "SetWeaponProperty");
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


void CWorldRPCs::SetHeatHaze ( NetBitStreamInterface& bitStream )
{
    SHeatHazeSync heatHaze;
    if ( bitStream.Read ( &heatHaze ) )
    {
        g_pMultiplayer->SetHeatHaze ( heatHaze );
    }
}


void CWorldRPCs::ResetHeatHaze ( NetBitStreamInterface& bitStream )
{
    g_pMultiplayer->ResetHeatHaze ();
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
        m_pClientGame->SetWanted ( ucWantedLevel );
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
    g_pCore->RecalculateFrameRateLimit ( sFPSLimit );
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

void CWorldRPCs::SetTrafficLightState ( NetBitStreamInterface& bitStream )
{
    char ucTrafficLightState;

    if ( bitStream.ReadBits ( &ucTrafficLightState, 4 ) )
    {
       bool bForced = bitStream.ReadBit();
       // We ignore updating the serverside traffic light state if it's blocked, unless script forced it
        if ( bForced || !g_pMultiplayer->GetTrafficLightsLocked() )
            g_pMultiplayer->SetTrafficLightState ( (unsigned char)* &ucTrafficLightState );
    }
}

void CWorldRPCs::SetJetpackMaxHeight ( NetBitStreamInterface& bitStream )
{
    float fJetpackMaxHeight;

    if ( bitStream.Read ( fJetpackMaxHeight ) )
    {
        g_pGame->GetWorld ()->SetJetpackMaxHeight ( fJetpackMaxHeight );
    }
}

void CWorldRPCs::SetInteriorSoundsEnabled ( NetBitStreamInterface& bitStream )
{
    bool bEnable;

    if ( bitStream.ReadBit ( bEnable ) )
    {
        g_pMultiplayer->SetInteriorSoundsEnabled ( bEnable );
    }
}

void CWorldRPCs::SetRainLevel ( NetBitStreamInterface& bitStream )
{
    float fRainLevel;

    if ( bitStream.Read ( fRainLevel ) )
    {
        g_pGame->GetWeather ()->SetAmountOfRain ( fRainLevel );
    }
}

void CWorldRPCs::SetSunSize ( NetBitStreamInterface& bitStream )
{
    float fSunSize;

    if ( bitStream.Read ( fSunSize ) )
    {
        g_pMultiplayer->SetSunSize ( fSunSize );
    }
}

void CWorldRPCs::SetSunColor ( NetBitStreamInterface& bitStream )
{
    unsigned char ucCoreR, ucCoreG, ucCoreB;
    unsigned char ucCoronaR, ucCoronaG, ucCoronaB;

    if ( bitStream.Read ( ucCoreR ) && bitStream.Read ( ucCoreG ) && bitStream.Read ( ucCoreB )
        && bitStream.Read ( ucCoronaR ) && bitStream.Read ( ucCoronaG ) && bitStream.Read ( ucCoronaB ) )
    {
        g_pMultiplayer->SetSunColor ( ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB );
    }
}

void CWorldRPCs::SetWindVelocity ( NetBitStreamInterface& bitStream )
{
    float fVelX, fVelY, fVelZ;

    if ( bitStream.Read ( fVelX ) && bitStream.Read ( fVelY ) && bitStream.Read ( fVelZ ) )
    {
        g_pMultiplayer->SetWindVelocity ( fVelX, fVelY, fVelZ );
    }
}

void CWorldRPCs::SetFarClipDistance ( NetBitStreamInterface& bitStream )
{
    float fFarClip;

    if ( bitStream.Read ( fFarClip ) )
    {
        g_pMultiplayer->SetFarClipDistance ( fFarClip );
    }
}

void CWorldRPCs::SetFogDistance ( NetBitStreamInterface& bitStream )
{
    float fFogDist;

    if ( bitStream.Read ( fFogDist ) )
    {
        g_pMultiplayer->SetFogDistance ( fFogDist );
    }
}

void CWorldRPCs::SetAircraftMaxHeight ( NetBitStreamInterface& bitStream )
{
    float fMaxHeight;

    if ( bitStream.Read ( fMaxHeight ) )
    {
        g_pGame->GetWorld ()->SetAircraftMaxHeight ( fMaxHeight );
    }
}

void CWorldRPCs::ResetRainLevel ( NetBitStreamInterface& bitStream )
{
    g_pGame->GetWeather ()->ResetAmountOfRain ( );
}

void CWorldRPCs::ResetSunSize ( NetBitStreamInterface& bitStream )
{
    g_pMultiplayer->ResetSunSize ( );
}

void CWorldRPCs::ResetSunColor ( NetBitStreamInterface& bitStream )
{
    g_pMultiplayer->ResetSunColor ( );
}

void CWorldRPCs::ResetWindVelocity ( NetBitStreamInterface& bitStream )
{
    g_pMultiplayer->RestoreWindVelocity ( );
}

void CWorldRPCs::ResetFarClipDistance ( NetBitStreamInterface& bitStream )
{
    g_pMultiplayer->RestoreFarClipDistance ( );
}

void CWorldRPCs::ResetFogDistance ( NetBitStreamInterface& bitStream )
{
    g_pMultiplayer->RestoreFogDistance ( );
}

void CWorldRPCs::SetWeaponProperty ( NetBitStreamInterface& bitStream )
{
    unsigned char ucWeapon = 0;
    unsigned char ucProperty = 0;
    unsigned char ucWeaponSkill = 0;
    float fData = 0.0f;
    short sData = 0;
    if ( bitStream.Read ( ucWeapon ) && bitStream.Read ( ucProperty ) && bitStream.Read ( ucWeaponSkill ) )
    {
        CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetWeaponStats( static_cast < eWeaponType > ( ucWeapon ), static_cast < eWeaponSkill > ( ucWeaponSkill ) );
        switch ( ucProperty )
        {
            case WEAPON_WEAPON_RANGE:
                {
                    bitStream.Read ( fData );
                    pWeaponInfo->SetWeaponRange ( fData );
                    break;
                }
            case WEAPON_TARGET_RANGE:
                {
                    bitStream.Read ( fData );
                    pWeaponInfo->SetTargetRange ( fData );
                    break;
                }
            case WEAPON_ACCURACY:
                {
                    bitStream.Read ( fData );
                    pWeaponInfo->SetAccuracy ( fData );
                    break;
                }
            case WEAPON_LIFE_SPAN:
                {
                    bitStream.Read ( fData );
                    pWeaponInfo->SetLifeSpan ( fData );
                    break;
                }
            case WEAPON_FIRING_SPEED:
                {
                    bitStream.Read ( fData );
                    pWeaponInfo->SetFiringSpeed ( fData );
                    break;
                }
            case WEAPON_MOVE_SPEED:
                {
                    bitStream.Read ( fData );
                    pWeaponInfo->SetMoveSpeed ( fData );
                    break;
                }
            case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
                {
                    bitStream.Read ( fData );
                    pWeaponInfo->SetWeaponAnimLoopFireTime ( fData );
                    break;
                }
            case WEAPON_DAMAGE:
                {
                    bitStream.Read ( sData );
                    pWeaponInfo->SetDamagePerHit ( sData );
                    break;
                }
            case WEAPON_MAX_CLIP_AMMO:
                {
                    bitStream.Read ( sData );
                    pWeaponInfo->SetMaximumClipAmmo ( sData );
                    break;
                }
            case WEAPON_FLAGS:
                {
                    bitStream.Read ( sData );
                    if ( pWeaponInfo->IsFlagSet ( sData ) )
                        pWeaponInfo->ClearFlag ( sData );
                    else
                        pWeaponInfo->SetFlag ( sData );
                    break;
                }
            case WEAPON_ANIM_GROUP:
                {
                    bitStream.Read ( sData );
                    pWeaponInfo->SetAnimGroup ( sData );
                    break;
                }
        }
    }
}