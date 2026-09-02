/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientWaterCannonManager.cpp
 *  PURPOSE:     Water cannon entity manager class
 *
 *****************************************************************************/

#include <StdInc.h>

CClientWaterCannonManager::CClientWaterCannonManager(CClientManager* pManager)
{
    m_pManager = pManager;
    m_bDontRemoveFromList = false;
}

CClientWaterCannonManager::~CClientWaterCannonManager()
{
    DeleteAll();
}

CClientWaterCannon* CClientWaterCannonManager::Create(ElementID ID)
{
    if (Get(ID))
        return nullptr;

    CClientWaterCannon* pCannon = new CClientWaterCannon(m_pManager, ID);
    if (!pCannon->HasNativeCannon())
    {
        // The custom-cannon pool (separate from the vehicle-owned one, but still capped) is full
        delete pCannon;
        return nullptr;
    }

    return pCannon;
}

void CClientWaterCannonManager::DeleteAll()
{
    m_bDontRemoveFromList = true;
    for (CClientWaterCannon* pCannon : m_List)
        delete pCannon;
    m_bDontRemoveFromList = false;

    m_List.clear();
}

CClientWaterCannon* CClientWaterCannonManager::Get(ElementID ID)
{
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTWATERCANNON)
        return static_cast<CClientWaterCannon*>(pEntity);

    return nullptr;
}

void CClientWaterCannonManager::DoPulse()
{
    for (CClientWaterCannon* pCannon : m_List)
        pCannon->DoPulse();
}

bool CClientWaterCannonManager::IsKnockdownEnabled(void* pWaterCannonInterface) const
{
    for (CClientWaterCannon* pCannon : m_List)
    {
        if (pCannon->GetNativeHandle() == pWaterCannonInterface)
            return pCannon->IsKnockdownEnabled();
    }
    return true;
}

void CClientWaterCannonManager::RemoveFromList(CClientWaterCannon* pCannon)
{
    if (!m_bDontRemoveFromList && !m_List.empty())
        m_List.remove(pCannon);
}
