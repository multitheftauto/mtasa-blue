/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CMonsterTruckSA.h
 *  PURPOSE:     Header file for monster truck vehicle entity class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CMonsterTruck.h>
#include "CAutomobileSA.h"

class CMonsterTruckSAInterface : public CAutomobileSAInterface {};

class CMonsterTruckSA final : public virtual CMonsterTruck, public virtual CAutomobileSA
{
public:
    CMonsterTruckSA(CMonsterTruckSAInterface* pInterface);

    CMonsterTruckSAInterface* GetMonsterTruckInterface() { return reinterpret_cast<CMonsterTruckSAInterface*>(GetInterface()); }
};
