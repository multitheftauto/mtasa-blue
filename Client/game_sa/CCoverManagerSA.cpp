/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCoverManagerSA.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCoverManagerSA.h"

CCoverManagerSA::CCoverManagerSA()
{
    m_pCoverList = reinterpret_cast<CPtrNodeDoubleListSAInterface<CEntitySAInterface>*>(0xC1A2B8);
}

void CCoverManagerSA::RemoveAllCovers()
{
    CPtrNodeDoubleLink<CEntitySAInterface>* pNode = m_pCoverList->m_pNode;
    while (pNode)
    {
        RemoveCoverFromArray(pNode->pItem);
        pNode = pNode->pNext;
    }
    m_pCoverList->RemoveAllItems();
}

void CCoverManagerSA::RemoveCover(CEntitySAInterface* entity)
{
    RemoveCoverFromArray(entity);

    m_pCoverList->RemoveItem(entity);
}

void CCoverManagerSA::RemoveCoverFromArray(CEntitySAInterface* entity)
{
    using CCover_RemoveCoverPointsForThisEntity = char(__cdecl*)(CEntitySAInterface*);
    ((CCover_RemoveCoverPointsForThisEntity)0x698740)(entity);
}
