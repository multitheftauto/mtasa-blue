
#include "StdInc.h"
#include "CCoverManagerSA.h"

CCoverManagerSA::CCoverManagerSA()
{
    m_pCoverList = reinterpret_cast<CPtrNodeSingleListSAInterface<CEntitySAInterface>*>(0xC1A2B8);
}

void CCoverManagerSA::RemoveAllCovers()
{
    while (m_pCoverList->m_pList)
    {
        RemoveCover(m_pCoverList->m_pList->pItem);
    }
}

void CCoverManagerSA::RemoveCover(CEntitySAInterface* entity)
{
    m_pCoverList->RemoveItem(entity);

    //using CCover_RemoveCoverPointsForThisEntity = char (__cdecl*)(CEntitySAInterface*);
    //((CCover_RemoveCoverPointsForThisEntity)0x698740)(entity);
}
