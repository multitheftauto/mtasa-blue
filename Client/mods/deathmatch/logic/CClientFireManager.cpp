/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFireManager.cpp
 *  PURPOSE:     Fire manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientFireManager.h"

void CClientFireManager::RemoveAll()
{
    for (const auto& pair : m_list)
        delete pair.second;

    m_list.clear();
}

CClientFire* CClientFireManager::Get(CFire* gameFire)
{
    if (!gameFire)
        return nullptr;

    auto it = m_list.find(gameFire);
    if (it != m_list.end())
        return it->second;

    return nullptr;
}
