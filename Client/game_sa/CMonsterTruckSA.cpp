/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CMonsterTruckSA.cpp
 *  PURPOSE:     Monster truck vehicle entity
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMonsterTruckSA.h"

CMonsterTruckSA::CMonsterTruckSA(CMonsterTruckSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
