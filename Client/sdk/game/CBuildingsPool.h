/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CBuildingsPool.h
 *  PURPOSE:     Buildings pool interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include <CVector.h>

class CBuilding;
class CBuildingSA;

class CBuildingsPool
{
public:

    // Buildings pool
    virtual CBuilding* AddBuilding(class CClientBuilding*, uint16_t modelId, CVector* vPos, CVector4D* vRot, uint8_t interior) = 0;
    virtual void       RemoveBuilding(CBuilding* pObject) = 0;
    virtual bool       HasFreeBuildingSlot() = 0;
    virtual bool       Resize(int size) = 0;
    virtual int        GetSize() const = 0;

    virtual void RemoveAllBuildings() = 0;
    virtual void RestoreAllBuildings() = 0;
};
