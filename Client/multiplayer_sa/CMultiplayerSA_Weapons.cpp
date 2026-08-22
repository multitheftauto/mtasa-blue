/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Weapons.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CEventDamage.h>

extern EDamageReasonType g_GenerateDamageEventReason;
extern FireHandler*      m_pFireHandler;
static CElapsedTime      ms_LastFxTimer;

//////////////////////////////////////////////////////////////////////////////////////////
//
// CWeapon::GenerateDamageEvent
//
// Try to detect pistol whippings
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CWeapon_GenerateDamageEvent(DWORD calledFrom, CPedSAInterface* pPed, CEntitySAInterface* pEntity, eWeaponType weaponType, uint uiFlags1,
                                      ePedPieceTypes pedPieceType, uint uiFlags2)
{
    // uiFlags1 appears to be:
    //          4 - punch
    //          7 - punch 2
    //         12 - punch 3
    //         20 - ground kick
    //          8 - pistol whip
    //        140 - deagle fire
    //         40 - silenced pistol fire
    //         25 - pistol fire
    //         25 - mp5 fire
    //         20 - tec fire
    //         20 - uzi fire
    //         30 - ak47 fire
    //         30 - m4 fire
    //         75 - sniper fire
    //         14 - shovel hit
    //         10 - rocket launcher explode
    //         10 - dead
    //          and lots more probably

    if (uiFlags1 == 8)
        g_GenerateDamageEventReason = EDamageReason::PISTOL_WHIP;
    else
        g_GenerateDamageEventReason = EDamageReason::OTHER;
}

// Hook info
#define HOOKPOS_CWeapon_GenerateDamageEvent  0x73A530
#define HOOKSIZE_CWeapon_GenerateDamageEvent 7
DWORD                         RETURN_CWeapon_GenerateDamageEvent = 0x73A537;
static void __declspec(naked) HOOK_CWeapon_GenerateDamageEvent()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        call    OnMY_CWeapon_GenerateDamageEvent
        add     esp, 4*6+4
        popad

        push    0FFFFFFFFh
        push    848E10h
        jmp     RETURN_CWeapon_GenerateDamageEvent
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CShotInfo_Update
//
// Reset shotinfo array when game is not running
//
//////////////////////////////////////////////////////////////////////////////////////////

// Hook info
#define HOOKPOS_CShotInfo_Update  0x739E60
#define HOOKSIZE_CShotInfo_Update 6
DWORD RETURN_CShotInfo_Update = 0x739E66;

// Clear all shotinfos
void ResetShotInfoArray()
{
    CFlameShotInfo* pInfo = (CFlameShotInfo*)ARRAY_CFlameShotInfo;
    memset(pInfo, 0, sizeof(CFlameShotInfo));
    pInfo->weaponType = WEAPONTYPE_PISTOL;
    pInfo->fRadius = 1;
    for (uint i = 1; i < MAX_FLAME_SHOT_INFOS; i++)
        memcpy(pInfo + i, pInfo, sizeof(CFlameShotInfo));
}

#pragma warning(push)
#pragma warning(disable : 4731)  // warning C4731: 'Call_CShotInfo_Update' : frame pointer register 'ebp' modified by inline assembly code

void Call_CShotInfo_Update()
{
    // clang-format off
    __asm
    {
        call inner
        jmp  done
    inner:
        push    ebp
        mov     ebp, esp
        and     esp, 0FFFFFFF8h
        jmp     RETURN_CShotInfo_Update
    done:
    }
    // clang-format on
}

#pragma warning(pop)

