/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColStoreSA.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColStoreSA.h"
#include "CGameSA.h"

extern CGameSA* pGame;

void CColStoreSA::Initialise()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x4113F0);
    function();
}

void CColStoreSA::Shutdown()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x4114D0);
    function();
}

void CColStoreSA::BoundingBoxesPostProcess()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x410EC0);
    function();
}

int CColStoreSA::AddColSlot(const char* name)
{
    using Signature = int(__cdecl*)(const char*);
    const auto function = reinterpret_cast<Signature>(0x411140);
    return function(name);
}

bool CColStoreSA::IsValidSlot(CollisionSlot slot)
{
    // Native signature: bool __cdecl sub_410660(int a1)
    // Must use int to match GTA SA's ABI - it reads a full DWORD from stack
    using Signature = bool(__cdecl*)(int);
    const auto function = reinterpret_cast<Signature>(0x410660);
    return function(static_cast<int>(slot));
}

void CColStoreSA::AddCollisionNeededAtPosition(const CVector& position)
{
    using Signature = void(__cdecl*)(const CVector&);
    const auto function = reinterpret_cast<Signature>(0x4103A0);
    function(position);
}

void CColStoreSA::EnsureCollisionIsInMemory(const CVector& position)
{
    // Wait for GTA to complete initialization before calling collision functions
    // Race condition: MTA can trigger streaming/collision operations before GTA completes initialization.
    // GTA calls CTimer::Initialise at 0x53BDE6 during startup, which sets _timerFunction at 0x56189E.
    // If called before GTA reaches GS_INIT_PLAYING_GAME, the timer isn't initialized > crash at 0x5619E9 (CTimer::Suspend)

    if (!pGame || pGame->GetSystemState() < SystemState::GS_INIT_PLAYING_GAME)
        return;  // GTA not ready yet - skip (will retry on next streaming update)

    // Just in case
    constexpr auto ADDR_timerFunction = 0xB7CB28;
    const auto     timerFunction = *reinterpret_cast<void* const*>(ADDR_timerFunction);
    if (!timerFunction)
        return;  // Timer not initialized yet - skip

    // SA function signature: void __cdecl CColStore::EnsureCollisionIsInMemory(const CVector2D&)
    // CVector implicitly converts to CVector2D (uses x, y components only)
    using Signature = void(__cdecl*)(const CVector&);
    const auto function = reinterpret_cast<Signature>(0x410AD0);
    function(position);
}

bool CColStoreSA::HasCollisionLoaded(const CVector& position, int areaCode)
{
    using Signature = bool(__cdecl*)(const CVector&, int);
    const auto function = reinterpret_cast<Signature>(0x410CE0);
    return function(position, areaCode);
}

void CColStoreSA::RequestCollision(const CVector& position, int areaCode)
{
    using Signature = void(__cdecl*)(const CVector&, int);
    const auto function = reinterpret_cast<Signature>(0x410C00);
    function(position, areaCode);
}

void CColStoreSA::SetCollisionRequired(const CVector& position, int areaCode)
{
    using Signature = void(__cdecl*)(const CVector&, int);
    const auto function = reinterpret_cast<Signature>(0x4104E0);
    function(position, areaCode);
}

void CColStoreSA::RemoveAllCollision()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x410E00);
    function();
}

void CColStoreSA::AddRef(CollisionSlot slot)
{
    // Native signature: void __cdecl CColStore::AddRef(int)
    // Must use int to match GTA SA's ABI - it reads a full DWORD from stack
    using Signature = void(__cdecl*)(int);
    const auto function = reinterpret_cast<Signature>(0x4107A0);
    function(static_cast<int>(slot));
}

void CColStoreSA::RemoveRef(CollisionSlot slot)
{
    // Native signature: void __cdecl CColStore::RemoveRef(int)
    // Must use int to match GTA SA's ABI - it reads a full DWORD from stack
    using Signature = void(__cdecl*)(int);
    const auto function = reinterpret_cast<Signature>(0x4107D0);
    function(static_cast<int>(slot));
}

void CColStoreSA::RemoveCol(CollisionSlot slot)
{
    // Native expects int parameter
    using Signature = void(__cdecl*)(int);
    const auto function = reinterpret_cast<Signature>(0x410730);
    function(static_cast<int>(slot));
}

void CColStoreSA::RemoveColSlot(CollisionSlot slot)
{
    // Native expects int parameter
    using Signature = void(__cdecl*)(int);
    const auto function = reinterpret_cast<Signature>(0x411330);
    function(static_cast<int>(slot));
}

void CColStoreSA::LoadAllBoundingBoxes()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x4113D0);
    function();
}

CColStore::BoundingBox CColStoreSA::GetBoundingBox(CollisionSlot slot)
{
    // Native expects int parameter
    using Signature = BoundingBox&(__cdecl*)(int);
    const auto function = reinterpret_cast<Signature>(0x410800);
    return function(static_cast<int>(slot));
}

void CColStoreSA::IncludeModelIndex(CollisionSlot slot, std::uint16_t model)
{
    // Native expects int parameters
    using Signature = void(__cdecl*)(int, int);
    const auto function = reinterpret_cast<Signature>(0x410820);
    function(static_cast<int>(slot), static_cast<int>(model));
}

int CColStoreSA::GetFirstModel(CollisionSlot slot)
{
    // Native expects int parameter
    using Signature = int(__cdecl*)(int);
    const auto function = reinterpret_cast<Signature>(0x537A80);
    return function(static_cast<int>(slot));
}

int CColStoreSA::GetLastModel(CollisionSlot slot)
{
    // Native expects int parameter
    using Signature = int(__cdecl*)(int);
    const auto function = reinterpret_cast<Signature>(0x537AB0);
    return function(static_cast<int>(slot));
}
