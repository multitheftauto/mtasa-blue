
#include "StdInc.h"
#include "CPlantManagerSA.h"

void CPlantManagerSA::RemoveAllPlants()
{
    int    removedCount = 0;
    void* list = *(void**)0xC0399C;
    while (list != nullptr)
    {
        RemovePlant((CEntitySAInterface*)list);
        removedCount++;
    }
    int a = removedCount;
}
