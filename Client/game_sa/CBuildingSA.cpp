/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingSA.cpp
 *  PURPOSE:     Building entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBuildingSA.h"
#include <game/CWorld.h>
#include "CGameSA.h"

extern CGameSA* pGame;

CBuildingSA::CBuildingSA(CBuildingSAInterface* pInterface)
{
    SetInterface(pInterface);
}

void CBuildingSA::SetLod(CBuilding* pLod)
{
    if (pLod)
    {
        if (m_pInterface->m_pLod)
        {
            SetLod(nullptr);
        }

        CBuildingSAInterface* pLodInterface = dynamic_cast<CBuildingSA*>(pLod)->GetBuildingInterface();
        assert(pLodInterface);

        // We should recreate buildings...
        pGame->GetWorld()->Remove(pLodInterface, CBuilding_SetLod);
        pGame->GetWorld()->Remove(m_pInterface, CBuilding_SetLod);

        m_pInterface->m_pLod = pLodInterface;
        pLodInterface->bUsesCollision = 0;
        pLodInterface->numLodChildren = 1;

        if (pGame->GetModelInfo(pLodInterface->m_nModelIndex)->GetLODDistance() > 300)
        {
            pLodInterface->bIsBIGBuilding = 1;
        }

        // Only this specific order works
        pGame->GetWorld()->Add(m_pInterface, CBuilding_SetLod);
        pGame->GetWorld()->Add(pLodInterface, CBuilding_SetLod);
    }
    else
    {
        CEntitySAInterface* pCurrentLod = m_pInterface->m_pLod;
        if (pCurrentLod)
        {
            pGame->GetWorld()->Remove(pCurrentLod, CBuilding_SetLod);

            m_pInterface->m_pLod = nullptr;
            pCurrentLod->bIsBIGBuilding = false;
            pCurrentLod->numLodChildren = 0;

            pGame->GetWorld()->Add(pCurrentLod, CBuilding_SetLod);
        }
    }
}
