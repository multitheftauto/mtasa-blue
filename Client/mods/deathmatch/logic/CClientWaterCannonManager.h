/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientWaterCannonManager.h
 *  PURPOSE:     Water cannon entity manager class header
 *
 *****************************************************************************/

#pragma once

#include "CClientWaterCannon.h"
#include <list>

class CClientWaterCannonManager
{
    friend class CClientManager;
    friend class CClientWaterCannon;

public:
    CClientWaterCannonManager(CClientManager* pManager);
    ~CClientWaterCannonManager();

    CClientWaterCannon* Create(ElementID ID);

    void DeleteAll();

    std::list<CClientWaterCannon*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CClientWaterCannon*>::const_iterator IterEnd() { return m_List.end(); };

    static CClientWaterCannon* Get(ElementID ID);

    void DoPulse();

    // Traces a hit back to the CClientWaterCannon that owns the raw native CWaterCannon instance
    // (CClientWaterCannon::GetNativeHandle) it happened on; null for a handle this manager doesn't
    // recognise (a real vehicle-owned cannon's native instance, most likely).
    CClientWaterCannon* GetByNativeHandle(void* pWaterCannonInterface) const;

private:
    void AddToList(CClientWaterCannon* pCannon) { m_List.push_back(pCannon); };
    void RemoveFromList(CClientWaterCannon* pCannon);

    CClientManager* m_pManager;

    std::list<CClientWaterCannon*> m_List;
    bool                           m_bDontRemoveFromList;
};
