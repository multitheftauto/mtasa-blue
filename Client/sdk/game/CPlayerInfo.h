/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPlayerInfo.h
 *  PURPOSE:     Player entity information interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include "CWanted.h"
#include "CVehicle.h"

class CPlayerInfo
{
public:
    virtual long GetPlayerMoney() = 0;
    virtual void SetPlayerMoney(long lMoney, bool bInstant = false) = 0;

    virtual void GivePlayerParachute() = 0;
    virtual void StreamParachuteWeapon(bool bAllowParachute) = 0;

    virtual short GetLastTimeEaten() = 0;
    virtual void  SetLastTimeEaten(short sTime) = 0;

    virtual CWanted* GetWanted() = 0;
    virtual float    GetFPSMoveHeading() = 0;

    virtual void GetCrossHair(bool& bActivated, float& fTargetX, float& fTargetY) = 0;

    virtual bool GetDoesNotGetTired() = 0;
    virtual void SetDoesNotGetTired(bool bDoesNotGetTired) = 0;

    virtual CVehicle* GivePlayerRemoteControlledCar(eVehicleTypes vehicletype) = 0;

    virtual DWORD GetLastTimeBigGunFired() = 0;
    virtual void  SetLastTimeBigGunFired(DWORD dwTime) = 0;

    virtual byte GetCamDrunkLevel() = 0;
    virtual void SetCamDrunkLevel(byte level) = 0;

    virtual DWORD GetCarTwoWheelCounter() = 0;
    virtual float GetCarTwoWheelDist() = 0;
    virtual DWORD GetCarLess3WheelCounter() = 0;
    virtual DWORD GetBikeRearWheelCounter() = 0;
    virtual float GetBikeRearWheelDist() = 0;
    virtual DWORD GetBikeFrontWheelCounter() = 0;
    virtual float GetBikeFrontWheelDist() = 0;
};
