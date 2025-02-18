/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingSA.h
 *  PURPOSE:     Header file for game building class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBuilding.h>
#include "CEntitySA.h"

class CBuildingSAInterface : public CEntitySAInterface
{
};
static_assert(sizeof(CBuildingSAInterface) == 0x38, "Invalid size CBuildingSAInterface");

class CBuildingSA final : public virtual CBuilding, public virtual CEntitySA
{
public:
    CBuildingSA(CBuildingSAInterface* pInterface);

    CBuildingSAInterface* GetBuildingInterface() { return static_cast<CBuildingSAInterface*>(GetInterface()); };

    void SetLod(CBuilding* pLod) override;

    void AllocateMatrix();
    void ReallocateMatrix();
    void RemoveAllocatedMatrix();
};
