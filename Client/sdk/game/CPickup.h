/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPickup.h
 *  PURPOSE:     Pickup entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "enums/PickupState.h"
#include "enums/PickupType.h"

class CObject;
class CVector;

class CPickup
{
public:
    virtual CObject* GetObject() = 0;
    virtual void     SetPosition(CVector* vecPosition) = 0;
    virtual CVector* GetPosition(CVector* vecPosition) = 0;

    virtual PickupType  GetType() = 0;
    virtual void         SetType(PickupType type) = 0;
    virtual float        GetCurrentValue() = 0;
    virtual void         SetCurrentValue(float fCurrentValue) = 0;
    virtual void         SetRegenerationTime(DWORD dwTime) = 0;
    virtual void         SetMoneyPerDay(WORD wMoneyPerDay) = 0;
    virtual WORD         GetMoneyPerDay() = 0;
    virtual WORD         GetModel() = 0;
    virtual void         SetModel(WORD wModelIndex) = 0;            // do not export
    virtual PickupState GetState() = 0;
    virtual void         SetState(PickupState bState) = 0;
    virtual BYTE         GetAmmo() = 0;
    virtual void         SetAmmo(BYTE bAmmo) = 0;
    virtual long         GetMonetaryValue() = 0;
    virtual void         SetMonetaryValue(long lMonetaryValue) = 0;
    virtual BYTE         IsNearby() = 0;
    virtual void         Remove() = 0;
    virtual void         GiveUsAPickUpObject(int ForcedObjectIndex = -1) = 0;
    virtual void         GetRidOfObjects() = 0;
};
