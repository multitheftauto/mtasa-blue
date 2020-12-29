/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_VehicleWeapons.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

VehicleWeaponHitHandler* pVehicleWeaponHitHandler = nullptr;

void CMultiplayerSA::SetVehicleWeaponHitHandler(VehicleWeaponHitHandler* pHandler)
{
    pVehicleWeaponHitHandler = pHandler;
}

static void TriggerVehicleWeaponHitEvent(EVehicleWeaponType weaponType, CVehicleSAInterface* pGameVehicle, CEntitySAInterface* pHitGameEntity,
                                         CVector& vecPosition, int iModel, int iSurfaceType)
{
    if (!pVehicleWeaponHitHandler)
        return;

    SVehicleWeaponHitEvent event = {
        weaponType, pGameVehicle, pHitGameEntity, vecPosition, iModel, iSurfaceType,
    };

    pVehicleWeaponHitHandler(event);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CWaterCannon::Render()
//
// Calculates the destination of the water stream, renders the splash fx there and
// informs the water cannon audio entity.
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x729324 | 0F 84 1C 02 00 00 | jz    0x729546
// >>> 0x72932A | 68 CD CC 4C 3E    | push  3E4CCCCDh
//     0x72932F | 68 00 00 80 3F    | push  3F800000h
#define HOOKPOS_CWaterCannon__Render         0x72932A
#define HOOKSIZE_CWaterCannon__Render        5
static DWORD CONTINUE_CWaterCannon__Render = 0x72932F;

static void HandleWaterCannonHit(CVehicleSAInterface* pGameVehicle, CColPointSAInterface* pColPoint, CEntitySAInterface** ppHitGameEntity)
{
    if (!pVehicleWeaponHitHandler)
        return;

    CEntitySAInterface* const pHitGameEntity = ppHitGameEntity ? *ppHitGameEntity : nullptr;
    const int                 iModel = pHitGameEntity ? pHitGameEntity->m_nModelIndex : -1;

    TriggerVehicleWeaponHitEvent(EVehicleWeaponType::WATER_CANNON, pGameVehicle, pHitGameEntity, pColPoint->Position, iModel, pColPoint->ucSurfaceTypeB);
}

static void _declspec(naked) HOOK_CWaterCannon__Render()
{
    _asm
    {
        pushad
        mov     eax, [ebx]                      // CVehicleSAInterface* CWaterCannon::m_pVehicle
        lea     ebx, [esp + 100h - 54h]         // CColPointSAInterface*
        lea     ecx, [esp + 100h - 58h]         // CEntitySAInterface**
        push    ecx                             // ppHitGameEntity
        push    ebx                             // pColPoint
        push    eax                             // pGameVehicle
        call    HandleWaterCannonHit
        add     esp, 12
        popad
        push    3E4CCCCDh
        jmp     CONTINUE_CWaterCannon__Render
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_VehicleWeapons
//
// Setup hooks for vehicle weapon related events
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_VehicleWeapons()
{
    EZHookInstall(CWaterCannon__Render);
}
