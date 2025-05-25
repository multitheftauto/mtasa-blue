/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPickupSA.h
 *  PURPOSE:     Header file for pickup entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPickup.h>
#include "CObjectSA.h"

class CObjectSAInterface;
class CObjectSA;

#define FUNC_GIVEUSAPICKUP      0x4567e0
#define FUNC_CPickup_Remove     0x4556C0

class CPickupSAInterface
{
public:
    float               CurrentValue;                      // For the revenue pickups  0
    CObjectSAInterface* pObject;                           // 4
    long                MonetaryValue;                     // 8
    DWORD               RegenerationTime;                  // 12
    short               CoorsX, CoorsY, CoorsZ;            // 16 // 18 // 20
    WORD                MoneyPerDay;                       // 22
    WORD                MI;                                // 24
    WORD                ReferenceIndex;                    // 26
    BYTE                Type;                              // 28
    BYTE                State : 1;                         // 29
    BYTE                bNoAmmo : 1;
    BYTE                bHelpMessageDisplayed : 1;
    BYTE                bIsPickupNearby : 1;            // If the pickup is nearby it will get an object and it will get updated.
    BYTE                TextIndex : 3;                  // What text label do we print out above it.
};

class CPickupSA : public CPickup
{
private:
    CPickupSAInterface* internalInterface;
    CObjectSA*          object;

public:
    CPickupSA(CPickupSAInterface* pickupInterface);
    CPickupSAInterface* GetInterface() { return internalInterface; };            // not to be exported
    CObject*            GetObject() { return object; };

    void     SetPosition(CVector* vecPosition);
    CVector* GetPosition(CVector* vecPosition);

    PickupType  GetType();
    void         SetType(PickupType type);
    float        GetCurrentValue();
    void         SetCurrentValue(float fCurrentValue);
    void         SetRegenerationTime(DWORD dwTime);
    void         SetMoneyPerDay(WORD wMoneyPerDay);
    WORD         GetMoneyPerDay();
    WORD         GetModel();
    void         SetModel(WORD wModelIndex);            // do not export
    PickupState GetState();
    void         SetState(PickupState bState);
    BYTE         GetAmmo();
    void         SetAmmo(BYTE bAmmo);
    long         GetMonetaryValue();
    void         SetMonetaryValue(long lMonetaryValue);
    BYTE         IsNearby();
    void         GiveUsAPickUpObject(int ForcedObjectIndex = -1);
    void         GetRidOfObjects();
    void         Remove();
};
