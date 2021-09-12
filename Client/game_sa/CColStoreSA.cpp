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
    using Signature = bool(__cdecl*)(CollisionSlot);
    const auto function = reinterpret_cast<Signature>(0x410660);
    return function(slot);
}

void CColStoreSA::AddCollisionNeededAtPosition(const CVector& position)
{
    using Signature = void(__cdecl*)(const CVector&);
    const auto function = reinterpret_cast<Signature>(0x4103A0);
    function(position);
}

void CColStoreSA::EnsureCollisionIsInMemory(const CVector& position)
{
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
    using Signature = void(__cdecl*)(CollisionSlot);
    const auto function = reinterpret_cast<Signature>(0x4107A0);
    function(slot);
}

void CColStoreSA::RemoveRef(CollisionSlot slot)
{
    using Signature = void(__cdecl*)(CollisionSlot);
    const auto function = reinterpret_cast<Signature>(0x4107D0);
    function(slot);
}

void CColStoreSA::RemoveCol(CollisionSlot slot)
{
    using Signature = void(__cdecl*)(CollisionSlot);
    const auto function = reinterpret_cast<Signature>(0x410730);
    function(slot);
}

void CColStoreSA::RemoveColSlot(CollisionSlot slot)
{
    using Signature = void(__cdecl*)(CollisionSlot);
    const auto function = reinterpret_cast<Signature>(0x411330);
    function(slot);
}

void CColStoreSA::LoadAllBoundingBoxes()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x4113D0);
    function();
}

CColStore::BoundingBox CColStoreSA::GetBoundingBox(CollisionSlot slot)
{
    using Signature = BoundingBox&(__cdecl*)(CollisionSlot);
    const auto function = reinterpret_cast<Signature>(0x410800);
    return function(slot);
}

void CColStoreSA::IncludeModelIndex(CollisionSlot slot, std::uint16_t model)
{
    using Signature = void(__cdecl*)(CollisionSlot, int);
    const auto function = reinterpret_cast<Signature>(0x410820);
    function(slot, model);
}

int CColStoreSA::GetFirstModel(CollisionSlot slot)
{
    using Signature = int(__cdecl*)(CollisionSlot);
    const auto function = reinterpret_cast<Signature>(0x537A80);
    return function(slot);
}

int CColStoreSA::GetLastModel(CollisionSlot slot)
{
    using Signature = int(__cdecl*)(CollisionSlot);
    const auto function = reinterpret_cast<Signature>(0x537AB0);
    return function(slot);
}
