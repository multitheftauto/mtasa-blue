/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CRemoteDataSA.cpp
*  PURPOSE:     Remote data storage class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// These includes have to be fixed!
#include "../game_sa/CPedSA.h"
#include "../game_sa/CVehicleSA.h"

using std::list;

// Our set of remote data
list < CRemoteDataStorageSA* >          m_RemoteData;
CPools *                                m_pPools;

void CRemoteDataSA::Init ( )
{
    m_pPools = pGameInterface->GetPools();
}

CRemoteDataStorageSA * CRemoteDataSA::GetRemoteDataStorage ( CPlayerPed * player )
{
    list < CRemoteDataStorageSA* > ::iterator iter = m_RemoteData.begin ();
    for ( ; iter != m_RemoteData.end (); iter++ )
    {
        if ( (*iter)->m_pPlayer == player )
        {
            return *iter;
        }
    }
    return NULL;
}

CRemoteDataStorageSA * CRemoteDataSA::GetRemoteDataStorage ( CPedSAInterface * ped )
{
    CPlayerPed * pPed = dynamic_cast < CPlayerPed* > ( m_pPools->GetPed ( (DWORD *)ped ) );
    if ( pPed )
        return GetRemoteDataStorage ( pPed );
    return NULL;
}

bool CRemoteDataSA::AddRemoteDataStorage ( CRemoteDataStorage * data )
{
    m_RemoteData.push_back ( (CRemoteDataStorageSA *)data );
    return true;
}

void CRemoteDataSA::RemoveRemoteDataStorage ( CRemoteDataStorage * data )
{
    if ( !m_RemoteData.empty() ) m_RemoteData.remove ( (CRemoteDataStorageSA *)data );
}
