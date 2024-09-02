/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedSA.cpp
 *  PURPOSE:     Ped entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CPedSA.h"
#include "CPedModelInfoSA.h"
#include "CPlayerInfoSA.h"
#include "CStatsSA.h"
#include "CTaskManagerSA.h"
#include "CTasksSA.h"
#include "CProjectileInfoSA.h"
#include "CWeaponStatManagerSA.h"
#include "CFireManagerSA.h"

extern CGameSA* pGame;

static bool g_onlyUpdateRotations = false;

CPedSA::CPedSA(CPedSAInterface* pPedInterface)
    : m_pPedInterface(pPedInterface)
{
    MemSetFast(m_pWeapons, 0, sizeof(CWeaponSA*) * WEAPONSLOT_MAX);
}

CPedSA::~CPedSA()
{
    SAFE_DELETE(m_pPedIntelligence);
    SAFE_DELETE(m_pPedSound);

    for (std::uint8_t i = 0; i < WEAPONSLOT_MAX; i++)
        SAFE_DELETE(m_pWeapons[i]);

    // Make sure this ped is not refed in the flame shot info array
    CFlameShotInfo* info = (CFlameShotInfo*)ARRAY_CFlameShotInfo;
    for (std::uint8_t i = 0; i < MAX_FLAME_SHOT_INFOS; i++, info++)
    {
        if (info->pInstigator != m_pInterface)
            continue;

        info->pInstigator = nullptr;
        info->ucFlag1 = 0;
    }
}

// used to init weapons at the moment, called by CPlayerPedSA when its been constructed
// can't use constructor as thats called before the interface pointer has been set the the aforementioned constructor

// TODO Reimplement weapons and PedIK for SA
void CPedSA::Init()
{
    CPedSAInterface* pedInterface = GetPedInterface();

    auto* pedIntelligenceInterface = static_cast<CPedIntelligenceSAInterface*>(pedInterface->pPedIntelligence);
    m_pPedIntelligence = new CPedIntelligenceSA(pedIntelligenceInterface, this);

    m_pPedSound = new CPedSoundSA(&pedInterface->pedSound);
    m_sDefaultVoiceType = m_pPedSound->GetVoiceTypeID();
    m_sDefaultVoiceID = m_pPedSound->GetVoiceID();

    for (std::uint8_t i = 0; i < WEAPONSLOT_MAX; i++)
        m_pWeapons[i] = new CWeaponSA(&(pedInterface->Weapons[i]), this, static_cast<eWeaponSlot>(i));

    #ifdef PedIK_SA
        this->m_pPedIK = new Cm_pPedIKSA(&(pedInterface->m_pPedIK));
    #endif
}

void CPedSA::SetModelIndex(DWORD modelIndex)
{
    // char __thiscall CPed::SetModelIndex(void *this, int a2)
    ((char(__thiscall*)(CEntitySAInterface*, int))FUNC_SetModelIndex)(m_pInterface, modelIndex);

    // Also set the voice gender
    CPedModelInfoSAInterface* modelInfo = static_cast<CPedModelInfoSAInterface*>(pGame->GetModelInfo(modelIndex)->GetInterface());
    if (!modelInfo)
        return;

    DWORD dwType = modelInfo->pedType;
    GetPedInterface()->pedSound.m_bIsFemale = (dwType == 5 || dwType == 22);
}

bool CPedSA::IsInWater() const noexcept
{
    CTask* task = GetPedIntelligence()->GetTaskManager()->GetTask(TASK_PRIORITY_EVENT_RESPONSE_NONTEMP);
    return (task && task->GetTaskType() == TASK_COMPLEX_IN_WATER);
}

bool CPedSA::AddProjectile(eWeaponType weaponType, CVector origin, float force, const CVector* target, const CEntity* targetEntity)
{
    CProjectileInfo* projectileInfo = pGame->GetProjectileInfo();
    if (!projectileInfo)
        return false;

    return projectileInfo->AddProjectile(static_cast<CEntitySA*>(this), weaponType, origin, force, const_cast<CVector*>(target), const_cast<CEntity*>(targetEntity));
}

void CPedSA::DetachPedFromEntity()
{
    // void __thiscall CPed::DettachPedFromEntity(CPed *this)
    ((void(__thiscall*)(CEntitySAInterface*))FUNC_DetachPedFromEntity)(m_pInterface);
}

