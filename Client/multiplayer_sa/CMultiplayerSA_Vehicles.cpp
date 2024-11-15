/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Vehicles.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "..\game_sa\gamesa_renderware.h"

#define FUNC_CBaseModelInfo_Shutdown       0x4C4D50
#define IN_PLACE_BUFFER_DIRT_SIZE          30

static RwTexture** const ms_aDirtTextures = (RwTexture**)0xC02BD0;

static bool __fastcall AreVehicleDoorsUndamageable(CVehicleSAInterface* vehicle)
{
    SClientEntity<CVehicleSA>* pair = pGameInterface->GetPools()->GetVehicle((DWORD*)vehicle);

    if (!pair)
        return false;

    return pair->pEntity->AreDoorsUndamageable();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CDamageManager::ProgressDoorDamage
//
// This hook checks if our CVehicleSA instance prevents door damage
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C2320 | 53             | push    ebx
//     0x6C2321 | 56             | push    esi
//     0x6C2322 | 0F B6 74 24 0C | movzx   esi, [esp + doorId]
//     0x6C2327 | 85 F6          | test    esi, esi
#define HOOKPOS_CDamageManager__ProgressDoorDamage         0x6C2320
#define HOOKSIZE_CDamageManager__ProgressDoorDamage        7
static DWORD CONTINUE_CDamageManager__ProgressDoorDamage = 0x6C2327;

static void _declspec(naked) HOOK_CDamageManager__ProgressDoorDamage()
{
    _asm
    {
        pushad
        mov     ecx, [esp + 08h]        // CAutomobileSAInterface*
        call    AreVehicleDoorsUndamageable
        test    al, al
        jz      continueGameCodeLocation

        popad
        mov     al, 0
        retn    8

        continueGameCodeLocation:
        popad
        push    ebx
        push    esi
        movzx   esi, [esp + 0Ch]
        jmp     CONTINUE_CDamageManager__ProgressDoorDamage
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAEVehicleAudioEntity::Initialise
//
// This hook setup a audio interfeace for custom vehicles
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x4F77C1 | 8D 34 C0              | lea esi, [eax + eax * 8]
//     0x4F77C4 | 8D BD 80 00 00 00     | lea edi, [ebp + 80h]
//     0x4F77CA | 8D 34 B5 F0 0A 86 00  | lea esi, _VehicleAudioProperties.m_eVehicleSoundType[esi * 4]
//     0x4F77D1 | B9 09 00 00 00        | mov ecx,9
//     0x4F77D6 | F3 A5                 | rep movsd
//     0x4F77D8 | 8B CA                 | mov ecx, edx;

static CVehicleSAInterface* pRequestSoundSettingsVehicle = 0;
tVehicleAudioSettings*      pVehicleAudioSettings = nullptr;

static tVehicleAudioSettings* __fastcall getVehicleSoundSettings()
{
    ushort usModel = pRequestSoundSettingsVehicle->m_nModelIndex;
    // Check if it is a custom model
    if (usModel < VT_LANDSTAL || usModel >= VT_MAX)
        usModel = pGameInterface->GetModelInfo(usModel)->GetParentID();
    return (tVehicleAudioSettings*)0x860AF0 + (usModel - VT_LANDSTAL);
}

#define HOOKPOS_CAEVehicleAudioEntity__Initialise  0x4F77C1
#define HOOKSIZE_CAEVehicleAudioEntity__Initialise 0x10
static DWORD CONTINUE_CAEVehicleAudioEntity__Initialise = 0x4F77D1;

static void _declspec(naked) HOOK_CAEVehicleAudioEntity__Initialise()
{
    _asm
    {
        pushad
        mov     pRequestSoundSettingsVehicle, edx
    }

    pVehicleAudioSettings = getVehicleSoundSettings();

    _asm
    {
        popad
        lea    edi, [ebp + 80h]
        mov    esi, pVehicleAudioSettings
        jmp CONTINUE_CAEVehicleAudioEntity__Initialise
    }
}

static void __fastcall CVehicleModelInfo_Shutdown(CVehicleModelInfoSAInterface* mi)
{
    if (!mi)
        return;

    mi->Shutdown();

    delete[] mi->m_dirtMaterials;
    mi->m_dirtMaterials = nullptr;
}

static void SetDirtTextures(CVehicleModelInfoSAInterface* mi, std::uint32_t level)
{
    RpMaterial** materials = mi->m_numDirtMaterials > IN_PLACE_BUFFER_DIRT_SIZE ? mi->m_dirtMaterials : mi->m_staticDirtMaterials;
    for (std::uint32_t i = 0; i < mi->m_numDirtMaterials; i++)
        RpMaterialSetTexture(materials[i], ms_aDirtTextures[level]);
}

#define HOOKPOS_CVehicleModelInfo_SetDirtTextures 0x5D5DBB
#define HOOKSIZE_CVehicleModelInfo_SetDirtTextures 6
static constexpr DWORD CONTINUE_CVehicleModelInfo_SetDirtTextures = 0x5D5DE3;
static void _declspec(naked) HOOK_CVehicleModelInfo_SetDirtTextures()
{
    _asm
    {
        push ebx
        push esi
        call SetDirtTextures
        add esp, 8

        jmp CONTINUE_CVehicleModelInfo_SetDirtTextures
    }
}

static RpMaterial* GetAtomicGeometryMaterialsCB(RpMaterial* material, void* data)
{
    RwTexture* texture = material->texture;
    if (!texture)
        return nullptr;

    auto cbData = static_cast<std::vector<RpMaterial*>*>(data);
    if (texture->name && std::strcmp(texture->name, "vehiclegrunge256") == 0)
        cbData->push_back(material);

    return material;
}

static bool GetEditableMaterialListCB(RpAtomic* atomic, void* data)
{
    RpGeometryForAllMaterials(atomic->geometry, &GetAtomicGeometryMaterialsCB, data);
    return true;
}

static void __fastcall FindEditableMaterialList(CVehicleModelInfoSAInterface* mi)
{
    std::vector<RpMaterial*> list;
    RpClumpForAllAtomics(reinterpret_cast<RpClump*>(mi->pRwObject), &GetEditableMaterialListCB, &list);

    for (std::uint32_t i = 0; i < mi->pVisualInfo->m_numExtras; i++)
        GetEditableMaterialListCB(mi->pVisualInfo->m_pExtra[i], &list);

    mi->m_numDirtMaterials = list.size();
    if (mi->m_numDirtMaterials > IN_PLACE_BUFFER_DIRT_SIZE)
    {
        mi->m_dirtMaterials = new RpMaterial*[mi->m_numDirtMaterials];
        std::copy(list.begin(), list.end(), mi->m_dirtMaterials);
    }
    else
    {
        mi->m_dirtMaterials = nullptr;
        std::copy(list.begin(), list.end(), mi->m_staticDirtMaterials);
    }

    mi->primColor = 255;
    mi->secColor = 255;
    mi->tertColor = 255;
    mi->quatColor = 255;
}

#define HOOKPOS_CVehicleModelInfo_SetClump 0x4C9648
#define HOOKSIZE_CVehicleModelInfo_SetClump 24
static constexpr DWORD CONTINUE_CVehicleModelInfo_SetClump = 0x4C9660;
static void _declspec(naked) HOOK_CVehicleModelInfo_SetClump()
{
    _asm
    {
        push ecx
        call FindEditableMaterialList
        pop ecx

        jmp CONTINUE_CVehicleModelInfo_SetClump
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Vehicles
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Vehicles()
{
    EZHookInstall(CDamageManager__ProgressDoorDamage);
    EZHookInstall(CAEVehicleAudioEntity__Initialise);

    // Fix vehicle dirt level
    EZHookInstall(CVehicleModelInfo_SetClump);
    EZHookInstall(CVehicleModelInfo_SetDirtTextures);
    MemCpy((void*)0x85C5E4, &CVehicleModelInfo_Shutdown, 4);
}
