/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVehicleModelInfoSA.cpp
 *  PURPOSE:     Vehicle model info class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelInfoSA.h"
#include "gamesa_renderware.h"
#include "enums/VehicleClass.h"

#define NUM_DIRT_TEXTURES 16
#define NUM_MAX_DESCS 12

static RwTexture** ms_aDirtTextures = *(RwTexture*(*)[NUM_DIRT_TEXTURES])0xC02BD0;
static RwObjectNameIdAssocation** ms_vehicleDescs = *(RwObjectNameIdAssocation*(*)[NUM_MAX_DESCS])0x8A7740;

static bool fixVehicleDirtLevel = false;

struct VehicleMaterialCBData
{
    CVehicleModelInfoSAInterface* modelInfo;
    int                           numDirt;
};

void CVehicleModelInfoSAInterface::SetAtomicRenderCallbacks()
{
    ((void(__thiscall*)(CVehicleModelInfoSAInterface*))0x4C7B10)(this);
}

void CVehicleModelInfoSAInterface::SetRenderPipelines()
{
    ((void(__thiscall*)(CVehicleModelInfoSAInterface*))0x4C8900)(this);
}

void CVehicleModelInfoSAInterface::PreprocessHierarchy()
{
    ((void(__thiscall*)(CVehicleModelInfoSAInterface*))0x4C8E60)(this);
}

void CVehicleModelInfoSAInterface::SetClump(RpClump* clump)
{
    // Call CPool_CVehicleStruct::allocate & CVehicleModelInfo::CVehicleStructure::CVehicleStructure constructor
    auto* vehicleStruct = ((CVehicleModelVisualInfoSAInterface * (__cdecl*)())0x4C9570)();
    pVisualInfo = vehicleStruct ? ((CVehicleModelVisualInfoSAInterface * (__thiscall*)(CVehicleModelVisualInfoSAInterface*))0x4C8D60)(vehicleStruct) : nullptr;

    static_cast<CClumpModelInfoSAInterface*>(this)->SetClump(clump);
    SetAtomicRenderCallbacks();
    static_cast<CClumpModelInfoSAInterface*>(this)->SetFrameIds(ms_vehicleDescs[vehicleType]);
    SetRenderPipelines();
    PreprocessHierarchy();
    ReduceMaterialsInVehicle();

    if (fixVehicleDirtLevel)
        FindEditableMaterialList();

    SetCarCustomPlate();
}

void CVehicleModelInfoSAInterface::ReduceMaterialsInVehicle()
{
    ((void(__thiscall*)(CVehicleModelInfoSAInterface*))0x4C8BD0)(this);
}

void CVehicleModelInfoSAInterface::SetCarCustomPlate()
{
    ((void(__thiscall*)(CVehicleModelInfoSAInterface*))0x4C9450)(this);
}

void CVehicleModelInfoSAInterface::SetDirtTextures(CVehicleModelInfoSAInterface* model, int dirtLevel)
{
    if (!fixVehicleDirtLevel)
        return;

    for (int i = 0; i < NUM_DIRT_TEXTURES; ++i)
    {
        if (!model->m_dirtMaterials[i])
            continue;

        RpMaterialSetTexture(model->m_dirtMaterials[i], ms_aDirtTextures[dirtLevel]);
    }
}

bool CVehicleModelInfoSAInterface::GetEditableMaterialListCB(RpAtomic* atomic, void* data)
{
    RpGeometryForAllMaterials(atomic->geometry,
        +[](RpMaterial* material, void* cbData) -> RpMaterial*
        {
            auto* materialData = static_cast<VehicleMaterialCBData*>(cbData);
            RwTexture*           tex = material->texture;
            if (tex && std::strcmp(tex->name, "vehiclegrunge256") == 0)
                materialData->modelInfo->m_dirtMaterials[materialData->numDirt++] = material;

            return material;
        },
    data);

    return true;
}

void CVehicleModelInfoSAInterface::FindEditableMaterialList()
{
    VehicleMaterialCBData cbData{};
    cbData.modelInfo = this;
    cbData.numDirt = 0;

    RpClumpForAllAtomics(reinterpret_cast<RpClump*>(pRwObject), GetEditableMaterialListCB, &cbData);
    for (int i = 0; i < pVisualInfo->m_numExtras; i++)
        GetEditableMaterialListCB(pVisualInfo->m_pExtra[i], &cbData);

    lastColours[0] = 255;
    lastColours[1] = 255;
    lastColours[2] = 255;
    lastColours[3] = 255;
}

bool CVehicleModelInfoSAInterface::IsComponentDamageable(int componentIndex) const
{
    return pVisualInfo->m_maskComponentDamagable & (1 << componentIndex);
}

void CVehicleModelInfoSAInterface::SetVehicleDirtLevelFixEnabled(bool enabled) noexcept
{
    fixVehicleDirtLevel = enabled;
}

static void CVehicleModelInfo__SetClump()
{
    _asm
    {
        mov eax, [esp+8]
        push eax
        call CVehicleModelInfoSAInterface::SetClump
        add esp, 4
        retn 4
    }
}

void CVehicleModelInfoSAInterface::StaticSetHooks()
{
    HookInstall(0x4C95C0, CVehicleModelInfo__SetClump);
    HookInstallCall(0x6D0E7E, (DWORD)CVehicleModelInfoSAInterface::SetDirtTextures);
}
