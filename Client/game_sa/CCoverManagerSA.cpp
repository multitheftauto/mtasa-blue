
#include "StdInc.h"
#include "CCoverManagerSA.h"

CCoverManagerSA::CCoverManagerSA()
{
    m_pCoverList = reinterpret_cast<CPtrNodeSingleListSAInterface<CEntitySAInterface>*>(0xC1A2B8);
}

void CCoverManagerSA::RemoveAllCovers()
{
    m_pCoverList->RemoveAllItems();
}

void CCoverManagerSA::RemoveCover(CEntitySAInterface* entity)
{
    m_pCoverList->RemoveItem(entity);
}
