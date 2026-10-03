/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Tasks.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include "CMultiplayerSA.h"
#include "CRemoteDataSA.h"
#include "multiplayer_shotsync.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskSimplePlayerOnFoot::MakeAbortable
//
// If ignorefirestate is enabled, we need this hook to avoid tweaking the aiming animation
// and the chainsaw turning off when entering fire
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool __IsIgnoreFireStateEnabled()
{
    return pGameInterface->IsIgnoreFireStateEnabled();
}

#define HOOKPOS_CTaskSimplePlayerOnFoot__MakeAbortable  0x68584D
#define HOOKSIZE_CTaskSimplePlayerOnFoot__MakeAbortable 6
static constexpr std::uintptr_t RETURN_CTaskSimplePlayerOnFoot__MakeAbortable = 0x68585F;
static constexpr std::uintptr_t SKIP_CTaskSimplePlayerOnFoot__MakeAbortable = 0x685855;
static void __declspec(naked)   HOOK_CTaskSimplePlayerOnFoot__MakeAbortable()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // return false and keep task alive
        call dword ptr [eax+8]
        cmp eax, 3Dh
        jl skip

        // if eventPriority == 66 (EventOnFire) && IsIgnoreFireStateEnabled()
        cmp eax, 42h
        jne continue_logic

        call __IsIgnoreFireStateEnabled
        test al, al
        jz continue_logic

        // return true but keep task alive
        pop edi
        pop esi
        pop ebx
        mov al, 1
        retn 0Ch

        continue_logic:
        jmp RETURN_CTaskSimplePlayerOnFoot__MakeAbortable

        skip:
        jmp SKIP_CTaskSimplePlayerOnFoot__MakeAbortable
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskComplexJump::CreateSubTask
//
// In GTA:SA, ledge grab height calculation on dynamic objects fails to account for
// the object's Z translation matrix. This hook adds object matrix position to the edge height.
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool ProcessGrab(CEntitySAInterface* entity, float entityEdgeHeight, const CVector* pedPosition)
{
    float edgeHeight = entityEdgeHeight;
    if (entity && entity->nType == ENTITY_TYPE_OBJECT)
    {
        if (entity->matrix)
            edgeHeight = entityEdgeHeight + entity->matrix->vPos.fZ;
        else
            edgeHeight = entityEdgeHeight + entity->m_transform.m_translate.fZ;
    }

    return (pedPosition && (edgeHeight - pedPosition->fZ >= 1.4f));
}

#define HOOKPOS_CTaskComplexJump__CreateSubTask  0x67DABE
#define HOOKSIZE_CTaskComplexJump__CreateSubTask 6
static constexpr std::uintptr_t JUMP_CTaskComplexJump__CreateSubTask_Grab = 0x67DAD6;
static constexpr std::uintptr_t JUMP_CTaskComplexJump__CreateSubTask_NoGrab = 0x67DAD1;

static void __declspec(naked) HOOK_CTaskComplexJump__CreateSubTask()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // eax = pedPosition, esi = jumpTask (this->m_pSubTask)
        // [esi + 28] = entity, [esi + 16] = edgeHeight
        pushad
        push    eax                     // pedPosition
        push    dword ptr [esi + 16]    // entityEdgeHeight
        push    dword ptr [esi + 28]    // entity
        call    ProcessGrab
        add     esp, 12
        test    al, al
        jz      no_grab

        popad
        jmp     JUMP_CTaskComplexJump__CreateSubTask_Grab

    no_grab:
        popad
        jmp     JUMP_CTaskComplexJump__CreateSubTask_NoGrab
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskSimplePlayerOnFoot::ProcessPlayerWeapon
//
// Handles player weapon state processing and synchronizing remote player firing/aiming tasks.
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool ProcessPlayerWeapon(CPedSAInterface* pedInterface)
{
    if (IsLocalPlayer(pedInterface))
        return true;

    SClientEntity<CPedSA>* pedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pedInterface);
    CPlayerPed*            ped = pedClientEntity ? dynamic_cast<CPlayerPed*>(pedClientEntity->pEntity) : nullptr;
    if (ped)
    {
        CRemoteDataStorageSA* remoteData = CRemoteDataSA::GetRemoteDataStorage(ped);
        if (remoteData && remoteData->ProcessPlayerWeapon())
        {
            return true;
        }
    }
    return false;
}

#define HOOKPOS_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon  0x6859A0
#define HOOKSIZE_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon 7
static constexpr std::uintptr_t RETURN_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon = 0x6859A7;

static void __declspec(naked) HOOK_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    dword ptr [esp + 32 + 4]    // pedInterface (arg1)
        call    ProcessPlayerWeapon
        add     esp, 4
        test    al, al
        jz      skip

        popad
        push    0FFFFFFFFh
        push    846BCEh
        jmp     RETURN_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon

    skip:
        popad
        retn    4
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskSimplePlayerOnFire::ProcessPed
//
// Passes the fire's creator entity (ped->pFire->pCreator) to the damage event instead of NULL,
// allowing kill/damage attribution to function properly when players catch fire.
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTaskSimplePlayerOnFire_ProcessPed  0x6336DA
#define HOOKSIZE_CTaskSimplePlayerOnFire_ProcessPed 5
static constexpr std::uintptr_t RETURN_CTaskSimplePlayerOnFire_ProcessPed = 0x6336E0;

