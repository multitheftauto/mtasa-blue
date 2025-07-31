/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCoverManagerSA.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CEntitySA.h"
#include "CPtrNodeDoubleListSA.h"

class CCoverManagerSA
{
public:
    CCoverManagerSA();
    ~CCoverManagerSA() = default;

    void RemoveAllCovers();
    void RemoveCover(CEntitySAInterface* entity);

private:
    void RemoveCoverFromArray(CEntitySAInterface* entity);

private:
    CPtrNodeDoubleListSAInterface<CEntitySAInterface>* m_pCoverList;
};
