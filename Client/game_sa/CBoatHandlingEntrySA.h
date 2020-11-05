/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBoatHandlingEntrySA.h
 *  PURPOSE:     Header file for vehicle flying handling data entry class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBoatHandlingEntry.h>
#include "Common.h"

struct tBoatHandlingDataSA
{
    int     iVehicleID;
    float   fThrustY;
    float   fThrustZ;
    float   fThrustAppZ;
    float   fAqPlaneForce;
    float   fAqPlaneLimit;
    float   fAqPlaneOffset;
    float   fLookLRHeight;
    float   fWaveAudioMult;
    CVector vecMoveRes;
    CVector vecTurnRes;
};

class CBoatHandlingEntrySA : public CBoatHandlingEntry
{
public:
    // Constructor for creatable dummy entries
    CBoatHandlingEntrySA();

    // Constructor for original entries
    CBoatHandlingEntrySA(tBoatHandlingDataSA* pOriginal);

    // Use this to copy data from an another handling class to this
    void Assign(const CBoatHandlingEntry* pData);

    tBoatHandlingDataSA* GetInterface() { return m_pBoatHandlingSA.get(); };

private:
    std::shared_ptr<tBoatHandlingDataSA> m_pBoatHandlingSA;
};
