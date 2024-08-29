/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHeliSA.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHeli.h>
#include "CAutomobileSA.h"

class CHeliSAInterface : public CAutomobileSAInterface
{
public:
    std::uint8_t m_heliFlags;

    std::uint8_t _pad1[3];
    std::uint32_t m_leftRightSkid;
    std::uint32_t m_steeringUpDown;
    std::uint32_t m_steeringLeftRight;
    std::uint32_t m_accelerationBreakStatus;
    std::uint32_t field_99C;
    std::uint32_t m_rotorZ;
    std::uint32_t m_secondRotorZ;
    std::uint32_t m_maxAltitude;
    std::uint32_t field_9AC;
    std::uint32_t m_minAltitude;
    std::uint32_t field_9B4;
    std::uint8_t field_9B8;
    std::uint8_t m_numSwatOccupants;
    std::uint8_t m_swatIDs[4];

    std::uint8_t _pad2[2];
    std::uint32_t field_9C0[4];
    std::uint32_t field_9D0;

    std::uint32_t m_particlesList;
    std::uint8_t field_9D8[24];
    std::uint32_t field_9F0;
    CVector m_searchLightTarget;
    std::uint32_t m_searchLightIntensity;
    std::uint32_t field_A04;
    std::uint32_t field_A08;
    std::uint32_t m_gunflashFx;
    std::uint8_t m_firingMultiplier;
    std::uint8_t m_searchLightEnabled;
    std::uint8_t _pad3[2];
    std::uint32_t field_A14;
};
static_assert(sizeof(CHeliSAInterface) == 0xA18, "Invalid size for CHeliSAInterface");

class CHeliSA final : public virtual CHeli, public virtual CAutomobileSA
{
public:
    CHeliSA(CHeliSAInterface* pInterface);
    CHeliSAInterface* GetHeliInterface() noexcept { return reinterpret_cast<CHeliSAInterface*>(GetInterface()); }
};