// Our code for when CShotInfo_Update is called
void OnMY_CShotInfo_Update()
{
    if (!pMultiplayer->IsConnected())
    {
        // Reset shotinfo array when game is not running
        ResetShotInfoArray();
    }

    __try
    {
        // Call original CShotInfo::Update with hacky protection against bad pointers
        Call_CShotInfo_Update();
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
    {
        // Reset shotinfo array when it has problems
        ResetShotInfoArray();
    }
}

// The hook goes here
static void __declspec(naked) HOOK_CShotInfo_Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    OnMY_CShotInfo_Update
        popad
        retn
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Fx_AddBulletImpact
//
// Modify bullet impact effect type
//
// 1 = sparks
// 2 = sand
// 3 = wood
// 4 = dust
//
//////////////////////////////////////////////////////////////////////////////////////////
int OnMY_Fx_AddBulletImpact(int iType)
{
    // Limit sand or dust effect due to performance issues
    if (iType == 2 || iType == 4)
    {
        if (ms_LastFxTimer.Get() > 500)
            ms_LastFxTimer.Reset();  // Allow once every 500ms
        else
            iType = 1;  // Otherwise replace with spark
    }
    return iType;
}

// Hook info
#define HOOKPOS_Fx_AddBulletImpact  0x049F3E8
#define HOOKSIZE_Fx_AddBulletImpact 5
DWORD RETURN_Fx_AddBulletImpact = 0x049F3ED;

static void __declspec(naked) HOOK_Fx_AddBulletImpact()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    eax
        call    OnMY_Fx_AddBulletImpact
        mov     [esp+0], eax         // Put result temp
        add     esp, 4*1
        popad

        mov     esi, [esp-32-4*1]    // Get result temp
        mov     eax, ds:0x0B6F03C
        jmp     RETURN_Fx_AddBulletImpact
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVisibilityPlugins::RenderWeaponPedsForPC
//
// Fix for the bright objects after weapon change sometimes
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CVisibilityPlugins_RenderWeaponPedsForPC  0x733123
#define HOOKSIZE_CVisibilityPlugins_RenderWeaponPedsForPC 5
static constexpr DWORD        CONTINUE_CVisibilityPlugins_RenderWeaponPedsForPC = 0x733128;
static void __declspec(naked) HOOK_CVisibilityPlugins_RenderWeaponPedsForPC()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov eax, 5DF4E0h
        call eax // call CPed::ResetGunFlashAlpha

        mov eax, 5533B0h
        mov ecx, ebx

        push 0
        call eax // call CPed::RemoveLighting

        jmp CONTINUE_CVisibilityPlugins_RenderWeaponPedsForPC
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CWaterLevel::TestLineAgainstWater
//
// Limit the values to world bounds, because the water level does not exceed it.
//
//////////////////////////////////////////////////////////////////////////////////////////
static void CLAMP_CWaterLevel_TestLineAgainstWater(float values[6])
{
    for (int i = 0; i < 6; ++i)
        values[i] = Clamp(-3000.0f, values[i], 3000.0f);
}

#define HOOKPOS_CWaterLevel_TestLineAgainstWater  0x6E61B0
#define HOOKSIZE_CWaterLevel_TestLineAgainstWater 10
static constexpr DWORD       CONTINUE_CWaterLevel_TestLineAgainstWater = 0x6E61BA;
static void _declspec(naked) HOOK_CWaterLevel_TestLineAgainstWater()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // [esp+4]  from.x
        // [esp+8]  from.y
        // [esp+12] from.z
        // [esp+16] to.x
        // [esp+20] to.y
        // [esp+24] to.z
        pushad
        lea     eax, [esp+32+4]
        push    eax
        call    CLAMP_CWaterLevel_TestLineAgainstWater
        add     esp, 4
        popad

        fld     [esp+0Ch]
        sub     esp, 88h
        jmp CONTINUE_CWaterLevel_TestLineAgainstWater
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CWorld::SetWorldOnFire
//
// Passes the creator entity parameter (creatorEntity) to CFireManager::StartFire
// instead of passing a null pointer, so world fire damage is attributed to the creator entity.
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CWorld_SetWorldOnFire  0x56B983
#define HOOKSIZE_CWorld_SetWorldOnFire 5
static constexpr std::uintptr_t RETURN_CWorld_SetWorldOnFire = 0x56B989;

static void __declspec(naked) HOOK_CWorld_SetWorldOnFire()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // Actually pass the creator entity parameter to CFireManager::StartFire (instead of null)
    // clang-format off
    __asm
    {
        push    7000
        push    [esp + 18h + 14h]
        jmp     RETURN_CWorld_SetWorldOnFire
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CFire::ProcessFire
//
// Sets new fire instances spawned from existing fire (creeping fire) to inherit
// the parent fire's creator entity.
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CFire_ProcessFire  0x53AC1A
#define HOOKSIZE_CFire_ProcessFire 5
static constexpr std::uintptr_t RETURN_CFire_ProcessFire = 0x53AC1F;

static void __declspec(naked) HOOK_CFire_ProcessFire()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // Set the new fire's creator to the parent fire's creator
    // clang-format off
    __asm
    {
        mov     eax, 0x53A450       // CCreepingFire::TryToStartFireAtCoors
        call    eax
        test    eax, eax
        jz      fail
        mov     ecx, [esi + 14h]
        mov     [eax + 14h], ecx

    fail:
        jmp     RETURN_CFire_ProcessFire
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CWeapon::FireAreaEffect
//
// Sets new creeping fire instances spawned by area effect weapons (e.g. Flamethrower / Molotov)
// to inherit the weapon owner entity.
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CWeapon_FireAreaEffect  0x73EBFE
#define HOOKSIZE_CWeapon_FireAreaEffect 5
static constexpr std::uintptr_t RETURN_CWeapon_FireAreaEffect = 0x73EC06;

static void __declspec(naked) HOOK_CWeapon_FireAreaEffect()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // Set the new fire's creator to the weapon owner
    // clang-format off
    __asm
    {
        mov     eax, 0x53A450       // CCreepingFire::TryToStartFireAtCoors
        call    eax
        add     esp, 1Ch            // Pop the 7 arguments pushed before the call
        test    eax, eax
        jz      fail
        mov     ecx, [esp + 54h]    // owner argument is at [esp + 54h] after popping the 28 bytes
        mov     [eax + 14h], ecx    // set fire->entityCreator

    fail:
        jmp     RETURN_CWeapon_FireAreaEffect
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CFire::ProcessFire (Ped Check)
//
// In original GTA:SA single-player, creeping/ground fire only checked FindPlayerPed(-1).
// This hook replaces the hardcoded local player check with a loop over CPools::ms_pPedPool
// so that all script-created peds, bots, and remote players catch fire when walking over
// fire on the ground.
//
//////////////////////////////////////////////////////////////////////////////////////////
static void OnProcessPedsNearFire(CFireSAInterface* fire)
{
    if (!fire || !fire->bActive)
        return;

    // Only check ground / creeping fire (skip fire attached to peds, vehicles, or objects)
    if (fire->entityTarget != nullptr)
        return;

    auto* pedPool = *reinterpret_cast<CPoolSAInterface<CPedSAInterface>**>(CLASS_CPedPool);
    if (!pedPool)
        return;

    // GTA SA ped pool slot stride (1988)
    constexpr std::uint32_t pedStride = 1988;
    auto*                   poolBase = reinterpret_cast<std::uint8_t*>(pedPool->m_pObjects);

    for (int i = 0; i < pedPool->m_nSize; ++i)
    {
        if (pedPool->IsEmpty(i))
            continue;

        auto* ped = reinterpret_cast<CPedSAInterface*>(poolBase + i * pedStride);
        if (!ped)
            continue;

        // Skip if ped is already on fire
        if (ped->pFireOnPed != nullptr)
            continue;

        // Skip if ped is dead or dying
        if (ped->fHealth <= 0.0f)
            continue;

        // Skip if ped is inside a vehicle
        if (ped->pVehicle != nullptr)
            continue;

        // Skip if ped is attached to another entity
        if (ped->m_pAttachedEntity != nullptr || ped->bAttachedToEntity)
            continue;

        // Skip if ped is fire proof or invulnerable
        if (ped->bFireProof || ped->bInvulnerable)
            continue;

        // Fast AABB Manhattan pre-filter (radius ~1.095m -> box threshold 1.1m) to reject distant peds with zero math
        const CVector& pedPosition = ped->matrix ? ped->matrix->vPos : ped->m_transform.m_translate;
        const float    deltaX = std::abs(pedPosition.fX - fire->vecPosition.fX);
        if (deltaX >= 1.1f)
            continue;

        const float deltaY = std::abs(pedPosition.fY - fire->vecPosition.fY);
        if (deltaY >= 1.1f)
            continue;

        const float deltaZ = std::abs(pedPosition.fZ - fire->vecPosition.fZ);
        if (deltaZ >= 1.1f)
            continue;

        // Exact 3D distance squared check (threshold 1.2)
        const float distanceSquared = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
        if (distanceSquared >= 1.2f)
            continue;

        // Validate entityCreator to ensure it points to a valid live ped entity
        CEntitySAInterface* validCreator = nullptr;
        if (fire->entityCreator)
        {
            const auto creatorPed = reinterpret_cast<CPedSAInterface*>(fire->entityCreator);
            if (pedPool->IsContains(pedPool->GetObjectIndexSafe(creatorPed)))
                validCreator = fire->entityCreator;
        }

        // Check friendly fire / team rules if a fire handler is registered
        if (m_pFireHandler && !m_pFireHandler(reinterpret_cast<CEntitySAInterface*>(ped), validCreator))
            continue;

        // If local player or player ped, call CPlayerPed::DoStuffToGoOnFire
        if (ped->IsPlayer())
        {
            using DoStuffToGoOnFire_t = void(__thiscall*)(CPedSAInterface*);
            reinterpret_cast<DoStuffToGoOnFire_t>(0x60A020)(ped);
        }

        // Start fire on the ped using native CFireManager::StartFire
        using StartFire_t = CFireSAInterface*(__thiscall*)(void*, CEntitySAInterface*, CEntitySAInterface*, float, std::uint8_t, std::uint32_t, std::int8_t);
        reinterpret_cast<StartFire_t>(0x53A050)(reinterpret_cast<void*>(CLASS_CFireManager), reinterpret_cast<CEntitySAInterface*>(ped), validCreator, 0.8f, 1,
                                                7000, 100);
    }
}

#define HOOKPOS_CFire_ProcessFire_PedCheck  0x53A7B4
#define HOOKSIZE_CFire_ProcessFire_PedCheck 5
static constexpr std::uintptr_t RETURN_CFire_ProcessFire_PedCheck = 0x53A8C5;

static void __declspec(naked) HOOK_CFire_ProcessFire_PedCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    esi                 // CFireSAInterface* (this pointer in CFire::ProcessFire)
        call    OnProcessPedsNearFire
        add     esp, 4
        popad

        jmp     RETURN_CFire_ProcessFire_PedCheck
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Weapons
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Weapons()
{
    EZHookInstall(CWeapon_GenerateDamageEvent);
    EZHookInstall(CShotInfo_Update);
    EZHookInstall(Fx_AddBulletImpact);
    EZHookInstall(CVisibilityPlugins_RenderWeaponPedsForPC);
    EZHookInstall(CWaterLevel_TestLineAgainstWater);
    EZHookInstall(CWorld_SetWorldOnFire);
    EZHookInstall(CFire_ProcessFire);
    EZHookInstall(CFire_ProcessFire_PedCheck);
    EZHookInstall(CWeapon_FireAreaEffect);
}
