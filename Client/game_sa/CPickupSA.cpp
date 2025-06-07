/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPickupSA.cpp
 *  PURPOSE:     Pickup entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CPickupSA.h"
#include "CWorldSA.h"

extern CGameSA* pGame;

CPickupSA::CPickupSA(CPickupSAInterface* pickupInterface)
{
    internalInterface = pickupInterface;
    object = 0;
}

void CPickupSA::SetPosition(CVector* vecPosition)
{
    GetInterface()->bIsPickupNearby = 0;

    CPickupSAInterface* iPickup = GetInterface();
    iPickup->CoorsX = (short)(vecPosition->fX * 8);
    iPickup->CoorsY = (short)(vecPosition->fY * 8);
    iPickup->CoorsZ = (short)(vecPosition->fZ * 8);
}

CVector* CPickupSA::GetPosition(CVector* vecPosition)
{
    CPickupSAInterface* iPickup = GetInterface();
    vecPosition->fX = iPickup->CoorsX / 8.0f;
    vecPosition->fY = iPickup->CoorsY / 8.0f;
    vecPosition->fZ = iPickup->CoorsZ / 8.0f;
    return vecPosition;
}

PickupType CPickupSA::GetType()
{
    return (PickupType)GetInterface()->Type;
}

void CPickupSA::SetType(PickupType type)
{
    GetInterface()->Type = (BYTE)type;
}

float CPickupSA::GetCurrentValue()
{
    return GetInterface()->CurrentValue;
}

void CPickupSA::SetCurrentValue(float fCurrentValue)
{
    GetInterface()->CurrentValue = fCurrentValue;
}

void CPickupSA::SetRegenerationTime(DWORD dwTime)
{
    GetInterface()->RegenerationTime = dwTime;
}

void CPickupSA::SetMoneyPerDay(WORD wMoneyPerDay)
{
    GetInterface()->MoneyPerDay = wMoneyPerDay;
}

WORD CPickupSA::GetMoneyPerDay()
{
    return GetInterface()->MoneyPerDay;
}

WORD CPickupSA::GetModel()
{
    return GetInterface()->MI;
}

void CPickupSA::SetModel(WORD wModelIndex)
{
    GetInterface()->MI = wModelIndex;
}

PickupState CPickupSA::GetState()
{
    return (PickupState)GetInterface()->State;
}

void CPickupSA::SetState(PickupState bState)
{
    GetInterface()->State = (BYTE)bState;
}

BYTE CPickupSA::GetAmmo()
{
    return GetInterface()->bNoAmmo;
}

void CPickupSA::SetAmmo(BYTE bAmmo)
{
    GetInterface()->bNoAmmo = bAmmo;
}

long CPickupSA::GetMonetaryValue()
{
    return GetInterface()->MonetaryValue;
}

void CPickupSA::SetMonetaryValue(long lMonetaryValue)
{
    GetInterface()->MonetaryValue = lMonetaryValue;
}

BYTE CPickupSA::IsNearby()
{
    return GetInterface()->bIsPickupNearby;
}

void CPickupSA::GiveUsAPickUpObject(int ForcedObjectIndex)
{
    DWORD GiveUsAPickUpObject = FUNC_GIVEUSAPICKUP;
    DWORD dwObject = (DWORD) & (GetInterface()->pObject);
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    ForcedObjectIndex
        push    dwObject
        mov     ecx, dwThis
        call    GiveUsAPickUpObject
    }
    if (GetInterface()->pObject)
    {
        if (object)
        {
            ((CEntitySA*)object)->DoNotRemoveFromGame = true;
            delete object;
        }

        object = new CObjectSA(GetInterface()->pObject);
    }
}

void CPickupSA::GetRidOfObjects()
{
    if (GetInterface()->pObject)
        ((CWorldSA*)pGame->GetWorld())->Remove(GetInterface()->pObject, CPickup_Destructor);

    if (object)
    {
        ((CEntitySA*)object)->DoNotRemoveFromGame = true;
        delete object;
        object = NULL;
    }
}

void CPickupSA::Remove()
{
    DWORD dwFunc = FUNC_CPickup_Remove;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    // CPickup::Remove also destroys the owned object, so we need to delete our CObjectSA class
    if (object)
    {
        ((CEntitySA*)object)->DoNotRemoveFromGame = true;
        delete object;
        object = NULL;
    }
}
