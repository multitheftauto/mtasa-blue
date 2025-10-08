/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFlyingHandlingEntrySA.h
 *  PURPOSE:     Header file for vehicle flying handling data entry class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CFlyingHandlingEntry.h>

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
    CFlyingHandlingEntrySA();
    CFlyingHandlingEntrySA(tFlyingHandlingDataSA* pOriginal);
    void                   Assign(const CFlyingHandlingEntry* pData);
    tFlyingHandlingDataSA* GetInterface() { return &m_pFlyingHandlingSA; };

private:
    tFlyingHandlingDataSA m_pFlyingHandlingSA;
};
