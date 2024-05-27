/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CMapInfoPacket.h
 *  PURPOSE:     Map/game information packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

typedef SFixedArray<bool, MAX_GARAGES> SGarageStates;

struct SWorldWaterLevelInfo
{
    bool  bNonSeaLevelSet;
    bool  bOutsideLevelSet;
    float fSeaLevel;
    float fNonSeaLevel;
    float fOutsideLevel;
};

class CMapInfoPacket final : public CPacket
{
public:
    explicit CMapInfoPacket(std::uint8_t ucWeather, std::uint8_t ucWeatherBlendingTo,
        std::uint8_t ucBlendedWeatherHour, std::uint8_t ucClockHour,
        std::uint8_t ucClockMin, unsigned long ulMinuteDuration, bool bShowNametags,
        bool bShowRadar, float fGravity, float fGameSpeed, float fWaveHeight,
        const SWorldWaterLevelInfo& worldWaterLevelInfo, bool bHasSkyGradient,
        const SGarageStates& garageStates, std::uint8_t ucSkyGradientTR,
        std::uint8_t ucSkyGradientTG, std::uint8_t ucSkyGradientTB,
        std::uint8_t ucSkyGradientBR, std::uint8_t ucSkyGradientBG,
        std::uint8_t ucSkyGradientBB, bool bHasHeatHaze,
        const SHeatHazeSettings& heatHazeSettings, std::uint16_t usFPSLimit = 36,
        bool bCloudsEnabled = true, float fJetpackMaxHeight = 100,
        bool bOverrideWaterColor = false, std::uint8_t ucWaterRed = 0,
        std::uint8_t ucWaterGreen = 0, std::uint8_t ucWaterBlue = 0,
        std::uint8_t ucWaterAlpha = 0, bool bInteriorSoundsEnabled = true,
        bool bOverrideRainLevel = false, float fRainLevel = 0, bool bOverrideSunSize = false,
        float fSunSize = 0, bool bOverrideSunColor = false, std::uint8_t ucSunCoreR = 0,
        std::uint8_t ucSunCoreG = 0, std::uint8_t ucSunCoreB = 0,
        std::uint8_t ucSunCoronaR = 0, std::uint8_t ucSunCoronaG = 0,
        std::uint8_t ucSunCoronaB = 0, bool bOverrideWindVelocity = false,
        float fWindVelX = 0, float fWindVelY = 0, float fWindVelZ = 0,
        bool bOverrideFarClipDistance = false, float fFarClip = 0,
        bool bOverrideFogDistance = false, float fFogDistance = 0,
        float fAircraftMaxHeight = 800, float fAircraftMaxVelocity = 1.5f,
        bool bOverrideMoonSize = false, int iMoonSize = 3
    ) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_MAP_INFO; };
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    std::uint8_t         m_ucWeather;
    std::uint8_t         m_ucWeatherBlendingTo;
    std::uint8_t         m_ucBlendedWeatherHour;
    std::uint8_t         m_ucClockHour;
    std::uint8_t         m_ucClockMin;
    unsigned long        m_ulMinuteDuration;
    bool                 m_bShowNametags;
    bool                 m_bShowRadar;
    float                m_fGravity;
    float                m_fGameSpeed;
    float                m_fWaveHeight;
    SWorldWaterLevelInfo m_WorldWaterLevelInfo;
    bool                 m_bHasSkyGradient;
    std::uint8_t         m_ucSkyGradientTR, m_ucSkyGradientTG, m_ucSkyGradientTB;
    std::uint8_t         m_ucSkyGradientBR, m_ucSkyGradientBG, m_ucSkyGradientBB;
    bool                 m_bHasHeatHaze;
    SHeatHazeSettings    m_HeatHazeSettings;
    std::uint16_t        m_usFPSLimit;
    const SGarageStates* m_pGarageStates;
    bool                 m_bCloudsEnabled;
    float                m_fJetpackMaxHeight;
    bool                 m_bOverrideWaterColor;
    std::uint8_t         m_ucWaterRed;
    std::uint8_t         m_ucWaterGreen;
    std::uint8_t         m_ucWaterBlue;
    std::uint8_t         m_ucWaterAlpha;
    bool                 m_bInteriorSoundsEnabled;
    bool                 m_bOverrideRainLevel;
    float                m_fRainLevel;
    bool                 m_bOverrideSunSize;
    float                m_fSunSize;
    bool                 m_bOverrideSunColor;
    std::uint8_t         m_ucSunCoreR;
    std::uint8_t         m_ucSunCoreG;
    std::uint8_t         m_ucSunCoreB;
    std::uint8_t         m_ucSunCoronaR;
    std::uint8_t         m_ucSunCoronaG;
    std::uint8_t         m_ucSunCoronaB;
    bool                 m_bOverrideWindVelocity;
    float                m_fWindVelX;
    float                m_fWindVelY;
    float                m_fWindVelZ;
    bool                 m_bOverrideFarClipDistance;
    float                m_fFarClip;
    bool                 m_bOverrideFogDistance;
    float                m_fFogDistance;
    float                m_fAircraftMaxHeight;
    float                m_fAircraftMaxVelocity;
    bool                 m_bOverrideMoonSize;
    int                  m_iMoonSize;
};
