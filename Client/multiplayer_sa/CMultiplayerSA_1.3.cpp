/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_1.3.cpp
 *  PURPOSE:     Multiplayer module class 1.3
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CWorld.h>

extern CCoreInterface* g_pCore;

#define FUNC_CPed__RenderTargetMarker 0x60BA80

WaterCannonHitHandler* m_pWaterCannonHitHandler = NULL;

VehicleFellThroughMapHandler* m_pVehicleFellThroughMapHandler = NULL;

#define HOOKPOS_CEventHitByWaterCannon 0x729899
DWORD RETURN_CWaterCannon_PushPeds_RETN = 0x7298A7;
DWORD CALL_CEventHitByWaterCannon = 0x4B1290;
DWORD RETURN_CWaterCannon_PushPeds_RETN_Cancel = 0x729AEB;

void HOOK_CEventHitByWaterCannon();

#define HOOKPOS_CTaskSimpleJetpack_ProcessInput 0x67E7F1
DWORD RETN_CTaskSimpleJetpack_ProcessInputEnable = 0x67E812;
DWORD RETN_CTaskSimpleJetpack_ProcessInputDisabled = 0x67E821;

#define HOOKPOS_CTaskSimplePlayerOnFoot_ProcessWeaponFire 0x685ABA
DWORD RETN_CTaskSimplePlayerOnFoot_ProcessWeaponFire = 0x685ABF;
DWORD RETN_CTaskSimplePlayerOnFoot_ProcessWeaponFire_Call = 0x540670;

#define HOOKPOS_CWorld_RemoveFallenPeds 0x565D0D
DWORD RETURN_CWorld_RemoveFallenPeds_Cont = 0x565D13;
DWORD RETURN_CWorld_RemoveFallenPeds_Cancel = 0x565E6F;

#define HOOKPOS_CWorld_RemoveFallenCars 0x565F52
DWORD RETURN_CWorld_RemoveFallenCars_Cont = 0x565F59;
DWORD RETURN_CWorld_RemoveFallenCars_Cancel = 0x56609B;

#define HOOKPOS_CVehicleModelInterface_SetClump 0x4C9606
DWORD RETURN_CVehicleModelInterface_SetClump = 0x4C9611;

#define HOOKPOS_CProjectile_FixTearGasCrash 0x4C0403
DWORD RETURN_CProjectile_FixTearGasCrash_Fix = 0x4C05B9;
DWORD RETURN_CProjectile_FixTearGasCrash_Cont = 0x4C0409;

#define HOOKPOS_CVehicle_ProcessTyreSmoke_Initial     0x6DE8A2
#define HOOKPOS_CVehicle_ProcessTyreSmoke_Burnouts    0x6DF197
#define HOOKPOS_CVehicle_ProcessTyreSmoke_Braking     0x6DECED
#define HOOKPOS_CVehicle_ProcessTyreSmoke_HookAddress 0x6DF308

#define HOOKPOS_CProjectile_FixExplosionLocation 0x738A77
DWORD RETURN_CProjectile_FixExplosionLocation = 0x738A86;

void          HOOK_CTaskSimpleJetpack_ProcessInput();
void          HOOK_CTaskSimplePlayerOnFoot_ProcessWeaponFire();
void          HOOK_CTaskSimpleJetpack_ProcessInputFixFPS2();
void          HOOK_CWorld_RemoveFallenPeds();
void          HOOK_CWorld_RemoveFallenCars();
void          HOOK_CVehicleModelInterface_SetClump();
void          HOOK_CProjectile_FixTearGasCrash();
void          HOOK_CProjectile_FixExplosionLocation();
void* __cdecl HOOK_CMemoryMgr_MallocAlign(int size, int alignment, int nHint);
void __cdecl  HOOK_CMemoryMgr_FreeAlign(void* ptr);

void CMultiplayerSA::Init_13()
{
    InitHooks_13();
    InitMemoryCopies_13();
}

