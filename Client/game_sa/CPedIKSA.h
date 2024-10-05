/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedIKSA.h
 *  PURPOSE:     Header file for ped entity inverse kinematics class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPedIK.h>

class CPedSAInterface;
struct RwV3d;

/**** STRAIGHT FROM R* *****/
// Ped IK flags
enum
{
    PEDIK_GUN_REACHED_TARGET = 0x1,
    PEDIK_TORSO_USED = 0x2,
    PEDIK_USE_ARM = 0x4,
    PEDIK_SLOPE_PITCH = 0x08,
    PEDIK_EVERYTHING_USED = 0x10
};

// Return flags from MoveLimb() function
enum MoveLimbResult
{
    CANT_REACH_TARGET,
    HAVENT_REACHED_TARGET,
    REACHED_TARGET
};

// Structure containing info about a limb
typedef struct
{
    float maxYaw, minYaw;
    float yawD;
    float maxPitch, minPitch;
    float pitchD;
} LimbMovementInfo;

/********* END ***********/

class CPedIKSAInterface
{
public:
    CPedSAInterface* ThisPed;            // 528
    LimbOrientation  m_torsoOrien;
    float            m_fSlopePitch;
    float            m_fSlopePitchLimitMult;
    float            m_fSlopeRoll;
    float            m_fBodyRoll;
    DWORD            m_flags;

    static RwV3d& XaxisIK;
    static RwV3d& YaxisIK;
    static RwV3d& ZaxisIK;
};

class CPedIKSA : public CPedIK
{
private:
    CPedIKSAInterface* internalInterface;

public:
    CPedIKSA(CPedIKSAInterface* ikInterface) { internalInterface = ikInterface; };

    // r*'s functions
    void SetFlag(DWORD flag) { internalInterface->m_flags |= flag; }
    void ClearFlag(DWORD flag) { internalInterface->m_flags &= ~flag; }
    bool IsFlagSet(DWORD flag) { return ((internalInterface->m_flags & flag) > 0 ? 1 : 0); }

    void RotateTorso(void* bone, LimbOrientation* orientation, bool flag);
};
