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

class CWanted;

class CPlayerInfo
{
public:
    virtual long GetPlayerMoney() = 0;
    virtual void SetPlayerMoney(long lMoney, bool bInstant = false) = 0;

    virtual void SetLastTimeEaten(short sTime) = 0;

    virtual CWanted* GetWanted() = 0;
    virtual float    GetFPSMoveHeading() = 0;

    virtual void SetDoesNotGetTired(bool bDoesNotGetTired) = 0;

    virtual byte GetCamDrunkLevel() = 0;
    virtual void SetCamDrunkLevel(byte level) = 0;

    virtual DWORD GetCarTwoWheelCounter() = 0;
    virtual float GetCarTwoWheelDist() = 0;
    virtual DWORD GetCarLess3WheelCounter() = 0;
    virtual DWORD GetBikeRearWheelCounter() = 0;
    virtual float GetBikeRearWheelDist() = 0;
    virtual DWORD GetBikeFrontWheelCounter() = 0;
    virtual float GetBikeFrontWheelDist() = 0;
    virtual std::uint8_t GetMaxHealth() const = 0;
    virtual std::uint8_t GetMaxArmor() const = 0;
};
