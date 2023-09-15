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

#include <CVector.h>
#include <game/CBoatHandlingEntry.h>

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
    CBoatHandlingEntrySA();
    CBoatHandlingEntrySA(tBoatHandlingDataSA* pOriginal);
    void                 Assign(const CBoatHandlingEntry* pData);
    tBoatHandlingDataSA* GetInterface() { return &m_pBoatHandlingSA; };

private:
    tBoatHandlingDataSA m_pBoatHandlingSA;
};
