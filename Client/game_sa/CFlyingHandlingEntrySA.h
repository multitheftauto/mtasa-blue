/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFlyingHandlingEntrySA.h
 *  PURPOSE:     Header file for vehicle flying handling data entry class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CFlyingHandlingEntry.h>
#include "Common.h"

// sizeof(tFlyingHandlingDataSA) == 0x58
struct tFlyingHandlingDataSA
{
    int     iVehicleID;
    float   fThrust;
    float   fThrustFallOff;
    float   fYaw;
    float   fYawStab;
    float   fSideSlip;
    float   fRoll;
    float   fRollStab;
    float   fPitch;
    float   fPitchStab;
    float   fFormLift;
    float   fAttackLift;
    float   fGearUpR;
    float   fGearDownL;
    float   fWindMult;
    float   fMoveResistance;
    CVector vecTurnResistance;
    CVector vecSpeedResistance;
};

class CFlyingHandlingEntrySA : public CFlyingHandlingEntry
{
public:
    // Constructor for creatable dummy entries
    CFlyingHandlingEntrySA();

    // Constructor for original entries
    CFlyingHandlingEntrySA(tFlyingHandlingDataSA* pOriginal);

    // Use this to copy data from an another handling class to this
    void Assign(const CFlyingHandlingEntry* pData);

    tFlyingHandlingDataSA* GetInterface() { return m_pFlyingHandlingSA.get(); };

private:
    std::shared_ptr<tFlyingHandlingDataSA> m_pFlyingHandlingSA;
};

