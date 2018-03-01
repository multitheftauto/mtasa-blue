/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/Audio/CAudioZonesSA.h
*  PURPOSE:     Header file for audio zones class
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

#include <cstdint>

struct CAudioZoneBoxSAInterface
{
    char        szName[8];  // + 0x00
    int16_t     wBank;      // + 0x08
    bool        bEnabled;   // + 0x0A
    int8_t field_B;
    uint16_t    wMinX;      // + 0x0C
    uint16_t    wMinY;      // + 0x0E
    uint16_t    wMinZ;      // + 0x10
    uint16_t    wMaxX;      // + 0x12
    uint16_t    wMaxY;      // + 0x14
    uint16_t    wMaxZ;      // + 0x16
};
static_assert(sizeof(CAudioZoneBoxSAInterface) == 0x18, "Invalid size of CAudioZoneBoxSAInterface");

struct CAudioZoneSphereSAInterface
{

};
// static_assert(sizeof(CAudioZoneSphereSAInterface) == 0x18, "Invalid size of CAudioZoneSphereSAInterface");

class CAudioZonesSA
{
public:
    static void Init                ( void );
    static void RegisterAudioBox    ( const char * szZoneName, int16_t wBank, bool bEnabled, float minX, float minY, float minZ, float maxX, float maxY, float maxZ );
    static void RegisterAudioSphere ( const char * szZoneName, int16_t wBank, bool bEnabled, float centerX, float centerY, float centerZ );
    static void SwitchAudioZone     ( const char * szZoneName, bool bEnabled );
    static void Update              ( bool bForced );

public:
    static CAudioZoneBoxSAInterface ( *m_aBoxes ) [100];
};
