/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingSA.cpp
 *  PURPOSE:     Building entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBuildingSA.h"
#include <game/CWorld.h>
#include "CGameSA.h"
#include "CMatrixLinkSA.h"
#include "CDynamicPool.h"
#include "CAnimManagerSA.h"
#include "gamesa_renderware.h"

extern CGameSA* pGame;

static CDynamicPool<CMatrixLinkSAInterface, PoolGrowAddStrategy<0, 500>> g_matrixPool{};

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

void CBuildingSA::SetAnimation(CAnimBlendHierarchySAInterface* animation, eAnimationFlags flags)
{
    if (!m_pInterface || !m_pInterface->m_pRwObject)
        return;

    RpClump* clump = reinterpret_cast<RpClump*>(m_pInterface->m_pRwObject);

    if (!RpAnimBlendClumpIsInitialized(clump) && animation)
        RpAnimBlendClumpInit(clump);

    if (animation)
        pGame->GetAnimManager()->BlendAnimation(clump, animation, flags, 1.0f);
}

bool CBuildingSA::SetAnimationSpeed(float speed)
{
    if (!m_pInterface || !m_pInterface->m_pRwObject)
        return false;

    auto assoc = pGame->GetAnimManager()->RpAnimBlendClumpGetFirstAssociation(GetRpClump());
    if (!assoc)
        return false;

    assoc->SetCurrentSpeed(speed);
    return true;
}

void CBuildingSA::AllocateMatrix()
{
    auto* newMatrix = g_matrixPool.AllocateItem();
    std::memset(newMatrix, 0, sizeof(CMatrixLinkSAInterface));
    newMatrix->SetTranslateOnly(m_pInterface->m_transform.m_translate);

    m_pInterface->matrix = reinterpret_cast<CMatrix_Padded*>(newMatrix);
}

void CBuildingSA::ReallocateMatrix()
{
    if (!m_pInterface->HasMatrix())
        return;

    auto* newMatrix = g_matrixPool.AllocateItem();
    std::memcpy(newMatrix, m_pInterface->matrix, sizeof(CMatrixLinkSAInterface));
    newMatrix->m_pOwner = nullptr;
    newMatrix->m_pPrev = nullptr;
    newMatrix->m_pNext = nullptr;

    m_pInterface->RemoveMatrix();
    m_pInterface->matrix = reinterpret_cast<CMatrix_Padded*>(newMatrix);
}

void CBuildingSA::RemoveAllocatedMatrix()
{
    if (!m_pInterface->HasMatrix())
        return;

    CMatrixLinkSAInterface* pMatrix = reinterpret_cast<CMatrixLinkSAInterface*>(m_pInterface->matrix);

    if (pMatrix->m_pOwner || (pMatrix->m_pNext && pMatrix->m_pPrev))
        return;

    g_matrixPool.RemoveItem(reinterpret_cast<CMatrixLinkSAInterface*>(m_pInterface->matrix));
    g_matrixPool.SetCapacity(0);
    m_pInterface->matrix = nullptr;
}
