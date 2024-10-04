
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
        CallGTAFunction<void*, __THISCALL>(FUNC_CPlantColEntEntry_ReleaseEntry, PrepareSignature(this));
    };
};

void CPlantManagerSA::RemovePlant(CEntitySAInterface* enity)
{
    CallGTAFunction<CEntitySAInterface*, __CDECL>(FUNC_CPlantColEntEntry_Remove, PrepareSignature(enity));
}

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
