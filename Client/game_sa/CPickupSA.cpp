/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPickupSA.cpp
 *  PURPOSE:     Pickup entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CPickupSA.h"
#include "CWorldSA.h"

extern CGameSA* pGame;

CPickupSA::CPickupSA(CPickupSAInterface* pickupInterface)
{
    this->internalInterface = pickupInterface;
    this->object = 0;
}

void CPickupSA::SetPosition(CVector* vecPosition)
{
    this->GetInterface()->bIsPickupNearby = 0;

    CPickupSAInterface* iPickup = this->GetInterface();
    iPickup->CoorsX = (short)(vecPosition->fX * 8);
    iPickup->CoorsY = (short)(vecPosition->fY * 8);
    iPickup->CoorsZ = (short)(vecPosition->fZ * 8);
}

CVector* CPickupSA::GetPosition(CVector* vecPosition)
{
    CPickupSAInterface* iPickup = this->GetInterface();
    vecPosition->fX = iPickup->CoorsX / 8.0f;
    vecPosition->fY = iPickup->CoorsY / 8.0f;
    vecPosition->fZ = iPickup->CoorsZ / 8.0f;
    return vecPosition;
}

ePickupType CPickupSA::GetType()
{
    return (ePickupType)this->GetInterface()->Type;
}

void CPickupSA::SetType(ePickupType type)
{
    this->GetInterface()->Type = type;
}

float CPickupSA::GetCurrentValue()
{
    return this->GetInterface()->CurrentValue;
}

void CPickupSA::SetCurrentValue(float fCurrentValue)
{
    this->GetInterface()->CurrentValue = fCurrentValue;
}

void CPickupSA::SetRegenerationTime(DWORD dwTime)
{
    this->GetInterface()->RegenerationTime = dwTime;
}

void CPickupSA::SetMoneyPerDay(WORD wMoneyPerDay)
{
    this->GetInterface()->MoneyPerDay = wMoneyPerDay;
}

WORD CPickupSA::GetMoneyPerDay()
{
    return this->GetInterface()->MoneyPerDay;
}

WORD CPickupSA::GetModel()
{
    return this->GetInterface()->MI;
}

void CPickupSA::SetModel(WORD wModelIndex)
{
    this->GetInterface()->MI = wModelIndex;
}

ePickupState CPickupSA::GetState()
{
    return (ePickupState)this->GetInterface()->State;
}

void CPickupSA::SetState(ePickupState bState)
{
    this->GetInterface()->State = (BYTE)bState;
}

BYTE CPickupSA::GetAmmo()
{
    return this->GetInterface()->bNoAmmo;
}

void CPickupSA::SetAmmo(BYTE bAmmo)
{
    this->GetInterface()->bNoAmmo = bAmmo;
}

long CPickupSA::GetMonetaryValue()
{
    return this->GetInterface()->MonetaryValue;
}

void CPickupSA::SetMonetaryValue(long lMonetaryValue)
{
    this->GetInterface()->MonetaryValue = lMonetaryValue;
}

BYTE CPickupSA::IsNearby()
{
    return this->GetInterface()->bIsPickupNearby;
}

void CPickupSA::GiveUsAPickUpObject(int ForcedObjectIndex)
{
    DWORD GiveUsAPickUpObject = FUNC_GIVEUSAPICKUP;
    DWORD dwObject = (DWORD) & (this->GetInterface()->pObject);
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        push    ForcedObjectIndex
        push    dwObject
        mov     ecx, dwThis
        call    GiveUsAPickUpObject
    }
    if (this->GetInterface()->pObject)
    {
        if (this->object)
        {
            ((CEntitySA*)this->object)->DoNotRemoveFromGame = true;
            delete this->object;
        }

        this->object = new CObjectSA(this->GetInterface()->pObject);
    }
}

void CPickupSA::GetRidOfObjects()
{
    if (this->GetInterface()->pObject)
        ((CWorldSA*)pGame->GetWorld())->Remove(this->GetInterface()->pObject, CPickup_Destructor);

    if (this->object)
    {
        ((CEntitySA*)this->object)->DoNotRemoveFromGame = true;
        delete this->object;
        this->object = NULL;
    }
}

void CPickupSA::Remove()
{
    DWORD dwFunc = FUNC_CPickup_Remove;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    // CPickup::Remove also destroys the owned object, so we need to delete our CObjectSA class
    if (this->object)
    {
        ((CEntitySA*)this->object)->DoNotRemoveFromGame = true;
        delete this->object;
        this->object = NULL;
    }
}
