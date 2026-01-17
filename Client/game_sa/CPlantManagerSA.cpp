
#include "StdInc.h"
#include "CPlantManagerSA.h"
#include "CPtrNodeSingleListSA.h"

class CPlantLocTri;

class CPlantColEntEntry
{
public:
    CEntitySAInterface* m_Entity;
    CPlantLocTri**     m_Objects;
    uint16             m_numTriangles;
    CPlantColEntEntry* m_NextEntry;
    CPlantColEntEntry* m_PrevEntry;

public:
    void               ReleaseEntry() {
        using CPlantColEntEntry_ReleaseEntry = void* ( __thiscall *)(CPlantColEntEntry*);
        ((CPlantColEntEntry_ReleaseEntry)0x5DB8A0)(this);
    };
};

void CPlantManagerSA::RemoveAllPlants()
{
    while (true)
    {
        auto list = reinterpret_cast<CPlantColEntEntry**>(0xC0399C);
        if (*list == nullptr)
        {
            break;
        }

        (*list)->ReleaseEntry();
    }
}