bool CPedSA::InternalAttachEntityToEntity(DWORD entityInteface, const CVector* position, const CVector* rotation)
{
    AttachPedToEntity(entityInteface, const_cast<CVector*>(position), 0, 0.0f, WEAPONTYPE_UNARMED, false);
    return true;
}

void CPedSA::AttachPedToEntity(DWORD entityInteface, CVector* vector, std::uint16_t direction, float rotationLimit, eWeaponType weaponType, bool changeCamera)
{
    // sDirection and fRotationLimit only apply to first-person shooting (bChangeCamera)
    CPedSAInterface* pedInterface = GetPedInterface();
    std::uint8_t pedType = pedInterface->bPedType;

    // Hack the CPed type(?) to non-player so the camera doesn't get changed
    if (!changeCamera)
        pedInterface->bPedType = 2;

    // CEntity *__thiscall CPed::AttachPedToEntity(CPed *this, CEntity *a2, float arg4, float a4, float a5, __int16 a6, int a7, eWeaponType a3)
    ((CEntitySAInterface*(__thiscall*)(CEntitySAInterface*, CEntitySAInterface*, float, float, float, std::uint16_t, float, std::uint8_t))FUNC_AttachPedToEntity)(m_pInterface, reinterpret_cast<CEntitySAInterface*>(entityInteface), vector->fX, vector->fY, vector->fZ, direction, rotationLimit, static_cast<std::uint8_t>(weaponType));

    // Hack the CPed type(?) to whatever it was set to
    if (!changeCamera)
        pedInterface->bPedType = pedType;
}

CVehicle* CPedSA::GetVehicle() const noexcept
{
    const CPedSAInterface* pedInterface = GetPedInterface();
    if (!pedInterface || !pedInterface->pedFlags.bInVehicle)
        return nullptr;

    CVehicleSAInterface* vehicleInterface = pedInterface->pVehicle;
    if (!vehicleInterface)
        return nullptr;

    auto* vehicleClientEntity = pGame->GetPools()->GetVehicle(reinterpret_cast<DWORD*>(vehicleInterface));
    return vehicleClientEntity ? vehicleClientEntity->pEntity : nullptr;
}

void CPedSA::Respawn(const CVector* position, bool cameraCut)
{
    if (!cameraCut)
        // CGameLogic::RestorePlayerStuffDuringResurrection
        // Disable calls to CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString & CCamera::RestoreWithJumpCut
        MemSet((void*)0x4422EA, 0x90, 20);

    // void __cdecl CGameLogic::RestorePlayerStuffDuringResurrection(CPlayerPed *player, __int128 a2)
    ((void(__cdecl*)(CEntitySAInterface*, float, float, float, float))FUNC_RestorePlayerStuffDuringResurrection)(m_pInterface, position->fX, position->fY, position->fZ, 1.0f);

    #ifdef SortOutStreamingAndMemory // Disabled to see if it reduces crashes
        float angle = 10.0f; // angle for CRenderer::RequestObjectsInDirection

        // void __cdecl CGameLogic::SortOutStreamingAndMemory(CVector *translation, float angle)
        ((void(__cdecl*)(CVector*, float))FUNC_SortOutStreamingAndMemory)(position, angle);
    #endif

    // BYTE *__thiscall CPed::RemoveGogglesModel(CPed *this)
    ((std::uint8_t*(__thiscall*)(CEntitySAInterface*))FUNC_RemoveGogglesModel)(m_pInterface);

    if (!cameraCut)
        // Re-enable calls to CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString & CCamera::RestoreWithJumpCut
        // Restore original bytes
        MemCpy((void*)0x4422EA, "\xB9\x28\xF0\xB6\x00\xE8\x4C\x9A\x0C\x00\xB9\x28\xF0\xB6\x00\xE8\xB2\x97\x0C\x00", 20);
}

void CPedSA::SetIsStanding(bool standing)
{
    // int __thiscall CPed::SetIsStanding(CPed *this, unsigned __int8 a2)
    ((void(__thiscall*)(CEntitySAInterface*, bool))FUNC_SetIsStanding)(m_pInterface, standing);
}

