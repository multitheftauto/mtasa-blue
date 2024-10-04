#pragma once

#include "CEntitySA.h"

#define FUNC_CPlantColEntEntry_ReleaseEntry 0x5DB8A0
#define FUNC_CPlantColEntEntry_Remove       0x5DBEF0

class CPlantManagerSA
{
public:
    CPlantManagerSA() = default;
    ~CPlantManagerSA() = default;

    void RemovePlant(CEntitySAInterface* enity);

    void RemoveAllPlants();
};
