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
static void _declspec(naked) HOOK_CWorld_TriggerExplosion()
{
    _asm
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
}

#define HOOKPOS_CWorld_TriggerExplosionSectorList  0x5677F4
#define HOOKSIZE_CWorld_TriggerExplosionSectorList 7
static constexpr std::uintptr_t RETURN_CWorld_TriggerExplosionSectorList = 0x5677FB;
static constexpr std::uintptr_t SKIP_CWorld_TriggerExplosionSectorList = 0x568473;
static void _declspec(naked) HOOK_CWorld_TriggerExplosionSectorList()
{
    _asm
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
}