void CPedSA::RemoveWeaponModel(int model)
{
    // void __thiscall CPed::RemoveWeaponModel(CPed *this, int modelID)
    ((void(__thiscall*)(CEntitySAInterface*, int))FUNC_RemoveWeaponModel)(m_pInterface, model);
}

void CPedSA::ClearWeapon(eWeaponType weaponType)
{
    // BYTE *__thiscall CPed::ClearWeapon(CPed *this, eWeaponType a2)
    ((std::uint8_t*(__thiscall*)(CEntitySAInterface*, std::uint8_t))FUNC_ClearWeapon)(m_pInterface, static_cast<std::uint8_t>(weaponType));
}

CWeapon* CPedSA::GiveWeapon(eWeaponType weaponType, std::uint32_t ammo, eWeaponSkill skill)
{
    // Load weapon model
    if (weaponType != WEAPONTYPE_UNARMED)
    {
        CWeaponInfo* weaponInfo = pGame->GetWeaponInfo(weaponType, skill);
        if (weaponInfo)
        {
            std::uint16_t model = weaponInfo->GetModel();
            if (model)
            {
                CModelInfo* modelInfo = pGame->GetModelInfo(model);
                if (modelInfo)
                {
                    modelInfo->Request(BLOCKING, "CPedSA::GiveWeapon");
                    modelInfo->MakeCustomModel();
                }
            }

            // If the weapon is satchels, load the detonator too
            if (weaponType == WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
                GiveWeapon(WEAPONTYPE_DETONATOR, 1, WEAPONSKILL_STD);
        }
    }

    // eWeaponType __thiscall CPed::GiveWeapon(CPed *this, eWeaponType weaponID, signed int ammo, int a4)
    // Last argument is unused
    eWeaponSlot weaponSlot = ((eWeaponSlot(__thiscall*)(CEntitySAInterface*, std::uint8_t, std::uint32_t, std::uint8_t))FUNC_GiveWeapon)(m_pInterface, weaponType, ammo, 1);

    return GetWeapon(weaponSlot);
}

CWeapon* CPedSA::GetWeapon(eWeaponType weaponType) const noexcept
{
    if (weaponType >= WEAPONTYPE_LAST_WEAPONTYPE)
        return nullptr;

    CWeapon* weapon = GetWeapon(pGame->GetWeaponInfo(weaponType)->GetSlot());
    return (weapon && weapon->GetType() == weaponType) ? weapon : nullptr;
}

CWeapon* CPedSA::GetWeapon(eWeaponSlot weaponSlot) const noexcept
{
    return (weaponSlot >= 0 && weaponSlot < WEAPONSLOT_MAX) ? m_pWeapons[weaponSlot] : nullptr;
}

void CPedSA::ClearWeapons() noexcept
{
    // Remove all the weapons
    for (auto& weapon : m_pWeapons)
    {
        weapon->SetAmmoInClip(0);
        weapon->SetAmmoTotal(0);
        weapon->Remove();
    }
}

void CPedSA::RestoreLastGoodPhysicsState()
{
    CPhysicalSA::RestoreLastGoodPhysicsState();
    SetCurrentRotation(0);
    SetTargetRotation(0);
}

void CPedSA::SetCurrentWeaponSlot(eWeaponSlot weaponSlot)
{
    if (weaponSlot >= WEAPONSLOT_MAX)
        return;

    eWeaponSlot currentSlot = GetCurrentWeaponSlot();
    if (weaponSlot == currentSlot)
        return;

    CWeapon* weapon = GetWeapon(currentSlot);
    if (weapon)
    {
        CWeaponInfo* weaponInfo = weapon->GetInfo(WEAPONSKILL_STD);
        if (weaponInfo)
            RemoveWeaponModel(weaponInfo->GetModel());
    }

    CPedSAInterface* pedInterface = GetPedInterface();

    // set the new weapon slot
    pedInterface->bCurrentWeaponSlot = weaponSlot;

    // is the player the local player?
    CPed* localPlayer = pGame->GetPools()->GetPedFromRef(static_cast<DWORD>(1));
    std::uintptr_t changeWeaponFunc;

    if (localPlayer == this)
    {
        auto* playerInfo = static_cast<CPlayerInfoSA*>(pGame->GetPlayerInfo());
        playerInfo->GetInterface()->PlayerPedData.m_nChosenWeapon = weaponSlot;

        // void __thiscall CPlayerPed::MakeChangesForNewWeapon(CPlayerPed *this, int a3)
        changeWeaponFunc = FUNC_MakeChangesForNewWeapon_Slot;
    }
    else
        // void __thiscall CPed::SetCurrentWeapon(CPed *this, int slot)
        changeWeaponFunc = FUNC_SetCurrentWeapon;

    ((void(__thiscall*)(CEntitySAInterface*, std::uint8_t))changeWeaponFunc)(m_pInterface, static_cast<std::uint8_t>(weaponSlot));
}

CVector* CPedSA::GetBonePosition(eBone bone, CVector* position)
{
    ApplySwimAndSlopeRotations();
    CEntitySAInterface* entity = GetInterface();

    // NOTE(botder): A crash used to occur at 0x749B7B in RpClumpForAllAtomics, because the clump pointer might have been null
    // for a broken model.
    if (entity->m_pRwObject)
        // int __thiscall CPed::GetBonePosition(CPed *this, CVector *pPoint, int bone_id, bool bDynamic)
        ((int(__thiscall*)(CEntitySAInterface*, CVector*, std::uint16_t, bool))FUNC_GetBonePosition)(entity, position, bone, true);

    // Clamp to a sane range as this function can occasionally return massive values,
    // which causes ProcessLineOfSight to effectively freeze
    if (!IsValidPosition(*position))
        *position = *GetPosition();

    return position;
}

CVector* CPedSA::GetTransformedBonePosition(eBone bone, CVector* position)
{
    ApplySwimAndSlopeRotations();
    CEntitySAInterface* entity = GetInterface();

    // NOTE(botder): A crash used to occur at 0x7C51A8 in RpHAnimIDGetIndex, because the clump pointer might have been null
    // for a broken model.
    if (entity->m_pRwObject)
        // RwV3D *__thiscall CPed::GetTransformedBonePosition(CPed *this, RwV3D *pointsIn, int boneId, char bUpdateBones)
        ((RwV3d*(__thiscall*)(CEntitySAInterface*, CVector*, std::uint16_t, bool))FUNC_GetTransformedBonePosition)(entity, position, bone, true);

    // Clamp to a sane range as this function can occasionally return massive values,
    // which causes ProcessLineOfSight to effectively freeze
    if (!IsValidPosition(*position))
        *position = *GetPosition();

    return position;
}

//
// Apply the extra ped rotations for slope pitch and swimming.
// Achieved by calling the code at the start of CPed::PreRenderAfterTest
//
void CPedSA::ApplySwimAndSlopeRotations()
{
    CPedSAInterface* pedInterface = GetPedInterface();
    if (pedInterface->pedFlags.bCalledPreRender)
        return;

    // void __thiscall CPed::PreRenderAfterTest(CPed *this)
    g_onlyUpdateRotations = true;
    ((void(__thiscall*)(CEntitySAInterface*))FUNC_PreRenderAfterTest)(pedInterface);
    g_onlyUpdateRotations = false;
}

void CPedSA::SetGogglesState(bool isWearingThem)
{
    // void __thiscall CPed::PutOnGoggles(CPed *this)
    // void __thiscall CPed::TakeOffGoggles(CPed *this)
    std::uintptr_t enableGoggleFunc = isWearingThem ? FUNC_PutOnGoggles : FUNC_TakeOffGoggles;
    ((void(__thiscall*)(CEntitySAInterface*))enableGoggleFunc)(m_pInterface);
}

void CPedSA::SetClothesTextureAndModel(const char* texture, const char* model, int textureType)
{
    DWORD* clothes = reinterpret_cast<DWORD*>(GetPedInterface()->pPlayerData->m_pClothes);
    if (!clothes)
        return;

    // int __fastcall CPedClothesDesc::SetTextureAndModel(DWORD* this, int unknown, char* textureName, char* modelName, eClothesTexturePart texturePart)
    // Second argument is unused in CKeyGen::GetUppercaseKey
    ((int(__fastcall*)(DWORD*, int, const char*, const char*, std::uint8_t))FUNC_CPedClothesDesc__SetTextureAndModel)(clothes, 0, texture, model, textureType);
}

void CPedSA::RebuildPlayer()
{
    // void __cdecl CClothes::RebuildPlayer(CPed *ped, char bIgnoreFatAndMuscle)
    ((void(__cdecl*)(CEntitySAInterface*, bool))FUNC_CClothes__RebuildPlayer)(m_pInterface, false);
}

void CPedSA::SetFightingStyle(eFightingStyle style, std::uint8_t styleExtra)
{
    CPedSAInterface* pedInterface = GetPedInterface();
    std::uint8_t     newStyle = static_cast<std::uint8_t>(style);

    if (newStyle == pedInterface->bFightingStyle)
        return;

    pedInterface->bFightingStyle = newStyle;

    if (styleExtra > 0 && styleExtra <= 6)
        pedInterface->bFightingStyleExtra |= (1 << (styleExtra - 1));
}

CEntity* CPedSA::GetContactEntity() const noexcept
{
    CEntitySAInterface* contactInterface = GetPedInterface()->pContactEntity;
    if (!contactInterface)
        return nullptr;

    CPools* pools = pGame->GetPools();
    switch (contactInterface->nType)
    {
        case ENTITY_TYPE_VEHICLE:
        {
            auto* vehicleClientEntity = pools->GetVehicle(reinterpret_cast<DWORD*>(contactInterface));
            return vehicleClientEntity ? vehicleClientEntity->pEntity : nullptr;
        }
        case ENTITY_TYPE_OBJECT:
        {
            auto* objectClientEntity = pools->GetObject(reinterpret_cast<DWORD*>(contactInterface));
            return objectClientEntity ? objectClientEntity->pEntity : nullptr;
        }
        default:
            break;
    }
}

CEntity* CPedSA::GetTargetedEntity() const noexcept
{
    CEntitySAInterface* targetInterface = GetPedInterface()->pTargetedEntity;
    if (!targetInterface)
        return nullptr;

    return pGame->GetPools()->GetEntity(reinterpret_cast<DWORD*>(targetInterface));
}

void CPedSA::SetTargetedEntity(CEntity* targetEntity)
{
    CEntitySAInterface* targetInterface = nullptr;
    if (targetEntity)
    {
        CEntitySA* entity = dynamic_cast<CEntitySA*>(targetEntity);
        targetInterface = entity ? entity->GetInterface() : nullptr;
    }

    GetPedInterface()->pTargetedEntity = targetInterface;
}

void CPedSA::RemoveBodyPart(std::uint8_t boneID, std::uint8_t direction)
{
    // char __thiscall CPed::RemoveBodyPart(CPed *this, int boneID, int localDir)
    // second argument is unused (direction)
    ((char(__thiscall*)(CEntitySAInterface*, std::uint8_t, std::uint8_t))FUNC_CPed_RemoveBodyPart)(m_pInterface, boneID, direction);
}

void CPedSA::SetFootBlood(std::uint32_t footBlood)
{
    CPedSAInterface* pedInterface = GetPedInterface();

    if (footBlood > 0)
        // Make sure the foot blood flag is activated
        pedInterface->pedFlags.bDoBloodyFootprints = true;
    else if (pedInterface->pedFlags.bDoBloodyFootprints)
        // If the foot blood flag is activated, deactivate it
        pedInterface->pedFlags.bDoBloodyFootprints = false;

    // Set the amount of foot blood
    pedInterface->dwTimeWhenDead = footBlood;
}

std::uint32_t CPedSA::GetFootBlood() const
{
    const CPedSAInterface* pedInterface = GetPedInterface();
    
    // If the foot blood flag is activated, return the amount of foot blood
    return pedInterface->pedFlags.bDoBloodyFootprints ? pedInterface->dwTimeWhenDead : 0;
}

void CPedSA::SetOnFire(bool onFire)
{
    CPedSAInterface* pedInterface = GetPedInterface();

    // If we are already on fire, don't apply a new fire
    if ((onFire && pedInterface->pFireOnPed) || (!onFire && !pedInterface->pFireOnPed))
        return;    

    CFireManagerSA* fireManager = static_cast<CFireManagerSA*>(pGame->GetFireManager());
    if (!fireManager)
        return;

    if (onFire)
    {
        CFire* fire = fireManager->StartFire(this, nullptr, static_cast<float>(DEFAULT_FIRE_PARTICLE_SIZE));
        if (!fire)
            return;

        fire->SetTarget(this);
        fire->Ignite();
        fire->SetStrength(1.0f);

        // Attach the fire only to the player, do not let it
        // create child fires when moving.
        fire->SetNumGenerationsAllowed(0);
        pedInterface->pFireOnPed = fire->GetInterface();
    }
    else
    {
        CFire* fire = fireManager->GetFire(pedInterface->pFireOnPed);
        if (fire)
            fire->Extinguish();
    }
}

void CPedSA::GetVoice(std::int16_t* voiceType, std::int16_t* voiceID) const
{
    if (!m_pPedSound)
        return;

    if (voiceType)
        *voiceType = m_pPedSound->GetVoiceTypeID();

    if (voiceID)
        *voiceID = m_pPedSound->GetVoiceID();
}

void CPedSA::GetVoice(const char** voiceType, const char** voice) const
{
    std::int16_t sVoiceType, sVoiceID;
    GetVoice(&sVoiceType, &sVoiceID);

    if (voiceType)
        *voiceType = CPedSoundSA::GetVoiceTypeNameFromID(sVoiceType);
    if (voice)
        *voice = CPedSoundSA::GetVoiceNameFromID(sVoiceType, sVoiceID);
}

void CPedSA::SetVoice(std::int16_t voiceType, std::int16_t voiceID)
{
    if (!m_pPedSound)
        return;

    m_pPedSound->SetVoiceTypeID(voiceType);
    m_pPedSound->SetVoiceID(voiceID);
}

void CPedSA::SetVoice(const char* voiceType, const char* voice)
{
    std::int16_t sVoiceType = CPedSoundSA::GetVoiceTypeIDFromName(voiceType);
    if (sVoiceType < 0)
        return;

    std::int16_t sVoiceID = CPedSoundSA::GetVoiceIDFromName(sVoiceType, voice);
    if (sVoiceID < 0)
        return;

    SetVoice(sVoiceType, sVoiceID);
}

// GetCurrentWeaponStat will only work if the game ped context is currently set to this ped
CWeaponStat* CPedSA::GetCurrentWeaponStat() const noexcept
{
    if (pGame->GetPedContext() != this)
    {
        OutputDebugLine("WARNING: GetCurrentWeaponStat ped context mismatch");
        return nullptr;
    }

    CWeapon* weapon = GetWeapon(GetCurrentWeaponSlot());
    if (!weapon)
        return nullptr;

    eWeaponType   weaponType = weapon->GetType();
    std::uint16_t weaponSkillStat = pGame->GetStats()->GetSkillStatIndex(weaponType);
    float         skill = pGame->GetStats()->GetStatValue(weaponSkillStat);

    return pGame->GetWeaponStatManager()->GetWeaponStatsFromSkillLevel(weaponType, skill);
}

float CPedSA::GetCurrentWeaponRange() const noexcept
{
    CWeaponStat* weaponStat = GetCurrentWeaponStat();
    if (!weaponStat)
        return 1.0f;

    return weaponStat->GetWeaponRange();
}

void CPedSA::AddWeaponAudioEvent(EPedWeaponAudioEventType audioEventType)
{
    // void __thiscall CAEPedWeaponAudioEntity::AddAudioEvent(CAEPedWeaponAudioEntity *this, int audioEventId)
    ((void(__thiscall*)(CPedWeaponAudioEntitySAInterface*, std::uint16_t))FUNC_CAEPedWeaponAudioEntity__AddAudioEvent)(&GetPedInterface()->weaponAudioEntity, static_cast<std::uint16_t>(audioEventType));
}

bool CPedSA::IsDoingGangDriveby() const noexcept
{
    if (!m_pPedIntelligence)
        return false;

    CTask* task = GetPedIntelligence()->GetTaskManager()->GetTask(TASK_PRIORITY_PRIMARY);
    return (task && task->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY);
}

float CPedSA::GetOxygenLevel() const
{
    return GetPedInterface()->pPlayerData->m_fBreath;
}

void CPedSA::SetOxygenLevel(float oxygen)
{
    GetPedInterface()->pPlayerData->m_fBreath = oxygen;
}

void CPedSA::GetAttachedSatchels(std::vector<SSatchelsData>& satchelsList) const
{
    // Array of projectiles objects
    CProjectileSAInterface** projectilesArray = (CProjectileSAInterface**)ARRAY_CProjectile;
    CProjectileSAInterface*  projectileInterface;

    // Array of projectiles infos
    CProjectileInfoSAInterface* projectilesInfoArray = (CProjectileInfoSAInterface*)ARRAY_CProjectileInfo;
    CProjectileInfoSAInterface* projectileInfoInterface;

    // Loop through all projectiles
    for (std::uint8_t i = 0; i < PROJECTILE_COUNT; i++)
    {
        projectileInterface = projectilesArray[i];

        // is attached to our ped?
        if (!projectileInterface || projectileInterface->m_pAttachedEntity != m_pInterface)
            continue;

        // index is always the same for both arrays
        projectileInfoInterface = &projectilesInfoArray[i];

        // We are only interested in satchels
        if (!projectileInfoInterface || projectileInfoInterface->dwProjectileType != eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
            continue;

        // Push satchel into the array. There is no need to check the counter because for satchels it restarts until the player detonates the charges
        satchelsList.push_back({projectileInterface, &projectileInterface->m_vecAttachedOffset, &projectileInterface->m_vecAttachedRotation});
    }
}

////////////////////////////////////////////////////////////////
//
// CPed_PreRenderAfterTest
//
// Code at start of CPed::PreRenderAfterTest applies extra rotations for slope pitch and swimming.
// Check if they have already been applied.
//
////////////////////////////////////////////////////////////////
#define HOOKPOS_CPed_PreRenderAfterTest 0x5E65A0
#define HOOKSIZE_CPed_PreRenderAfterTest 15
static constexpr DWORD RETURN_CPed_PreRenderAfterTest = 0x5E65AF;
static constexpr DWORD RETURN_CPed_PreRenderAfterTestSkip = 0x5E6658;
static void _declspec(naked) HOOK_CPed_PreRenderAfterTest()
{
    _asm
    {
        // Replaced code
        sub esp,70h
        push ebx
        push ebp
        push esi
        mov ebp, ecx
        mov ecx, dword ptr [ebp+47Ch]
        push edi

        // Check what to do
        mov eax, [ebp+474h] // Load m_nThirdPedFlags
        test eax, 400h // check bCalledPreRender flag
        jnz skip_rotation_update

        // Run code at start of CPed::PreRenderAfterTest
        jmp RETURN_CPed_PreRenderAfterTest

skip_rotation_update:
        // Skip code at start of CPed::PreRenderAfterTest
        jmp RETURN_CPed_PreRenderAfterTestSkip
    }
}

////////////////////////////////////////////////////////////////
//
// CPed_PreRenderAfterTest_Mid
//
// Code at mid of CPed::PreRenderAfterTest does all sorts of stuff
// Check if it should not be called because we only wanted to do the extra rotations
//
////////////////////////////////////////////////////////////////
#define HOOKPOS_CPed_PreRenderAfterTest_Mid 0x5E6669
#define HOOKSIZE_CPed_PreRenderAfterTest_Mid 5
static constexpr DWORD RETURN_CPed_PreRenderAfterTest_Mid = 0x5E666E;
static constexpr DWORD RETURN_CPed_PreRenderAfterTest_MidSkip = 0x5E766F;
static void _declspec(naked) HOOK_CPed_PreRenderAfterTest_Mid()
{
    _asm
    {
        // Check what to do
        movzx eax, byte ptr g_onlyUpdateRotations
        test eax, eax
        jnz skip_tail

        // Replaced code
        mov al, byte ptr ds:[00B7CB89h]
        // Run code at mid of CPed::PreRenderAfterTest
        jmp RETURN_CPed_PreRenderAfterTest_Mid

skip_tail:
        // Skip code at mid of CPed::PreRenderAfterTest
        jmp RETURN_CPed_PreRenderAfterTest_MidSkip
    }
}

////////////////////////////////////////////////////////////////
//
// Setup hooks
//
////////////////////////////////////////////////////////////////
void CPedSA::StaticSetHooks()
{
    EZHookInstall(CPed_PreRenderAfterTest);
    EZHookInstall(CPed_PreRenderAfterTest_Mid);
}
