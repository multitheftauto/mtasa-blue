/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CMapInfoPacket.h
*  PURPOSE:     Map/game information packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
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
                                 const SWorldWaterLevelInfo& worldWaterLevelInfo,
                                 bool bHasSkyGradient,
                                 const SGarageStates& garageStates,
                                 unsigned char ucSkyGradientTR,
                                 unsigned char ucSkyGradientTG,
                                 unsigned char ucSkyGradientTB,
                                 unsigned char ucSkyGradientBR,
                                 unsigned char ucSkyGradientBG,
                                 unsigned char ucSkyGradientBB,
                                 bool bHasHeatHaze,
                                 const SHeatHazeSettings& heatHazeSettings,
                                 unsigned short usFPSLimit,
                                 bool bCloudsEnabled,
                                 float fJetpackMaxHeight,
                                 bool bOverrideWaterColor,
                                 unsigned char ucWaterRed,
                                 unsigned char ucWaterGreen,
                                 unsigned char ucWaterBlue,
                                 unsigned char ucWaterAlpha,
                                 bool bInteriorSoundsEnabled,
                                 bool bOverrideRainLevel,
                                 float fRainLevel,
                                 bool bOverrideSunSize,
                                 float fSunSize,
                                 bool bOverrideSunColor,
                                 unsigned char ucSunCoreR,
                                 unsigned char ucSunCoreG,
                                 unsigned char ucSunCoreB,
                                 unsigned char ucSunCoronaR,
                                 unsigned char ucSunCoronaG,
                                 unsigned char ucSunCoronaB,
                                 bool bOverrideWindVelocity,
                                 float fWindVelX,
                                 float fWindVelY,
                                 float fWindVelZ,
                                 bool bOverrideFarClipDistance,
                                 float fFarClip,
                                 bool bOverrideFogDistance,
                                 float fFogDistance,
                                 float fAircraftMaxHeight )
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
    m_WorldWaterLevelInfo = worldWaterLevelInfo;
    m_bHasSkyGradient = bHasSkyGradient;
    m_pGarageStates = &garageStates;
    m_ucSkyGradientTR = ucSkyGradientTR;
    m_ucSkyGradientTG = ucSkyGradientTG;
    m_ucSkyGradientTB = ucSkyGradientTB;
    m_ucSkyGradientBR = ucSkyGradientBR;
    m_ucSkyGradientBG = ucSkyGradientBG;
    m_ucSkyGradientBB = ucSkyGradientBB;
    m_bHasHeatHaze = bHasHeatHaze;
    m_HeatHazeSettings = heatHazeSettings;
    m_usFPSLimit = usFPSLimit;
    m_bCloudsEnabled = bCloudsEnabled;
    m_fJetpackMaxHeight = fJetpackMaxHeight;
    m_bOverrideWaterColor = bOverrideWaterColor;
    m_ucWaterRed = ucWaterRed;
    m_ucWaterGreen = ucWaterGreen;
    m_ucWaterBlue = ucWaterBlue;
    m_ucWaterAlpha = ucWaterAlpha;
    m_bInteriorSoundsEnabled = bInteriorSoundsEnabled;
    m_bOverrideRainLevel = bOverrideRainLevel;
    m_fRainLevel = fRainLevel;
    m_bOverrideSunSize = bOverrideSunSize;
    m_fSunSize = fSunSize;
    m_bOverrideSunColor = bOverrideSunColor;
    m_ucSunCoreR = ucSunCoreR;
    m_ucSunCoreG = ucSunCoreG;
    m_ucSunCoreB = ucSunCoreB;
    m_ucSunCoronaR = ucSunCoronaR;
    m_ucSunCoronaG = ucSunCoronaG;
    m_ucSunCoronaB = ucSunCoronaB;
    m_bOverrideWindVelocity = bOverrideWindVelocity;
    m_fWindVelX = fWindVelX;
    m_fWindVelY = fWindVelY;
    m_fWindVelZ = fWindVelZ;
    m_bOverrideFarClipDistance = bOverrideFarClipDistance;
    m_fFarClip = fFarClip;
    m_bOverrideFogDistance = bOverrideFogDistance;
    m_fFogDistance = fFogDistance;
    m_fAircraftMaxHeight = fAircraftMaxHeight;
}