void CMultiplayerSA::InitHooks_13()
{
    // HookInstalls go here
    HookInstall(HOOKPOS_CEventHitByWaterCannon, (DWORD)HOOK_CEventHitByWaterCannon, 9);

    InitHooks_VehicleSirens();

    HookInstall(HOOKPOS_CTaskSimpleJetpack_ProcessInput, (DWORD)HOOK_CTaskSimpleJetpack_ProcessInput, 5);
    HookInstall(HOOKPOS_CTaskSimplePlayerOnFoot_ProcessWeaponFire, (DWORD)HOOK_CTaskSimplePlayerOnFoot_ProcessWeaponFire, 5);

    HookInstall(HOOKPOS_CWorld_RemoveFallenPeds, (DWORD)HOOK_CWorld_RemoveFallenPeds, 6);

    HookInstall(HOOKPOS_CWorld_RemoveFallenCars, (DWORD)HOOK_CWorld_RemoveFallenCars, 5);

    HookInstall(HOOKPOS_CVehicleModelInterface_SetClump, (DWORD)HOOK_CVehicleModelInterface_SetClump, 7);

    HookInstall(HOOKPOS_CProjectile_FixTearGasCrash, (DWORD)HOOK_CProjectile_FixTearGasCrash, 6);

    HookInstall(HOOKPOS_CProjectile_FixExplosionLocation, (DWORD)HOOK_CProjectile_FixExplosionLocation, 12);

    InitHooks_ClothesSpeedUp();
    EnableHooks_ClothesMemFix(true);
    InitHooks_FixBadAnimId();
    InitHooks_HookDestructors();
    InitHooks_RwResources();
    InitHooks_ClothesCache();
    InitHooks_Files();
    InitHooks_Weapons();
    InitHooks_Peds();
    InitHooks_ObjectCollision();
    InitHooks_VehicleCollision();
    InitHooks_VehicleDummies();
    InitHooks_Vehicles();
    InitHooks_Rendering();
    InitHooks_FixMallocAlign();
}

void CMultiplayerSA::InitMemoryCopies_13()
{
    // Memory based fixes go here
    // MemSet ( (void*)0x6AB35A, 0x90, 12 ); // Ignore some retarded R* if statement that checks if the model is the buffalo and jumps the siren code even
    // though it doesn't have a siren anyway

    // Pass on loading priority to dependent models
    MemPut<BYTE>(0x040892A, 0x53);
    MemPut<BYTE>(0x040892B, 0x90);

    MemPut<BYTE>(0x04341C0, 0xC3);  // Skip CCarCtrl::GenerateRandomCars

    // Prevent garages deleting vehicles
    MemPut<BYTE>(0x0449C50, 0xC3);
    MemPut<BYTE>(0x0449D10, 0xC3);

    // Move birds up a bit so they don't fly through solid objects quite so often
    MemPut<float>(0x71240e, 10.f + 10.f);
    MemPut<float>(0x712413, 2.f + 10.f);
    MemPut<float>(0x712447, 13.f + 10.f);
    MemPut<float>(0x71244c, 4.f + 10.f);

    // shoot any weapon while on a jetpack -> moved to a hook.
    // MemPut < BYTE > ( 0x67E7F1, 0x90 );
    // MemPut < BYTE > ( 0x67E7F1+1, 0x90 );
    // MemPut < BYTE > ( 0x67E7FA, 0x90 );
    // MemPut < BYTE > ( 0x67E7FA+1, 0x90 );
    // Fixes
    // MemPut < BYTE > ( 0x685AC1, 0xEB );
    // MemPut < BYTE > ( 0x685C2D, 0xEB );
}

// Water Cannon Stuff

void CMultiplayerSA::SetWaterCannonHitHandler(WaterCannonHitHandler* pHandler)
{
    m_pWaterCannonHitHandler = pHandler;
}

CPedSAInterface*     pPedHitByWaterCannonInterface = NULL;
CVehicleSAInterface* pVehicleWithTheCannonMounted = NULL;
bool                 TriggerTheEvent()
{
    // Is our handler alive
    if (m_pWaterCannonHitHandler)
    {
        // Return our handlers return
        return !m_pWaterCannonHitHandler(pVehicleWithTheCannonMounted, pPedHitByWaterCannonInterface);
    }
    return false;
}

