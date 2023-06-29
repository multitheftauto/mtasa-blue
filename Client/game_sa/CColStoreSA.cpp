/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CColStoreSA.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColStoreSA.h"

void CColStoreSA::Initialise()
{
    (reinterpret_cast<void(__cdecl*)()>(0x4113F0))();
}

void CColStoreSA::Shutdown()
{
    (reinterpret_cast<void(__cdecl*)()>(0x4114D0))();
}

void CColStoreSA::BoundingBoxesPostProcess()
{
    (reinterpret_cast<void(__cdecl*)()>(0x410EC0))();
}

int CColStoreSA::AddColSlot(const char* name)
{
    return (reinterpret_cast<int(__cdecl*)(const char*)>(0x411140))(name);
}

bool CColStoreSA::IsValidSlot(CollisionSlot slot)
{
    return (reinterpret_cast<bool(__cdecl*)(CollisionSlot)>(0x410660))(slot);
}

void CColStoreSA::AddCollisionNeededAtPosition(const CVector& position)
{
    (reinterpret_cast<void(__cdecl*)(const CVector&)>(0x4103A0))(position);
}

void CColStoreSA::EnsureCollisionIsInMemory(const CVector& position)
{
    (reinterpret_cast<void(__cdecl*)(const CVector&)>(0x410AD0))(position);
}

bool CColStoreSA::HasCollisionLoaded(const CVector& position, int areaCode)
{
    return (reinterpret_cast<bool(__cdecl*)(const CVector&, int)>(0x410CE0))(position, areaCode);
}

void CColStoreSA::RequestCollision(const CVector& position, int areaCode)
{
    (reinterpret_cast<void(__cdecl*)(const CVector&, int)>(0x410C00))(position, areaCode);
}

void CColStoreSA::SetCollisionRequired(const CVector& position, int areaCode)
{
    (reinterpret_cast<void(__cdecl*)(const CVector&, int)>(0x4104E0))(position, areaCode);
}

void CColStoreSA::RemoveAllCollision()
{
    (reinterpret_cast<void(__cdecl*)()>(0x410E00))();
}

void CColStoreSA::AddRef(CollisionSlot slot)
{
    (reinterpret_cast<void(__cdecl*)(CollisionSlot)>(0x4107A0))(slot);
}

void CColStoreSA::RemoveRef(CollisionSlot slot)
{
    (reinterpret_cast<void(__cdecl*)(CollisionSlot)>(0x4107D0))(slot);
}

void CColStoreSA::RemoveCol(CollisionSlot slot)
{
    (reinterpret_cast<void(__cdecl*)(CollisionSlot)>(0x410730))(slot);
}

void CColStoreSA::RemoveColSlot(CollisionSlot slot)
{
    (reinterpret_cast<void(__cdecl*)(CollisionSlot)>(0x411330))(slot);
}

void CColStoreSA::LoadAllBoundingBoxes()
{
    (reinterpret_cast<void(__cdecl*)()>(0x4113D0))();
}

CColStore::BoundingBox CColStoreSA::GetBoundingBox(CollisionSlot slot)
{
    return (reinterpret_cast<BoundingBox&(__cdecl*)(CollisionSlot)>(0x410800))(slot);
}

void CColStoreSA::IncludeModelIndex(CollisionSlot slot, std::uint16_t model)
{
    (reinterpret_cast<void(__cdecl*)(CollisionSlot, int)>(0x410820))(slot, model);
}

int CColStoreSA::GetFirstModel(CollisionSlot slot)
{
    return (reinterpret_cast<int(__cdecl*)(CollisionSlot)>(0x537A80))(slot);
}

int CColStoreSA::GetLastModel(CollisionSlot slot)
{
    return (reinterpret_cast<int(__cdecl*)(CollisionSlot)>(0x537AB0))(slot);
}
