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

extern CGameSA* pGame;

static std::array<std::array<std::uint8_t, 32>, 32> m_fireStatus{};

bool CCreepingFireSA::TryToStartFireAtCoors(CVector position, std::uint8_t numGenerationsAllowed, int unused_1, bool unused_2, float zDistance)
{
    std::uint8_t status = m_fireStatus[static_cast<std::size_t>(position.fX) % 32][static_cast<std::size_t>(position.fY) % 32];
    if (status == 0)
        return false;

    if (!pGame->GetFireManager()->PlentyFiresAvailable())
        return false;

    // Call CWorld::ProcessVerticalLine
    CEntitySAInterface* hitEntity = nullptr;
    CColPointSAInterface cp;
    if (!((bool(__cdecl*)(CVector*, float, CColPointSAInterface*, CEntitySAInterface**, bool, bool, bool, bool, bool, bool, void*))0x5674E0)(
            &position, position.fZ - zDistance, &cp, &hitEntity, true, false, false, false, false, false, nullptr))
        return false;
    
    position.fZ = cp.Position.fZ;
    status = 6;

    return pGame->GetFireManager()->StartFire(position, 0.8f, nullptr, 20000, numGenerationsAllowed) != nullptr;
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

    // Patch call to CCreepingFire::Update
    HookInstallCall(0x53BFFB, (DWORD)CCreepingFireSA::Update);
}