static void __declspec(naked) HOOK_CEventHitByWaterCannon()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        // EDX = CWaterCannon
        // EDX+0h = CVehicle Owner
        // ESI = CPed Hit
        mov eax, [edx]
        mov pPedHitByWaterCannonInterface, esi
        mov pVehicleWithTheCannonMounted, eax
    }
    // clang-format on
    if (TriggerTheEvent())
    {
        // clang-format off
        __asm
        {
            popad
            // Cancel.
            jmp RETURN_CWaterCannon_PushPeds_RETN_Cancel
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            // Replaced code
            push ebp
            push ecx
            lea ecx, [esp+0B0h]
            // Call our function
            call CALL_CEventHitByWaterCannon
            // Go back to execution
            jmp RETURN_CWaterCannon_PushPeds_RETN
        }
        // clang-format on
    }
}
CPedSAInterface* pPedUsingJetpack;
DWORD            dwJetpackPedIntelligence = NULL;
bool             IsUsingJetPack()
{
    if (pPedUsingJetpack && pPedUsingJetpack->pPedIntelligence)
    {
        dwJetpackPedIntelligence = (DWORD)pPedUsingJetpack->pPedIntelligence;
        if (dwJetpackPedIntelligence)
        {
            DWORD CPedIntelligence_FindJetpackTask = 0x601110;
            DWORD dwReturn = 0;
            // clang-format off
            __asm
            {
                mov ecx, dwJetpackPedIntelligence
                call CPedIntelligence_FindJetpackTask
                mov dwReturn, eax
            }
            // clang-format on
            return dwReturn > 0;
        }
    }
    return false;
}
bool AllowJetPack()
{
    if (pPedUsingJetpack)
    {
        if (pPedUsingJetpack->bCurrentWeaponSlot > 0)
        {
            if (IsUsingJetPack())
            {
                eWeaponType weaponType = pPedUsingJetpack->Weapons[pPedUsingJetpack->bCurrentWeaponSlot].m_eWeaponType;
                return pGameInterface->GetJetpackWeaponEnabled(weaponType);
            }
        }
    }
    return false;
}

static void __declspec(naked) HOOK_CTaskSimpleJetpack_ProcessInput()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov pPedUsingJetpack, edi
        pushad
    }
    // clang-format on
    if (AllowJetPack())
    {
        // clang-format off
        __asm
        {
            popad
            jmp RETN_CTaskSimpleJetpack_ProcessInputEnable
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            jmp RETN_CTaskSimpleJetpack_ProcessInputDisabled
        }
        // clang-format on
    }
}

static void __declspec(naked) HOOK_CTaskSimplePlayerOnFoot_ProcessWeaponFire()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        mov pPedUsingJetpack, esi
    }
    // clang-format on
    if (AllowJetPack())
    {
        // clang-format off
        __asm
        {
            popad
            xor al, al
            jmp RETN_CTaskSimplePlayerOnFoot_ProcessWeaponFire
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            popad
            call RETN_CTaskSimplePlayerOnFoot_ProcessWeaponFire_Call
            jmp RETN_CTaskSimplePlayerOnFoot_ProcessWeaponFire
        }
        // clang-format on
    }
}

CPedSAInterface* pFallingPedInterface;
bool             CWorld_Remove_FallenPedsCheck()
{
    SClientEntity<CPedSA>* pPedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pFallingPedInterface);
    CPed*                  pPed = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
    if (pPed && pPed->GetVehicle() != NULL)
    {
        // Disallow
        return true;
    }
    // Allow
    return false;
}

static void __declspec(naked) HOOK_CWorld_RemoveFallenPeds()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // If it's going to skip the code anyway just do it otherwise check if he's in a vehicle as the vehicle will be respawned anyway and he will be warped with
    // it.
    // clang-format off
    __asm
    {
        test ah, 5
        jp [RemoveFallenPeds_Cancel]
        pushad
        mov pFallingPedInterface, esi
    }
    // clang-format on
    if (CWorld_Remove_FallenPedsCheck())
    {
        // clang-format off
        __asm
        {
            popad
RemoveFallenPeds_Cancel:
            jmp RETURN_CWorld_RemoveFallenPeds_Cancel
        }
        // clang-format on
    }
    // clang-format off
    __asm
    {
        popad
        jmp RETURN_CWorld_RemoveFallenPeds_Cont
    }
    // clang-format on
}

void CMultiplayerSA::SetVehicleFellThroughMapHandler(VehicleFellThroughMapHandler* pHandler)
{
    m_pVehicleFellThroughMapHandler = pHandler;
}

CVehicleSAInterface* pFallingVehicleInterface;
bool                 CWorld_Remove_FallenVehiclesCheck()
{
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pFallingVehicleInterface);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (pVehicle && m_pVehicleFellThroughMapHandler(pFallingVehicleInterface))
    {
        // Disallow
        return true;
    }
    // Allow
    return false;
}

DWORD HOOK_CWorld_RemoveFallenCars_Cont1 = 0x565F57;

static void __declspec(naked) HOOK_CWorld_RemoveFallenCars()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // If the vehicle fell through the map give it another try to respawn.
    // clang-format off
    __asm
    {
        pushad
        mov pFallingVehicleInterface, esi
    }
    // clang-format on
    if (CWorld_Remove_FallenVehiclesCheck())
    {
        // clang-format off
        __asm
        {
            popad
            jmp RETURN_CWorld_RemoveFallenCars_Cancel
        }
        // clang-format on
    }
    // clang-format off
    __asm
    {
        popad
        mov eax, [esi + 14h]
        test eax, eax
        jz RemoveFallenCars_Cancel
        jmp RETURN_CWorld_RemoveFallenCars_Cont

     RemoveFallenCars_Cancel:
        jmp HOOK_CWorld_RemoveFallenCars_Cont1
    }
    // clang-format on
}

