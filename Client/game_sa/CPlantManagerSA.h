#pragma once

#include "CEntitySA.h"

class CPlantManagerSA
{
public:
    CPlantManagerSA() = default;
    ~CPlantManagerSA() = default;

    void RemovePlant(CEntitySAInterface* enity)
    {
        using CPlantColEntry_Remove = CEntitySAInterface* (*)(CEntitySAInterface*);
        ((CPlantColEntry_Remove)0x5DBEF0)(enity);
    };

    void RemoveAllPlants();
};
