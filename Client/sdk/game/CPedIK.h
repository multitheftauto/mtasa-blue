/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPedIK.h
 *  PURPOSE:     Ped inverse kinematics interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Structure containing current state of limb
struct LimbOrientation
{
    float yaw, pitch;
};

class CPedIK
{
public:
    virtual void SetFlag(DWORD flag) = 0;
    virtual void ClearFlag(DWORD flag) = 0;
    virtual bool IsFlagSet(DWORD flag) = 0;
    virtual void RotateTorso(void* bone, LimbOrientation* orientation, bool flag) = 0;
};
