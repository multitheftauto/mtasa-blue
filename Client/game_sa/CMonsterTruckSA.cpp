/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMonsterTruckSA.cpp
 *  PURPOSE:     Monster truck vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMonsterTruckSA.h"

CMonsterTruckSA::CMonsterTruckSA(CMonsterTruckSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