void CMultiplayerSA::SetPedTargetingMarkerEnabled(bool bEnable)
{
    static const uint8 original = 0x83;
    uint32             dwFunc = FUNC_CPed__RenderTargetMarker;
    if (bEnable)
    {
        MemPut<uint8>(dwFunc, original);
    }
    else
    {
        MemPut<uint8>(dwFunc, 0xC3);
    }
}

bool CMultiplayerSA::IsPedTargetingMarkerEnabled()
{
    uint32 dwFunc = FUNC_CPed__RenderTargetMarker;
    return *(uint8*)dwFunc != 0xC3;
}

CBaseModelInfoSAInterface* pLoadingModelInfo = 0;
RpClump*                   pLoadingClump = NULL;
void                       CVehicleModelInterface_SetClump()
{
    // Loop through all vehicles and find the vehicle id that this interface belongs to
    CModelInfo* pModelInfo = NULL;
    for (int i = 400; i < 612; i++)
    {
        pModelInfo = pGameInterface->GetModelInfo(i);
        if (pModelInfo && (DWORD)pModelInfo->GetInterface() == (DWORD)pLoadingModelInfo)
        {
            pModelInfo->InitialiseSupportedUpgrades(pLoadingClump);
            break;
        }
    }
}

static void __declspec(naked) HOOK_CVehicleModelInterface_SetClump()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // Grab our currently loading clump
    // Get our Handling ID because that's all that's in the interface
    // clang-format off
    __asm
    {
        pushad
        mov pLoadingClump, eax
        mov pLoadingModelInfo, esi
    }
    // clang-format on
    //   Init our supported upgrades structure for this model info
    CVehicleModelInterface_SetClump();
    // Perform overwrite sequence and jump back
    // clang-format off
    __asm
    {
        popad
        push eax
        mov ecx, esi
        mov dword ptr [esp+14h], 0FFFFFFFFh
        jmp RETURN_CVehicleModelInterface_SetClump
    }
    // clang-format on
}

// fixes a crash where a vehicle is the source of a tear gas projectile.
static void __declspec(naked) HOOK_CProjectile_FixTearGasCrash()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp ebp, 0h
        je cont
        mov ecx, [ebp+47Ch]
        // no terminators in this time period
        jmp RETURN_CProjectile_FixTearGasCrash_Cont
    cont :
        // come with me if you want to live
        jmp RETURN_CProjectile_FixTearGasCrash_Fix
        // dundundundundun
        // dundundundundun
    }
    // clang-format on
}

void CMultiplayerSA::SetBoatWaterSplashEnabled(bool bEnabled)
{
    if (bEnabled)
    {
        // Enable water splashing by restoring the original code
        MemPut<BYTE>(0x6DD167, 0x0F);
        MemPut<BYTE>(0x6DD168, 0x85);
        MemPut<BYTE>(0x6DD169, 0x6D);
        MemPut<BYTE>(0x6DD16A, 0x05);
        MemPut<BYTE>(0x6DD16B, 0x00);
        MemPut<BYTE>(0x6DD16C, 0x00);
    }
    else
    {
        // Disable water splashing by forcing a jump to the end of the function
        MemPut<BYTE>(0x6DD167, 0xE9);
        MemPut<BYTE>(0x6DD168, 0x6E);
        MemPut<BYTE>(0x6DD169, 0x05);
        MemPut<BYTE>(0x6DD16A, 0x00);
        MemPut<BYTE>(0x6DD16B, 0x00);
        MemPut<BYTE>(0x6DD16C, 0x00);
    }
}

DWORD            dwReturnAddressTyreSmoke = 0x6DE8A8;
DWORD            dwReturnIgnorePed = 0x6DF3B9;
CPedSAInterface* pTyreSmokePed = NULL;

bool IsPlayerPedLocal()
{
    SClientEntity<CPedSA>* pPedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pTyreSmokePed);
    CPed*                  pPed = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
    if (pPed)
    {
        CPed* pLocalPlayerPed = pGameInterface->GetPools()->GetPedFromRef((DWORD)1);
        if (pPed != NULL && pLocalPlayerPed != NULL)
        {
            if (pLocalPlayerPed == pPed)
            {
                return true;
            }
        }
    }
    return false;
}

