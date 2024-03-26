#pragma once

#include "CEntitySA.h"
#include "CPtrNodeSingleListSA.h"

class CCoverManagerSA
{
public:
    CCoverManagerSA();
    ~CCoverManagerSA() = default;

    void RemoveAllCovers();
    void RemoveCover(CEntitySAInterface* entity);

private:
    CPtrNodeSingleListSAInterface<CEntitySAInterface>* m_pCoverList;
};
