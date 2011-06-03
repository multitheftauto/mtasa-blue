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

#ifndef __CMAPINFOPACKET_H
#define __CMAPINFOPACKET_H

#include "CPacket.h"

class CMapInfoPacket : public CPacket
{
public:
    explicit                CMapInfoPacket          ( unsigned char ucWeather,
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
                                                      float fWaterLevel,
                                                      bool bHasSkyGradient,
                                                      bool* pbGarageStates,
                                                      unsigned char ucSkyGradientTR,
                                                      unsigned char ucSkyGradientTG,
                                                      unsigned char ucSkyGradientTB,
                                                      unsigned char ucSkyGradientBR,
                                                      unsigned char ucSkyGradientBG,
                                                      unsigned char ucSkyGradientBB,
                                                      bool bHasHeatHaze,
                                                      const SHeatHazeSettings& heatHazeSettings,
                                                      unsigned short usFPSLimit = 36,
                                                      bool bCloudsEnabled = true,
                                                      float fJetpackMaxHeight = 100,
                                                      bool bOverrideWaterColor = false,
                                                      unsigned char ucWaterRed = 0,
                                                      unsigned char ucWaterGreen = 0,
                                                      unsigned char ucWaterBlue = 0,
                                                      unsigned char ucWaterAlpha = 0,
                                                      bool bInteriorSoundsEnabled = true,
                                                      bool bOverrideRainLevel = false,
                                                      float fRainLevel = 0,
                                                      bool bOverrideSunSize = false,
                                                      float fSunSize = 0,
                                                      bool bOverrideSunColor = false,
                                                      unsigned char ucSunCoreR = 0,
                                                      unsigned char ucSunCoreG = 0,
                                                      unsigned char ucSunCoreB = 0,
                                                      unsigned char ucSunCoronaR = 0,
                                                      unsigned char ucSunCoronaG = 0,
                                                      unsigned char ucSunCoronaB = 0,
                                                      bool bOverrideWindVelocity = false,
                                                      float fWindVelX = 0,
                                                      float fWindVelY = 0,
                                                      float fWindVelZ = 0,
                                                      bool bOverrideFarClipDistance = false,
                                                      float fFarClip = 0,
                                                      bool bOverrideFogDistance = false,
                                                      float fFogDistance = 0);

    inline ePacketID        GetPacketID             ( void ) const              { return PACKET_ID_MAP_INFO; };
    inline unsigned long    GetFlags                ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                   ( NetBitStreamInterface& BitStream ) const;

private:
    unsigned char           m_ucWeather;
    unsigned char           m_ucWeatherBlendingTo;
    unsigned char           m_ucBlendedWeatherHour;
    unsigned char           m_ucClockHour;
    unsigned char           m_ucClockMin;
    unsigned long           m_ulMinuteDuration;
    bool                    m_bShowNametags;
    bool                    m_bShowRadar;
    float                   m_fGravity;
    float                   m_fGameSpeed;
    float                   m_fWaveHeight;
    float                   m_fWaterLevel;
    bool                    m_bHasSkyGradient;
    unsigned char           m_ucSkyGradientTR, m_ucSkyGradientTG, m_ucSkyGradientTB;
    unsigned char           m_ucSkyGradientBR, m_ucSkyGradientBG, m_ucSkyGradientBB;
    bool                    m_bHasHeatHaze;
    SHeatHazeSettings       m_HeatHazeSettings;
    unsigned short          m_usFPSLimit;
    bool*                   m_pbGarageStates;
    bool                    m_bCloudsEnabled;
    float                   m_fJetpackMaxHeight;
    bool                    m_bOverrideWaterColor;
    unsigned char           m_ucWaterRed;
    unsigned char           m_ucWaterGreen;
    unsigned char           m_ucWaterBlue;
    unsigned char           m_ucWaterAlpha;
    bool                    m_bInteriorSoundsEnabled;
    bool                    m_bOverrideRainLevel;
    float                   m_fRainLevel;
    bool                    m_bOverrideSunSize;
    float                   m_fSunSize;
    bool                    m_bOverrideSunColor;
    unsigned char           m_ucSunCoreR;
    unsigned char           m_ucSunCoreG;
    unsigned char           m_ucSunCoreB;
    unsigned char           m_ucSunCoronaR;
    unsigned char           m_ucSunCoronaG;
    unsigned char           m_ucSunCoronaB;
    bool                    m_bOverrideWindVelocity;
    float                   m_fWindVelX;
    float                   m_fWindVelY;
    float                   m_fWindVelZ;
    bool                    m_bOverrideFarClipDistance;
    float                   m_fFarClip;
    bool                    m_bOverrideFogDistance;
    float                   m_fFogDistance;
};

#endif
