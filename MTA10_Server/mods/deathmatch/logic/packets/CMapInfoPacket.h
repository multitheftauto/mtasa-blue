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
                                                      unsigned char ucSkyGradientTR = 0,
                                                      unsigned char ucSkyGradientTG = 0,
                                                      unsigned char ucSkyGradientTB = 0,
                                                      unsigned char ucSkyGradientBR = 0,
                                                      unsigned char ucSkyGradientBG = 0,
                                                      unsigned char ucSkyGradientBB = 0,
                                                      unsigned short usFPSLimit = 36,
                                                      bool bCloudsEnabled = true,
                                                      float fJetpackMaxHeight = 100,
                                                      bool bOverrideWaterColor = false,
                                                      unsigned char ucWaterRed = 0,
                                                      unsigned char ucWaterGreen = 0,
                                                      unsigned char ucWaterBlue = 0,
                                                      unsigned char ucWaterAlpha = 0);

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
    unsigned short          m_usFPSLimit;
    bool*                   m_pbGarageStates;
    bool                    m_bCloudsEnabled;
    float                   m_fJetpackMaxHeight;
    bool                    m_bOverrideWaterColor;
    unsigned char           m_ucWaterRed;
    unsigned char           m_ucWaterGreen;
    unsigned char           m_ucWaterBlue;
    unsigned char           m_ucWaterAlpha;
};

#endif
