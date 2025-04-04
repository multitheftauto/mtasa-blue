/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/interfaces/C3DMarkerSAInterface.h
 *  PURPOSE:     Header file for 3D Marker game layer interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/C3DMarker.h>
#include "game/RenderWare.h"
#include "../CMatrixSA.h"

class C3DMarkerSAInterface
{
public:
    CMatrixSAInterface m_mat;            // local space to world space transform
    RpAtomic*      m_pRwObject;
    RpMaterial*    m_pMaterial;
    std::uint16_t  m_nType;              // e3DMarkerType
    bool           m_bIsUsed;            // has this marker been allocated this frame?
    bool           m_mustBeRenderedThisFrame;
    std::uint32_t  m_nIdentifier;
    RwColor        rwColour;
    std::uint16_t  m_nPulsePeriod;
    std::int16_t   m_nRotateRate;            // deg per frame (in either direction)
    std::uint32_t  m_nStartTime;
    float          m_fPulseFraction;
    float          m_fStdSize;
    float          m_fSize;
    float          m_fBrightness;
    float          m_fCameraRange;
    CVector        m_normal;            // Normal of the object point at

    // The following variables remember the last time we read the height of the map.
    // Using this we don't have to do this every frame and we can still have moving markers.
    std::uint16_t m_LastMapReadX;
    std::uint16_t m_LastMapReadY;
    float         m_LastMapReadResultZ;
    float         m_roofHeight;
    CVector       m_lastPosition;
    std::uint32_t m_OnScreenTestTime;            // time last screen check was done

public:
    inline bool AddMarker(std::uint32_t id, e3DMarkerType type, float size, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a, std::uint16_t pulsePeriod,
                   float pulseFraction, std::int16_t rotateRate);
    inline void DeleteMarkerObject();
    inline bool IsZCoordinateUpToDate() const;
    inline void SetZCoordinateIfNotUpToDate(float newZPos);
    inline void UpdateZCoordinate(CVector point, float zDistance);
    inline void DeleteIfHasAtomic();
};
static_assert(sizeof(C3DMarkerSAInterface) == 0xA0, "Invalid size for C3DMarkerSAInterface");
