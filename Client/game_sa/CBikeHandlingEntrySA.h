/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHandlingEntrySA.h
 *  PURPOSE:     Header file for vehicle handling data entry class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBikeHandlingEntry.h>
#include "Common.h"
#define FUNC_HandlingDataMgr_ConvertBikeDataToGameUnits 0x6F5290

struct tBikeHandlingDataSA
{
    int iVehicleID;
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
    // Constructor for creatable dummy entries
    CBikeHandlingEntrySA();

    // Constructor for original entries
    CBikeHandlingEntrySA(tBikeHandlingDataSA* pOriginal);

    virtual ~CBikeHandlingEntrySA();

    // Use this to copy data from an another handling class to this
    void Assign(const CBikeHandlingEntry* pData);

    void Recalculate();

    tBikeHandlingDataSA* GetInterface() { return m_pBikeHandlingSA; };

private:
    tBikeHandlingDataSA* m_pBikeHandlingSA;
    bool                 m_bDeleteInterface;

    tBikeHandlingDataSA m_BikeHandling;
};
