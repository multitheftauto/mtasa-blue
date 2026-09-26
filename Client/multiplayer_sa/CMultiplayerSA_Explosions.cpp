/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Explosions.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

bool              CMultiplayerSA::m_bExplosionsDisabled = false;
ExplosionHandler* CMultiplayerSA::m_pExplosionHandler = nullptr;

bool CMultiplayerSA::GetExplosionsDisabled()
{
    return m_bExplosionsDisabled;
}

void CMultiplayerSA::DisableExplosions(bool bDisabled)
{
    m_bExplosionsDisabled = bDisabled;
}

void CMultiplayerSA::SetExplosionHandler(ExplosionHandler* pExplosionHandler)
{
    m_pExplosionHandler = pExplosionHandler;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CExplosion::AddExplosion
//
// Intercept explosions to trigger MTA events and allow custom explosion handling
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool CallExplosionHandler(CEntitySAInterface* explodingEntityInterface, CEntitySAInterface* explosionCreatorInterface, const CVector& location,
                                 eExplosionType explosionType)
{
    if (!CMultiplayerSA::m_pExplosionHandler)
        return true;

    CEntity* explosionCreator = nullptr;
    CEntity* explodingEntity = nullptr;

    if (explosionCreatorInterface)
        explosionCreator = pGameInterface->GetPools()->GetEntity((DWORD*)explosionCreatorInterface);

    if (explodingEntityInterface)
        explodingEntity = pGameInterface->GetPools()->GetEntity((DWORD*)explodingEntityInterface);

    return CMultiplayerSA::m_pExplosionHandler(explodingEntity, explosionCreator, location, explosionType);
}

#define HOOKPOS_CExplosion_AddExplosion  0x736A50
#define HOOKSIZE_CExplosion_AddExplosion 6
static constexpr std::uintptr_t RETURN_CExplosion_AddExplosion = 0x736A56;

static void __declspec(naked) HOOK_CExplosion_AddExplosion()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Check if explosions are globally disabled
        cmp     CMultiplayerSA::m_bExplosionsDisabled, 0
        jz      check_handler
        xor     al, al
        retn

    check_handler:
        cmp     CMultiplayerSA::m_pExplosionHandler, 0
        jz      proceed_with_explosion

        // Preserve registers before calling C++ handler
        pushad

        // Pass arguments directly from stack to CallExplosionHandler:
        // [esp + 32] is return address
        // [esp + 32 + 4] = explodingEntity (CEntitySAInterface*)
        // [esp + 32 + 8] = explosionCreator (CEntitySAInterface*)
        // [esp + 32 + 12] = explosionType (eExplosionType)
        // [esp + 32 + 16..24] = location (float x, y, z)
        push    dword ptr [esp + 32 + 12]       // explosionType
        lea     eax, [esp + 32 + 16 + 4]        // &location
        push    eax
        push    dword ptr [esp + 32 + 8 + 8]    // explosionCreator
        push    dword ptr [esp + 32 + 4 + 12]   // explodingEntity
        call    CallExplosionHandler
        add     esp, 16

        test    al, al
        jz      cancel_explosion

        // Explosion allowed by handler -> restore registers and proceed
        popad

    proceed_with_explosion:
        // Replaced GTA:SA instructions (6 bytes)
        sub     esp, 1Ch
        push    ebx
        push    ebp
        push    esi
        jmp     RETURN_CExplosion_AddExplosion

    cancel_explosion:
        popad
        xor     al, al
        retn
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CExplosion::Update
//
// Set the creeping fire's creator to the explosion creator
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CExplosion_Update  0x7377D3
#define HOOKSIZE_CExplosion_Update 5
static constexpr std::uintptr_t RETURN_CExplosion_Update = 0x7377D8;

static void __declspec(naked) HOOK_CExplosion_Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // Set the new creeping fire's creator to the explosion's creator
    // clang-format off
    __asm
    {
        mov     eax, 0x53A450       // CCreepingFire::TryToStartFireAtCoors
        call    eax
        test    eax, eax
        jz      fail
        mov     ecx, [esi - 18h]
        mov     [eax + 14h], ecx

    fail:
        jmp     RETURN_CExplosion_Update
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CWorld::TriggerExplosion
//
// Fix for multiple damage instances in certain areas during explosions (GH #4125, #997)
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CWorld_TriggerExplosion  0x56B82E
#define HOOKSIZE_CWorld_TriggerExplosion 8
static constexpr std::uintptr_t RETURN_CWorld_TriggerExplosion = 0x56B836;
static void __declspec(naked)   HOOK_CWorld_TriggerExplosion()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov [esp+1Ch-8h], eax
        mov [esp+1Ch-10h], ecx

        // Call SetNextScanCode
        mov ecx, 0x4072E0
        call ecx
        mov ecx, esi

        // SetNextScanCode overwrote the result of the cmp instruction at 0x56B82A
        // so we call it again
        cmp esi, eax
        jmp RETURN_CWorld_TriggerExplosion
    }
    // clang-format on
}

#define HOOKPOS_CWorld_TriggerExplosionSectorList  0x5677F4
#define HOOKSIZE_CWorld_TriggerExplosionSectorList 7
static constexpr std::uintptr_t RETURN_CWorld_TriggerExplosionSectorList = 0x5677FB;
static constexpr std::uintptr_t SKIP_CWorld_TriggerExplosionSectorList = 0x568473;
static void __declspec(naked)   HOOK_CWorld_TriggerExplosionSectorList()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // check entity->m_nScanCode == CWorld::ms_nCurrentScanCode
        mov ecx, dword ptr ds:[0xB7CD78]
        cmp [esi+2Ch], cx
        jz skip

        // set entity current scan code
        mov [esi+2Ch], cx

        mov al, [esi+36h]
        and al, 7
        cmp al, 4
        jmp RETURN_CWorld_TriggerExplosionSectorList

        skip:
        jmp SKIP_CWorld_TriggerExplosionSectorList
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Explosions
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Explosions()
{
    EZHookInstall(CWorld_TriggerExplosion);
    EZHookInstall(CWorld_TriggerExplosionSectorList);
    EZHookInstall(CExplosion_AddExplosion);
    EZHookInstall(CExplosion_Update);
}