static void __declspec(naked) HOOK_CTaskSimplePlayerOnFire_ProcessPed()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    3
        push    0x25
        push    edx
        mov     eax, [edi + 0x730]      // eax = ped->pFire
        mov     eax, [eax + 0x14]       // eax = pFire->pCreator
        push    eax
        jmp     RETURN_CTaskSimplePlayerOnFire_ProcessPed
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskSimpleClimb::ScanToGrabSectorList
//
// Skips entities that have collision disabled or were explicitly set non-collidable
// with the climbing ped via setElementCollidableWith.
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool CTaskSimpleClimb_ShouldSkipEntity(CPedSAInterface* ped, CEntitySAInterface* target, bool targetUsesCollision)
{
    if (!targetUsesCollision)
        return true;

    if (ped && target && CMultiplayerSA::m_pProcessCollisionHandler)
        return !CMultiplayerSA::m_pProcessCollisionHandler(ped, target);

    return false;
}

#define HOOKPOS_CTaskSimpleClimb_ScanToGrabSectorList  0x67DF28
#define HOOKSIZE_CTaskSimpleClimb_ScanToGrabSectorList 8
static constexpr std::uintptr_t RETURN_CTaskSimpleClimb_ScanToGrabSectorList = 0x67DF36;
static constexpr std::uintptr_t SKIP_CTaskSimpleClimb_ScanToGrabSectorList = 0x67E580;

static void __declspec(naked) HOOK_CTaskSimpleClimb_ScanToGrabSectorList()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // ebx = ped, esi = target entity, cx = target scan code
        test    byte ptr [esi + 1Ch], 1
        mov     word ptr [esi + 2Ch], cx
        setne   al

        pushad
        movzx   eax, al
        push    eax                     // targetUsesCollision
        push    esi                     // target
        push    ebx                     // ped
        call    CTaskSimpleClimb_ShouldSkipEntity
        add     esp, 12
        test    al, al
        jnz     skip_entity

        popad
        jmp     RETURN_CTaskSimpleClimb_ScanToGrabSectorList

    skip_entity:
        popad
        jmp     SKIP_CTaskSimpleClimb_ScanToGrabSectorList
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskSimpleGangDriveBy::ProcessPed
//
// Dispatches to custom drive-by animation handler to allow override/custom animations.
//
//////////////////////////////////////////////////////////////////////////////////////////
static void CTaskSimpleGangDriveBy_ProcessPed(DWORD gangDriveByTask)
{
    if (!gangDriveByTask)
        return;

    auto*        requiredAnim = reinterpret_cast<AnimationId*>(gangDriveByTask + 0x24);
    AssocGroupId requiredAnimGroup = *reinterpret_cast<AssocGroupId*>(gangDriveByTask + 0x28);

    if (CMultiplayerSA::m_pDrivebyAnimationHandler != nullptr)
        *requiredAnim = CMultiplayerSA::m_pDrivebyAnimationHandler(*requiredAnim, requiredAnimGroup);
}

#define HOOKPOS_CTaskSimpleGangDriveBy__ProcessPed  0x62D5A7
#define HOOKSIZE_CTaskSimpleGangDriveBy__ProcessPed 5
static constexpr std::uintptr_t RETURN_CTaskSimpleGangDriveBy__ProcessPed = 0x62D5AC;
static constexpr std::uintptr_t CANCEL_CTaskSimpleGangDriveBy__ProcessPed = 0x62D5C1;

static void __declspec(naked) HOOK_CTaskSimpleGangDriveBy__ProcessPed()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // esi contains 'this' (CTaskSimpleGangDriveBy)
        pushad
        push    esi
        call    CTaskSimpleGangDriveBy_ProcessPed
        add     esp, 4
        popad

        // Replaced code
        cmp     [esi + 28h], edi
        jnz     cancel

        jmp     RETURN_CTaskSimpleGangDriveBy__ProcessPed

    cancel:
        jmp     CANCEL_CTaskSimpleGangDriveBy__ProcessPed
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Tasks
//
// Setup task hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Tasks()
{
    EZHookInstall(CTaskSimplePlayerOnFoot__MakeAbortable);
    EZHookInstall(CTaskComplexJump__CreateSubTask);
    EZHookInstall(CTaskSimplePlayerOnFoot_ProcessPlayerWeapon);
    EZHookInstall(CTaskSimplePlayerOnFire_ProcessPed);
    EZHookInstall(CTaskSimpleClimb_ScanToGrabSectorList);
    EZHookInstall(CTaskSimpleGangDriveBy__ProcessPed);
}
