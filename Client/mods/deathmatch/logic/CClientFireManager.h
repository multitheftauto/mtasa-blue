/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFireManager.h
 *  PURPOSE:     Fire manager class header
 *
 *****************************************************************************/

#pragma once

#include "CClientFire.h",
#include <unordered_map>

class CClientFireManager
{
    friend class CClientFire;

public:
    CClientFireManager() = default;
    ~CClientFireManager() { RemoveAll(); }

    void RemoveAll();

    CClientFire* Get(CFire* gameFire);

private:
    void AddToList(CClientFire* fire) { m_list[fire->GetGameFire()] = fire; };
    void RemoveFromList(CClientFire* fire) { m_list.erase(fire->GetGameFire()); }

    std::unordered_map<CFire*, CClientFire*> m_list;
};
