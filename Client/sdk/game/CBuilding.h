/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CBuilding.h
 *  PURPOSE:     Physical entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CEntity.h"

class CBuildingSAInterface;

class CBuilding : public virtual CEntity
{
public:
    virtual ~CBuilding(){};

    virtual CBuildingSAInterface* GetBuildingInterface() = 0;
    virtual void                  SetLod(CBuilding* pLod) = 0;
};
