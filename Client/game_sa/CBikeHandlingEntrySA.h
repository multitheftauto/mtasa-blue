/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHandlingEntrySA.h
 *  PURPOSE:     Header file for vehicle handling data entry class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBikeHandlingEntry.h>

struct tBikeHandlingDataSA
{
    int   iVehicleID;
    float fLeanFwdCOM;
    float fLeanFwdForce;
    float fLeanBackCOM;
    float fLeanBackForce;
    float fMaxLean;
    float fFullAnimLean;
    float fDesLean;
    float fSpeedSteer;
    float fSlipSteer;
    float fNoPlayerCOMz;
    float fWheelieAng;
    float fStoppieAng;
    float fWheelieSteer;
    float fWheelieStabMult;
    float fStoppieStabMult;
};

class CBikeHandlingEntrySA : public CBikeHandlingEntry
{
public:
    CBikeHandlingEntrySA();
    CBikeHandlingEntrySA(tBikeHandlingDataSA* pOriginal);
    void                 Assign(const CBikeHandlingEntry* pData);
    void                 Recalculate();
    tBikeHandlingDataSA* GetInterface() { return &m_pBikeHandlingSA; };

private:
    tBikeHandlingDataSA m_pBikeHandlingSA;
    tBikeHandlingDataSA m_BikeHandling;
};
