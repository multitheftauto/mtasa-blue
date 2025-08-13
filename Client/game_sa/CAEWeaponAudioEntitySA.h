/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAEWeaponAudioEntitySA.h
 *  PURPOSE:     Weapon audio entity header
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CAudioEngineSA.h"

enum class eMiniGunState : std::uint8_t
{
    SPINNING,
    FIRING,
    STOPPING,
    STOPPED,
};

enum class eChainsawState : std::uint8_t
{
    IDLE,
    ACTIVE,
    CUTTING,
    STOPPING,
    STOPPED,
};

class CAEWeaponAudioEntitySAInterface : public CAEAudioEntity
{
public:
    bool m_isMiniGunSpinActive;
    bool m_isMiniGunFireActive;

    std::uint8_t m_lastWeaponPlaneFrequencyIndex;

    eMiniGunState  m_miniGunState;
    eChainsawState m_chainsawState;

    std::uint32_t  m_lastFlamethrowerFireTimeInMS;
    std::uint32_t  m_lastSpraycanFireTimeInMS;
    std::uint32_t  m_lastFireExtinguisherFireTimeInMS;
    std::uint32_t  m_lastMinigunFireTimeInMS;
    std::uint32_t  m_lastChainsawFireTimeInMS;
    std::uint32_t  m_lastGunFireTimeInMS;

    CAESound* m_flameThrowerIdleGasLoopSound{};
};
static_assert(sizeof(CAEWeaponAudioEntitySAInterface) == 0xA0, "Invalid size for CAEWeaponAudioEntitySAInterface");
