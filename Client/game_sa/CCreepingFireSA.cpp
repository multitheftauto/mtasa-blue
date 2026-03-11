/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCreepingFireSA.cpp
 *  PURPOSE:     Fire manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCreepingFireSA.h"
#include "CGameSA.h"
#include "CFireManagerSA.h"
#include "CColPointSA.h"
#include "CFireSA.h"
#include "CPoolsSA.h"

extern CGameSA* pGame;

static std::array<std::array<std::uint8_t, 32>, 32> m_fireStatus{};

bool CCreepingFireSA::TryToStartFireAtCoors(CVector position, std::uint8_t numGenerationsAllowed, void* registerWithCreator, bool unused_2, float zDistance)
{
    std::uint8_t status = m_fireStatus[static_cast<std::size_t>(position.fX) & 31][static_cast<std::size_t>(position.fY) & 31];
    if (status != 0)
        return false;

    if (!pGame->GetFireManager()->PlentyFiresAvailable())
        return false;

    // Call CWorld::ProcessVerticalLine
    CEntitySAInterface*  hitEntity = nullptr;
    CColPointSAInterface cp;
    if (!((bool(__cdecl*)(CVector*, float, CColPointSAInterface*, CEntitySAInterface**, bool, bool, bool, bool, bool, bool, void*))0x5674E0)(
            &position, position.fZ - zDistance, &cp, &hitEntity, true, false, false, false, false, false, nullptr))
        return false;

    position.fZ = cp.Position.fZ;
    status = 6;

    CFire* fire = pGame->GetFireManager()->StartFire(position, 0.8f, nullptr, 20000, numGenerationsAllowed);
    if (!fire)
        return false;

    CEntitySAInterface* creatorInterface = nullptr;
    if (registerWithCreator)
    {
        void* returnAddress = _ReturnAddress();
        if (returnAddress == (void*)0x7377D8)                                               // CExplosion::Update
            creatorInterface = *(CEntitySAInterface**)((char*)registerWithCreator - 0x18);  // [esi-0x18]
        else if (returnAddress == (void*)0x73EC03)                                          // CWeapon::FireAreaEffect
            // esp + 0x6C+0x4 + 0xC (CVector) + 0x4 (push) = esp + 0x80
            creatorInterface = *(CEntitySAInterface**)((char*)registerWithCreator + 0x80);  // [esp+0x80]
        else                                                                                // our CFireSA::ProcessFire
            creatorInterface = static_cast<CEntitySAInterface*>(registerWithCreator);
    }

    CEntity* creator = creatorInterface ? pGame->GetPools()->GetEntity((DWORD*)creatorInterface) : nullptr;
    if (creator)
        fire->SetCreator(creator);

    return true;
}

void CCreepingFireSA::Update()
{
    int          frame = pGame->GetSystemFrameCounter();
    std::uint8_t status = m_fireStatus[frame % 32][(frame / 32) % 32];

    if (status == 4)
        status = 0;
    else if (status > 4 && status <= 6)
        status--;
}

void CCreepingFireSA::StaticSetHooks()
{
    // Patch calls to CCreepingFire::TryToStartFireAtCoors
    HookInstallCall(0x7377D3, (DWORD)CCreepingFireSA::TryToStartFireAtCoors);
    HookInstallCall(0x73EBFE, (DWORD)CCreepingFireSA::TryToStartFireAtCoors);

    // Change push 1 to push esi in CExplosion::Update
    MemCpy((void*)0x7377BE, "\x56\x90", 2);
    // Change push 1 to push esp in CWeapon::FireAreaEffect
    MemCpy((void*)0x73EB7F, "\x54\x90", 2);

    // Patch call to CCreepingFire::Update
    HookInstallCall(0x53BFFB, (DWORD)CCreepingFireSA::Update);
}