bool CMapInfoPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the map weather
    BitStream.Write ( m_ucWeather );
    BitStream.Write ( m_ucWeatherBlendingTo );
    BitStream.Write ( m_ucBlendedWeatherHour );

    BitStream.WriteBit ( m_bHasSkyGradient );
    if ( m_bHasSkyGradient )
    {
        BitStream.Write ( m_ucSkyGradientTR );
        BitStream.Write ( m_ucSkyGradientTG );
        BitStream.Write ( m_ucSkyGradientTB );
        BitStream.Write ( m_ucSkyGradientBR );
        BitStream.Write ( m_ucSkyGradientBG );
        BitStream.Write ( m_ucSkyGradientBB );
    }

    // Write heat haze
    BitStream.WriteBit ( m_bHasHeatHaze );
    if ( m_bHasHeatHaze )
    {
        SHeatHazeSync heatHaze ( m_HeatHazeSettings );
        BitStream.Write ( &heatHaze );
    }

    // Write the map hour
    BitStream.Write ( m_ucClockHour );
    BitStream.Write ( m_ucClockMin );

    BitStream.WriteCompressed ( m_ulMinuteDuration );

    // Write the map flags
    SMapInfoFlagsSync flags;
    flags.data.bShowNametags  = m_bShowNametags;
    flags.data.bShowRadar     = m_bShowRadar;
    flags.data.bCloudsEnabled = m_bCloudsEnabled;
    BitStream.Write ( &flags );

    // Write any other world conditions
    BitStream.Write ( m_fGravity );
    if ( m_fGameSpeed == 1.0f )
        BitStream.WriteBit ( true );
    else
    {
        BitStream.WriteBit ( false );
        BitStream.Write ( m_fGameSpeed );
    }
    BitStream.Write ( m_fWaveHeight );
    // Write world water level
    BitStream.Write ( m_WorldWaterLevelInfo.fSeaLevel );
    BitStream.WriteBit ( m_WorldWaterLevelInfo.bNonSeaLevelSet );
    if ( m_WorldWaterLevelInfo.bNonSeaLevelSet )
        BitStream.Write ( m_WorldWaterLevelInfo.fNonSeaLevel );

    BitStream.WriteCompressed ( m_usFPSLimit );

    // Write the garage states
    for ( unsigned char i = 0 ; i < MAX_GARAGES ; i++ )
    {
        const SGarageStates& garageStates = *m_pGarageStates;
        BitStream.WriteBit( garageStates[i] );
    }

    // Write the fun bugs state
    SFunBugsStateSync funBugs;
    funBugs.data.bQuickReload = g_pGame->IsGlitchEnabled ( CGame::GLITCH_QUICKRELOAD );
    funBugs.data.bFastFire    = g_pGame->IsGlitchEnabled ( CGame::GLITCH_FASTFIRE );
    funBugs.data.bFastMove    = g_pGame->IsGlitchEnabled ( CGame::GLITCH_FASTMOVE );
    funBugs.data.bCrouchBug   = g_pGame->IsGlitchEnabled ( CGame::GLITCH_CROUCHBUG );
    funBugs.data.bCloseRangeDamage = g_pGame->IsGlitchEnabled ( CGame::GLITCH_CLOSEDAMAGE );
    BitStream.Write ( &funBugs );

    BitStream.Write ( m_fJetpackMaxHeight );

    BitStream.WriteBit ( m_bOverrideWaterColor );
    if ( m_bOverrideWaterColor )
    {
        BitStream.Write ( m_ucWaterRed );
        BitStream.Write ( m_ucWaterGreen );
        BitStream.Write ( m_ucWaterBlue );
        BitStream.Write ( m_ucWaterAlpha );
    }

    // Interior sounds
    BitStream.WriteBit ( m_bInteriorSoundsEnabled );

    // Rain level
    BitStream.WriteBit ( m_bOverrideRainLevel );
    if ( m_bOverrideRainLevel )
    {
        BitStream.Write ( m_fRainLevel );
    }

    // Sun size
    BitStream.WriteBit ( m_bOverrideSunSize );
    if ( m_bOverrideSunSize )
    {
        BitStream.Write ( m_fSunSize );
    }

    // Sun color
    BitStream.WriteBit ( m_bOverrideSunColor );
    if ( m_bOverrideSunColor )
    {
        BitStream.Write ( m_ucSunCoreR );
        BitStream.Write ( m_ucSunCoreG );
        BitStream.Write ( m_ucSunCoreB );
        BitStream.Write ( m_ucSunCoronaR );
        BitStream.Write ( m_ucSunCoronaG );
        BitStream.Write ( m_ucSunCoronaB );
    }

    // Wind velocity
    BitStream.WriteBit ( m_bOverrideWindVelocity );
    if ( m_bOverrideWindVelocity )
    {
        BitStream.Write ( m_fWindVelX );
        BitStream.Write ( m_fWindVelY );
        BitStream.Write ( m_fWindVelZ );
    }

    // Far clip distance
    BitStream.WriteBit ( m_bOverrideFarClipDistance );
    if ( m_bOverrideFarClipDistance )
    {
        BitStream.Write ( m_fFarClip );
    }

    // Fog distance
    BitStream.WriteBit ( m_bOverrideFogDistance );
    if ( m_bOverrideFogDistance )
    {
        BitStream.Write ( m_fFogDistance );
    }

    BitStream.Write ( m_fAircraftMaxHeight );

    if ( BitStream.Version () >= 0x30 )
    {
        for (int i = WEAPONTYPE_BRASSKNUCKLE; i <= WEAPONTYPE_PISTOL; i++)
        {
            bool bEnabled;
            bEnabled = g_pGame->GetJetpackWeaponEnabled ( (eWeaponType) i );
            BitStream.WriteBit ( bEnabled );
        }
    }
    for (int i = WEAPONTYPE_PISTOL;i <= WEAPONTYPE_EXTINGUISHER;i++)
    {
        sWeaponPropertySync WeaponProperty;
        CWeaponStat* pWeaponStat = g_pGame->GetWeaponStatManager ()->GetWeaponStats( (eWeaponType)i );
        BitStream.WriteBit ( true );
        WeaponProperty.data.weaponType = (int)pWeaponStat->GetWeaponType();
        WeaponProperty.data.fAccuracy = pWeaponStat->GetAccuracy();
        WeaponProperty.data.fMoveSpeed = pWeaponStat->GetMoveSpeed();
        WeaponProperty.data.fTargetRange = pWeaponStat->GetTargetRange();
        WeaponProperty.data.fWeaponRange = pWeaponStat->GetWeaponRange();
        WeaponProperty.data.nAmmo = pWeaponStat->GetMaximumClipAmmo();
        WeaponProperty.data.nDamage = pWeaponStat->GetDamagePerHit();
        WeaponProperty.data.nFlags = pWeaponStat->GetFlags();

        WeaponProperty.data.anim_loop_start = pWeaponStat->GetWeaponAnimLoopStart();
        WeaponProperty.data.anim_loop_stop = pWeaponStat->GetWeaponAnimLoopStop();
        WeaponProperty.data.anim_loop_bullet_fire = pWeaponStat->GetWeaponAnimLoopFireTime();

        WeaponProperty.data.anim2_loop_start = pWeaponStat->GetWeaponAnim2LoopStart();
        WeaponProperty.data.anim2_loop_stop = pWeaponStat->GetWeaponAnim2LoopStop();
        WeaponProperty.data.anim2_loop_bullet_fire = pWeaponStat->GetWeaponAnim2LoopFireTime();

        WeaponProperty.data.anim_breakout_time = pWeaponStat->GetWeaponAnimBreakoutTime();
        BitStream.Write( &WeaponProperty );
        if ( BitStream.Version () >= 0x30 )
        {
            BitStream.WriteBit ( g_pGame->GetJetpackWeaponEnabled ( (eWeaponType) i ) );
        }
    }

    for (int i = WEAPONTYPE_PISTOL;i <= WEAPONTYPE_TEC9;i++)
    {
        sWeaponPropertySync WeaponProperty;
        BitStream.WriteBit ( true );
        for (int j = 0; j <= 2;j++)
        {
            CWeaponStat* pWeaponStat = g_pGame->GetWeaponStatManager ()->GetWeaponStats( (eWeaponType)i, (eWeaponSkill)j );
            WeaponProperty.data.weaponType = (int)pWeaponStat->GetWeaponType();
            WeaponProperty.data.fAccuracy = pWeaponStat->GetAccuracy();
            WeaponProperty.data.fMoveSpeed = pWeaponStat->GetMoveSpeed();
            WeaponProperty.data.fTargetRange = pWeaponStat->GetTargetRange();
            WeaponProperty.data.fWeaponRange = pWeaponStat->GetWeaponRange();
            WeaponProperty.data.nAmmo = pWeaponStat->GetMaximumClipAmmo();
            WeaponProperty.data.nDamage = pWeaponStat->GetDamagePerHit();
            WeaponProperty.data.nFlags = pWeaponStat->GetFlags();

            WeaponProperty.data.anim_loop_start = pWeaponStat->GetWeaponAnimLoopStart();
            WeaponProperty.data.anim_loop_stop = pWeaponStat->GetWeaponAnimLoopStop();
            WeaponProperty.data.anim_loop_bullet_fire = pWeaponStat->GetWeaponAnimLoopFireTime();

            WeaponProperty.data.anim2_loop_start = pWeaponStat->GetWeaponAnim2LoopStart();
            WeaponProperty.data.anim2_loop_stop = pWeaponStat->GetWeaponAnim2LoopStop();
            WeaponProperty.data.anim2_loop_bullet_fire = pWeaponStat->GetWeaponAnim2LoopFireTime();

            WeaponProperty.data.anim_breakout_time = pWeaponStat->GetWeaponAnimBreakoutTime();
            BitStream.Write( &WeaponProperty );
        }
        if ( BitStream.Version () >= 0x30 )
        {
            BitStream.WriteBit ( g_pGame->GetJetpackWeaponEnabled ( (eWeaponType) i ) );
        }
    }
    if ( BitStream.Version () >= 0x30 )
    {
        for (int i = WEAPONTYPE_CAMERA; i <= WEAPONTYPE_PARACHUTE; i++)
        {
            bool bEnabled;
            bEnabled = g_pGame->GetJetpackWeaponEnabled ( (eWeaponType) i );
            BitStream.WriteBit ( bEnabled );
        }
    }

    multimap< unsigned short, CBuildingRemoval* >::const_iterator iter = g_pGame->GetBuildingRemovalManager ( )->IterBegin();
    for (; iter != g_pGame->GetBuildingRemovalManager ( )->IterEnd();++iter)
    {
        CBuildingRemoval * pBuildingRemoval = (*iter).second;
        BitStream.WriteBit( true );
        BitStream.Write( pBuildingRemoval->GetModel ( ) );
        BitStream.Write( pBuildingRemoval->GetRadius ( ) );
        BitStream.Write( pBuildingRemoval->GetPosition ( ).fX );
        BitStream.Write( pBuildingRemoval->GetPosition ( ).fY );
        BitStream.Write( pBuildingRemoval->GetPosition ( ).fZ );
    }
    BitStream.WriteBit( false );

    if ( BitStream.Version () >= 0x25 )
    {
        bool bOcclusionsEnabled = g_pGame->GetOcclusionsEnabled ();
        BitStream.WriteBit( bOcclusionsEnabled );
    }

    return true;
}