// makes sure remote player tyre smoke isn't processed when tyre smoke is in the "off" position
static void __declspec(naked) HOOK_CMultiplayerSA_ToggleTyreSmoke()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        mov pTyreSmokePed, ecx
    }
    // clang-format on

    if (!IsPlayerPedLocal())
    {
        // clang-format off
        __asm
        {
            popad
            jmp dwReturnIgnorePed
        }
        // clang-format on
    }

    // clang-format off
    __asm
    {
        popad
        test eax, 20000h
        jnz ToggleTyreSmoke_Cancel
        jmp dwReturnAddressTyreSmoke

    ToggleTyreSmoke_Cancel:
        jmp dwReturnIgnorePed
    }
    // clang-format on
}

void CMultiplayerSA::SetTyreSmokeEnabled(bool bEnabled)
{
    SetBoatWaterSplashEnabled(bEnabled);
    if (bEnabled)
    {
        // revert changes made by disable.
        // this is the start of the function and ensures that remote vehicles aren't processed for tyre smoke
        MemPut<BYTE>(0x6DE8A2, 0x0F);
        MemPut<BYTE>(0x6DE8A3, 0x85);
        MemPut<BYTE>(0x6DE8A4, 0x11);
        MemPut<BYTE>(0x6DE8A5, 0x0B);
        MemPut<BYTE>(0x6DE8A6, 0x00);
        MemPut<BYTE>(0x6DE8A7, 0x00);

        // This ensures that the local vehicle tyre smoke while doing burnouts isn't rendered
        MemPut<BYTE>(0x6DF197, 0x8B);
        MemPut<BYTE>(0x6DF198, 0x44);
        MemPut<BYTE>(0x6DF199, 0x24);
        MemPut<BYTE>(0x6DF19A, 0x28);
        MemPut<BYTE>(0x6DF19B, 0x50);

        // This ensures that the local vehicle tyre smoke under braking isn't rendered
        MemPut<BYTE>(0x6DECED, 0x0F);
        MemPut<BYTE>(0x6DECEE, 0x85);
        MemPut<BYTE>(0x6DECEF, 0xA2);
        MemPut<BYTE>(0x6DECF0, 0x01);
        MemPut<BYTE>(0x6DECF1, 0x00);
    }
    else
    {
        // this is the start of the function and ensures that remote vehicles aren't processed for tyre smoke
        HookInstall(HOOKPOS_CVehicle_ProcessTyreSmoke_Initial, (DWORD)HOOK_CMultiplayerSA_ToggleTyreSmoke, 6);
        // This ensures that the local vehicle tyre smoke while doing burnouts isn't rendered
        HookInstall(HOOKPOS_CVehicle_ProcessTyreSmoke_Burnouts, HOOKPOS_CVehicle_ProcessTyreSmoke_HookAddress, 5);
        // This ensures that the local vehicle tyre smoke under braking isn't rendered
        HookInstall(HOOKPOS_CVehicle_ProcessTyreSmoke_Braking, HOOKPOS_CVehicle_ProcessTyreSmoke_HookAddress, 5);
    }
}
CPhysicalSAInterface* pExplosionEntity;

void UpdateExplosionLocation()
{
    if (pExplosionEntity)
    {
        // project backwards 20% of our velocity just to catch us going too far
        CVector vecStart = pExplosionEntity->matrix->vPos + (pExplosionEntity->m_vecLinearVelocity * 0.20f);
        // project forwards 120% to look for collisions forwards
        CVector vecEnd = vecStart - (pExplosionEntity->m_vecLinearVelocity * 1.20f);
        // calculate our actual impact position
        if (pGameInterface->GetWorld()->CalculateImpactPosition(vecStart, vecEnd))
        {
            // Apply it
            if (pExplosionEntity->matrix)
            {
                pExplosionEntity->matrix->vPos = vecEnd;
            }
            else
            {
                pExplosionEntity->m_transform.m_translate = vecEnd;
            }
        }
    }
}

static void __declspec(naked) HOOK_CProjectile_FixExplosionLocation()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov pExplosionEntity, esi
        pushad
    }
    // clang-format on
    UpdateExplosionLocation();
    // clang-format off
    __asm
    {
        popad
        mov eax, [esi+14h]
        test eax, eax
        jz skip
        add eax, 30h
        jmp RETURN_CProjectile_FixExplosionLocation
skip:
        lea eax, [esi+4]
        jmp RETURN_CProjectile_FixExplosionLocation
    }
    // clang-format on
}
