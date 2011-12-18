/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPickupsSA.cpp
*  PURPOSE:     Pickup manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPickupsSA::CPickupsSA()
{
    DEBUG_TRACE("CPickupsSA::CPickupsSA()");
    for(int i = 0;i<MAX_PICKUPS;i++)
        Pickups[i] = new CPickupSA((CPickupSAInterface *)(ARRAY_PICKUPS + i * sizeof(CPickupSAInterface)));
}

CPickupsSA::~CPickupsSA ( void )
{
    for ( int i = 0; i < MAX_PICKUPS; i++ )
    {
        delete Pickups [i];
    }
}

CPickup * CPickupsSA::GetPickup(DWORD ID)
{
    DEBUG_TRACE("CPickup * CPickupsSA::GetPickup(DWORD ID)");
    return (CPickup *)Pickups[ID];
}

CPickup * CPickupsSA::CreatePickup(CVector * position, DWORD ModelIndex, ePickupType Type, DWORD dwMonetaryValue, DWORD dwMoneyPerDay, BYTE bPingOutOfPlayer)
{
    DEBUG_TRACE("CPickup * CPickupsSA::CreatePickup(CVector * position, DWORD ModelIndex, ePickupType Type, DWORD dwMonetaryValue, DWORD dwMoneyPerDay, BYTE bPingOutOfPlayer)");
    DWORD FreeSlot=0;
    bool bFoundFreeSlot=false;
    CPickupSA * pickup;

    /*if (Type == PICKUP_FLOATINGPACKAGE || Type == PICKUP_NAUTICAL_MINE_INACTIVE || bPingOutOfPlayer)
    {
      FreeSlot = MAX_PICKUPS + MAX_PICKUPS_ALWAYS_UPDATED - 1;
      while (FreeSlot >= 0 && Pickups[FreeSlot]->GetInterface()->Type != PICKUP_NONE)
      {
         FreeSlot--;
      }

      if (FreeSlot >= 0) bFoundFreeSlot = true;
    }*/

    if (!bFoundFreeSlot)
    {
      FreeSlot = 0;
      while (FreeSlot < MAX_PICKUPS && Pickups[FreeSlot]->GetInterface()->Type != PICKUP_NONE)
      {
         FreeSlot++;
      }
      if (FreeSlot < MAX_PICKUPS) bFoundFreeSlot = true;
    }

    if (!bFoundFreeSlot)
    {
      // Simply use first money pickup.
      FreeSlot = 0;
      while (FreeSlot < MAX_PICKUPS && Pickups[FreeSlot]->GetInterface()->Type != PICKUP_MONEY)
      {
         FreeSlot++;
      }

      if (FreeSlot >= MAX_PICKUPS)
      { // In that case use the first PICKUP_ONCE_TIMEOUT
         FreeSlot = 0;
         while (FreeSlot < MAX_PICKUPS &&
            Pickups[FreeSlot]->GetInterface()->Type != PICKUP_ONCE_TIMEOUT &&
            Pickups[FreeSlot]->GetInterface()->Type != PICKUP_ONCE_TIMEOUT_SLOW)
         {
            FreeSlot++;
         }
      }

      // We need to tidy up the objects that go with this pickup
      if (FreeSlot >= MAX_PICKUPS)
      { // Couldn't find one. Let's just hope we don't break anything in the scripts.
        // ASSERTMSG(0, "Ran out of pickups");
         return (CPickup *)NULL;
      }
      else
      {
         Pickups[FreeSlot]->GetRidOfObjects();
      }
    }


    if (FreeSlot >= MAX_PICKUPS + MAX_PICKUPS_ALWAYS_UPDATED) return (CPickup *)NULL;
    pickup = Pickups[FreeSlot];
    // Generate an object in the world for us.
    // Set the values for this pickup
    pickup->SetType(Type);
    pickup->SetState(PUSTATE_ON);
    pickup->SetMonetaryValue(dwMonetaryValue);
    pickup->SetMoneyPerDay((WORD)dwMoneyPerDay);
    pickup->SetCurrentValue(0.0f);
    pickup->SetRegenerationTime(pGame->GetSystemTime());
    pickup->SetAmmo(bPingOutOfPlayer);
    
    if (Type == PICKUP_ONCE_TIMEOUT)
    {
      pickup->SetRegenerationTime(pGame->GetSystemTime() + 20000);
    }
    if (Type == PICKUP_ONCE_TIMEOUT_SLOW)
    {
      pickup->SetRegenerationTime(pGame->GetSystemTime() + 120000);
    }
    if (Type == PICKUP_MONEY)
    {
      pickup->SetRegenerationTime(pGame->GetSystemTime() + 30000); // Money stays for 30 secs
    }
    if (Type == PICKUP_MINE_INACTIVE || Type == PICKUP_MINE_ARMED)
    {
      pickup->SetType(PICKUP_MINE_INACTIVE);
      pickup->SetRegenerationTime(pGame->GetSystemTime() + 1500); // Mines get activated after 2 secs
    }
    if (Type == PICKUP_NAUTICAL_MINE_INACTIVE || Type == PICKUP_NAUTICAL_MINE_ARMED)
    {
      pickup->SetType(PICKUP_NAUTICAL_MINE_INACTIVE);
      pickup->GetInterface()->RegenerationTime = pGame->GetSystemTime() + 1500; // Mines get activated after 2 secs
    }
    pickup->SetModel((WORD)ModelIndex);

    // All pickups get generated at the coordinates that we get in from the
    // script. This way the level designers can create them floating in mid-air
    pickup->SetPosition(position);
    //MemCpyFast (&Pickups[FreeSlot]->GetInterface()->position,position, sizeof(CVector));
    // Pickups[FreeSlot]->GetInterface()->PUCoors = CVector(Coors.x, Coors.y, 0.7f + CWorld::FindGroundZFor3DCoord(Coors.x, Coors.y, Coors.z + 1.0f));

    pickup->GiveUsAPickUpObject();

    if (pickup->GetInterface()->pObject)
      ((CWorldSA *)pGame->GetWorld())->Add(pickup->GetInterface()->pObject);

    return pickup;
}

void CPickupsSA::DisablePickupProcessing ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_CPickups__Update;
        MemPut < BYTE > ( FUNC_CPickups__Update, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_CPickups__Update, byteOriginal );
        byteOriginal = 0;
    }
}
