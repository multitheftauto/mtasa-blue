/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CRemoteDataSA.cpp
 *  PURPOSE:     Remote data storage class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// These includes have to be fixed!
#include "../game_sa/CPedSA.h"

CPools*                                      CRemoteDataSA::m_pPools;
std::map<CPlayerPed*, CRemoteDataStorageSA*> CRemoteDataSA::m_RemoteData;

void CRemoteDataSA::Init()
{
    m_pPools = pGameInterface->GetPools();
}

CRemoteDataStorageSA* CRemoteDataSA::GetRemoteDataStorage(CPlayerPed* pPlayerPed)
{
    std::map<CPlayerPed*, CRemoteDataStorageSA*>::iterator it;
    it = m_RemoteData.find(pPlayerPed);
    if (it != m_RemoteData.end())
        return it->second;

    return NULL;
}

CRemoteDataStorageSA* CRemoteDataSA::GetRemoteDataStorage(CPedSAInterface* ped)
{
    SClientEntity<CPedSA>* pPedClientEntity = m_pPools->GetPed((DWORD*)ped);
    if (pPedClientEntity)
    {
        CPlayerPed* pPed = dynamic_cast<CPlayerPed*>(pPedClientEntity->pEntity);
        if (pPed)
            return GetRemoteDataStorage(pPed);
    }
    return NULL;
}

void CRemoteDataSA::AddRemoteDataStorage(CPlayerPed* pPed, CRemoteDataStorage* pData)
{
    m_RemoteData.insert(std::make_pair(pPed, (CRemoteDataStorageSA*)pData));
}

void CRemoteDataSA::RemoveRemoteDataStorage(CPlayerPed* pPed)
{
    m_RemoteData.erase(pPed);
}
