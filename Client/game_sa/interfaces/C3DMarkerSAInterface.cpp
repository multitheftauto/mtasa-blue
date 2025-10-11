/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/interfaces/C3DMarkerSAInterface.cpp
 *  PURPOSE:     3D Marker game layer interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include "C3DMarkerSAInterface.h"

bool C3DMarkerSAInterface::AddMarker(std::uint32_t id, e3DMarkerType type, float size, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a, std::uint16_t pulsePeriod, float pulseFraction, std::int16_t rotateRate)
{
    return ((bool(__thiscall*)(C3DMarkerSAInterface*, std::uint32_t, std::uint16_t, float, std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t, std::uint16_t, float, std::int16_t))0x722230)(this, id, static_cast<std::uint16_t>(type), size, r, g, b, a, pulsePeriod, pulseFraction, rotateRate);
}

void C3DMarkerSAInterface::DeleteMarkerObject()
{
    ((void(__thiscall*)(C3DMarkerSAInterface*))0x722390)(this);
}

bool C3DMarkerSAInterface::IsZCoordinateUpToDate() const
{
    const CVector& pos = m_mat.GetPosition();
    return m_LastMapReadX == static_cast<std::uint16_t>(pos.fX) && m_LastMapReadY == static_cast<std::uint16_t>(pos.fY);
}

void C3DMarkerSAInterface::SetZCoordinateIfNotUpToDate(float newZPos)
{
    if (!IsZCoordinateUpToDate())
    {
        CVector& pos = m_mat.GetPosition();
        pos.fZ = newZPos;
    }
}

void C3DMarkerSAInterface::UpdateZCoordinate(CVector point, float zDistance)
{
    ((void(__thiscall*)(C3DMarkerSAInterface*, CVector, float))0x724D40)(this, point, zDistance);
}

void C3DMarkerSAInterface::DeleteIfHasAtomic()
{
    if (m_pRwObject)
        DeleteMarkerObject();
}
