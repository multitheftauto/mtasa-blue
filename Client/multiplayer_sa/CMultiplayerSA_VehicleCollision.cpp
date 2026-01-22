/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_VehicleCollision.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

extern CCoreInterface* g_pCore;

static CVehicleSAInterface* pCollisionVehicle = nullptr;
VehicleCollisionHandler*    pVehicleCollisionHandler = nullptr;

void CMultiplayerSA::SetVehicleCollisionHandler(VehicleCollisionHandler* pHandler)
{
    pVehicleCollisionHandler = pHandler;
}

void TriggerVehicleCollisionEvent()
{
    if (!pVehicleCollisionHandler || !pCollisionVehicle)
        return;

    CEntitySAInterface* pEntity = pCollisionVehicle->m_pCollidedEntity;

    if (!pEntity)
        return;

    TIMING_CHECKPOINT("+TriggerVehColEvent");
    if (pEntity->nType == ENTITY_TYPE_VEHICLE)
    {
        auto pInterface = static_cast<CVehicleSAInterface*>(pEntity);

        pVehicleCollisionHandler(pCollisionVehicle, pEntity, pEntity->m_nModelIndex, pCollisionVehicle->m_fDamageImpulseMagnitude,
                                 pInterface->m_fDamageImpulseMagnitude, pCollisionVehicle->m_usPieceType, pCollisionVehicle->m_vecCollisionPosition,
                                 pCollisionVehicle->m_vecCollisionImpactVelocity, false);
    }
    else
    {
        const bool isProjectile = static_cast<CProjectileSAInterface*>(pEntity)->IsProjectableVTBL();
        pVehicleCollisionHandler(pCollisionVehicle, pEntity, pEntity->m_nModelIndex, pCollisionVehicle->m_fDamageImpulseMagnitude, 0.0f,
                                 pCollisionVehicle->m_usPieceType, pCollisionVehicle->m_vecCollisionPosition, pCollisionVehicle->m_vecCollisionImpactVelocity,
                                 pEntity->nType == ENTITY_TYPE_OBJECT && isProjectile);
    }
    TIMING_CHECKPOINT("-TriggerVehColEvent");
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::ProcessControl
//
// Trigger onClientVehicleCollision for:
//      CAutomobile, CPlane, CHeli, CMonsterTruck, CQuadBike, CTrailer
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CAutomobile_ProcessControl_VehicleDamage               0x6B1F3B
#define HOOKSIZE_CAutomobile_ProcessControl_VehicleDamage              6
static const DWORD CONTINUE_CAutomobile_ProcessControl_VehicleDamage = 0x6B1F41;

static void _declspec(naked) HOOK_CAutomobile_ProcessControl_VehicleDamage()
{
    _asm
    {
        pushad
        mov pCollisionVehicle, ecx
    }

    TriggerVehicleCollisionEvent();

    _asm
    {
        popad
        mov     ecx, pCollisionVehicle
        mov     esi, pCollisionVehicle
        mov     eax, [esi]
        call    dword ptr[eax + 0E0h]
        jmp     CONTINUE_CAutomobile_ProcessControl_VehicleDamage
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CBike::ProcessControl
//
// Trigger onClientVehicleCollision for:
//      CBike, CBmx
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CBike_ProcessControl_VehicleDamage               0x6B9AA5
#define HOOKSIZE_CBike_ProcessControl_VehicleDamage              6
static const DWORD CONTINUE_CBike_ProcessControl_VehicleDamage = 0x6B9AAB;

static void _declspec(naked) HOOK_CBike_ProcessControl_VehicleDamage()
{
    _asm
    {
        pushad
        mov pCollisionVehicle, ecx
    }

    TriggerVehicleCollisionEvent();

    _asm
    {
        popad
        mov     ecx, pCollisionVehicle
        mov     esi, pCollisionVehicle
        mov     eax, [esi]
        call    dword ptr[eax + 0E0h]
        jmp     CONTINUE_CBike_ProcessControl_VehicleDamage
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CBoat::ProcessControl
//
// Trigger onClientVehicleCollision for:
//      CBoat
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CBoat_ProcessControl_VehicleDamage               0x6F1864
#define HOOKSIZE_CBoat_ProcessControl_VehicleDamage              5
static const DWORD CONTINUE_CBoat_ProcessControl_VehicleDamage = 0x6F1869;
static const DWORD FUNC_CVehicle_ProcessCarAlarm = 0x6D21F0;

static void _declspec(naked) HOOK_CBoat_ProcessControl_VehicleDamage()
{
    _asm
    {
        pushad
        mov pCollisionVehicle, ecx
    }

    TriggerVehicleCollisionEvent();

    _asm
    {
        popad
        mov     ecx, pCollisionVehicle
        mov     esi, pCollisionVehicle
        call    FUNC_CVehicle_ProcessCarAlarm
        jmp     CONTINUE_CBoat_ProcessControl_VehicleDamage
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTrain::ProcessControl
//
// Trigger onClientVehicleCollision for:
//      CTrain
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTrain_ProcessControl_VehicleDamage               0x6F86BB
#define HOOKSIZE_CTrain_ProcessControl_VehicleDamage              5
static const DWORD CONTINUE_CTrain_ProcessControl_VehicleDamage = 0x6F86C0;

static void _declspec(naked) HOOK_CTrain_ProcessControl_VehicleDamage()
{
    _asm
    {
        pushad
        mov pCollisionVehicle, esi
    }

    TriggerVehicleCollisionEvent();

    _asm
    {
        popad
        mov     esi, pCollisionVehicle
        mov     al, ds:[0BA6728h]
        jmp     CONTINUE_CTrain_ProcessControl_VehicleDamage
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_VehicleCollision
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_VehicleCollision()
{
    EZHookInstall(CAutomobile_ProcessControl_VehicleDamage);
    EZHookInstall(CBike_ProcessControl_VehicleDamage);
    EZHookInstall(CBoat_ProcessControl_VehicleDamage);
    EZHookInstall(CTrain_ProcessControl_VehicleDamage);
}
