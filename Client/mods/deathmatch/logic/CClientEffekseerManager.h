/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPedManager.h
 *  PURPOSE:     Ped entity manager class
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "CClientEffekseerEffect.h"

class CClientManager;
class CEntity;

class CClientEffekseerManager
{
    friend class CClientManager;

public:
    void DeleteAll();

    CClientEffekseerEffect* Get(ElementID ID);

protected:
    CClientEffekseerManager(class CClientManager* pManager);
    ~CClientEffekseerManager();

    void AddToList(CClientEffekseerEffect* pEffekseerFX) { m_List.push_back(pEffekseerFX); }

    CClientManager*                      m_pManager;
    std::vector<CClientEffekseerEffect*> m_List;
    bool                                 m_bCanRemoveFromList;
};
