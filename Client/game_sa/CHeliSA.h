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
    std::uint8_t  m_nHeliFlags;                          // 0x988
    std::uint8_t  ___pad1[3];                            // 0x989
    float         m_fLeftRightSkid;                      // 0x98C
    float         m_fSteeringUpDown;                     // 0x990
    float         m_fSteeringLeftRight;                  // 0x994
    float         m_fAccelerationBreakStatus;            // 0x998
    std::int32_t  field_99C;                             // 0x99C
    float         m_fRotorZ;                             // 0x9A0
    float         m_fSecondRotorZ;                       // 0x9A4
    float         m_fMaxAltitude;                        // 0x9A8
    std::int32_t  field_9AC;                             // 0x9AC
    float         m_fMinAltitude;                        // 0x9B0
    std::int32_t  field_9B4;                             // 0x9B4
    std::uint8_t  field_9B8;                             // 0x9B8
    std::uint8_t  m_nNumSwatOccupants;                   // 0x9B9
    std::uint8_t  m_anSwatIDs[4];                        // 0x9BA
    std::uint8_t  ___pad2[2];                            // 0x9BE
    std::uint32_t field_9C0[4];                          // 0x9C0
    std::int32_t  field_9D0;                             // 0x9D0
    FxSystem_c**  m_pParticlesList;                      // 0x9D4
    std::uint8_t  field_9D8[24];                         // 0x9D8
    std::int32_t  field_9F0;                             // 0x9F0
    CVector       m_vecSearchLightTarget;                // 0x9F4
    float         m_fSearchLightIntensity;               // 0xA00
    std::int32_t  field_A04;                             // 0xA04
    std::int32_t  field_A08;                             // 0xA08
    FxSystem_c**  m_ppGunflashFx;                        // 0xA0C
    std::uint8_t  m_nFiringMultiplier;                   // 0xA10
    bool          m_bSearchLightEnabled;                 // 0xA11
    std::uint8_t  ___pad3[2];                            // 0xA12
    std::int32_t  field_A14;                             // 0xA14
};
static_assert(sizeof(CHeliSAInterface) == 0xA18, "Invalid size for CHeliSAInterface");

class CHeliSA final : public virtual CHeli, public virtual CAutomobileSA
{
public:
    CHeliSA(CHeliSAInterface* pInterface);
    CHeliSAInterface* GetHeliInterface() noexcept { return reinterpret_cast<CHeliSAInterface*>(GetInterface()); }
};
