/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedSA.cpp
 *  PURPOSE:     Ped entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
#include "CAnimManagerSA.h"
#include "CAnimBlendAssociationSA.h"
#include "TaskSA.h"
#include "CAnimBlendHierarchySA.h"

extern CGameSA* pGame;

static bool g_onlyUpdateRotations = false;

CPedSA::~CPedSA()
{
    // Make sure this ped is not refed in the flame shot info array
    auto* info = reinterpret_cast<CFlameShotInfo*>(ARRAY_CFlameShotInfo);
    for (std::size_t i = 0; i < MAX_FLAME_SHOT_INFOS; i++, info++)
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

    m_pedIntelligence = std::make_unique<CPedIntelligenceSA>(pedInterface->pPedIntelligence, this);

    m_pedSound = std::make_unique<CPedSoundSA>(&pedInterface->pedSound);
    m_defaultVoiceType = m_pedSound->GetVoiceTypeID();
    m_defaultVoiceID = m_pedSound->GetVoiceID();

    for (std::size_t i = 0; i < WEAPONSLOT_MAX; i++)
        m_weapons[i] = std::make_unique<CWeaponSA>(&(pedInterface->Weapons[i]), this, static_cast<eWeaponSlot>(i));

    #ifdef PedIK_SA
        this->m_pPedIK = new Cm_pPedIKSA(&(pedInterface->m_pPedIK));
    #endif
}

void CPedSA::SetModelIndex(std::uint32_t modelIndex)
{
    // char __thiscall CPed::SetModelIndex(void *this, int a2)
    ((char(__thiscall*)(CEntitySAInterface*, int))FUNC_SetModelIndex)(m_pInterface, modelIndex);

    // Also set the voice gender
    CModelInfo* mi = pGame->GetModelInfo(modelIndex);
    if (!mi)
        return;

    CPedModelInfoSAInterface* modelInfo = static_cast<CPedModelInfoSAInterface*>(mi->GetInterface());
    if (!modelInfo)
        return;

    std::uint32_t type = modelInfo->pedType;
    GetPedInterface()->pedSound.m_bIsFemale = type == 5 || type == 22;
}

bool CPedSA::AddProjectile(eWeaponType weaponType, CVector origin, float force, CVector* target, CEntity* targetEntity)
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
    // sDirection and fRotationLimit only apply to first-person shooting (bChangeCamera)
    CPedSAInterface* pedInterface = GetPedInterface();
    std::uint8_t pedType = pedInterface->bPedType;

    // Hack the CPed type(?) to non-player so the camera doesn't get changed
    pedInterface->bPedType = 2;

    // CEntity *__thiscall CPed::AttachPedToEntity(CPed *this, CEntity *a2, float arg4, float a4, float a5, __int16 a6, int a7, eWeaponType a3)
    ((CEntitySAInterface*(__thiscall*)(CEntitySAInterface*, CEntitySAInterface*, float, float, float, std::uint16_t, int, eWeaponType))FUNC_AttachPedToEntity)(m_pInterface, reinterpret_cast<CEntitySAInterface*>(entityInteface), position->fX, position->fY, position->fZ, 0, 0, WEAPONTYPE_UNARMED);

    // Hack the CPed type(?) to whatever it was set to
    pedInterface->bPedType = pedType;

    return true;
}

CVehicle* CPedSA::GetVehicle() const
{
    CPedSAInterface* pedInterface = GetPedInterface();
    if (!pedInterface || !pedInterface->pedFlags.bInVehicle)
        return nullptr;

    CVehicleSAInterface* vehicleInterface = pedInterface->pVehicle;
    if (!vehicleInterface)
        return nullptr;

    auto* vehicleClientEntity = pGame->GetPools()->GetVehicle(reinterpret_cast<DWORD*>(vehicleInterface));
    return vehicleClientEntity ? vehicleClientEntity->pEntity : nullptr;
}

void CPedSA::Respawn(CVector* position, bool cameraCut)
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

CWeapon* CPedSA::GiveWeapon(eWeaponType weaponType, std::uint32_t ammo, eWeaponSkill skill)
{
    // Load weapon model
    if (weaponType != WEAPONTYPE_UNARMED)
    {
        CWeaponInfo* weaponInfo = pGame->GetWeaponInfo(weaponType, skill);
        if (weaponInfo)
        {
            CModelInfo* modelInfo = pGame->GetModelInfo(static_cast<std::uint32_t>(weaponInfo->GetModel()));
            if (modelInfo)
            {
                modelInfo->Request(BLOCKING, "CPedSA::GiveWeapon");
                modelInfo->MakeCustomModel();
            }
            
            // If the weapon is satchels, load the detonator too
            if (weaponType == WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
                GiveWeapon(WEAPONTYPE_DETONATOR, 1, WEAPONSKILL_STD);
        }
    }

    // eWeaponType __thiscall CPed::GiveWeapon(CPed *this, eWeaponType weaponID, signed int ammo, int a4)
    // Last argument is unused
    eWeaponSlot weaponSlot = ((eWeaponSlot(__thiscall*)(CEntitySAInterface*, eWeaponType, std::uint32_t, int))FUNC_GiveWeapon)(m_pInterface, weaponType, ammo, 1);

    return GetWeapon(weaponSlot);
}

CWeapon* CPedSA::GetWeapon(eWeaponType weaponType) const
{
    if (weaponType >= WEAPONTYPE_LAST_WEAPONTYPE)
        return nullptr;

    CWeapon* weapon = GetWeapon(pGame->GetWeaponInfo(weaponType)->GetSlot());
    return (weapon && weapon->GetType() == weaponType) ? weapon : nullptr;
}

CWeapon* CPedSA::GetWeapon(eWeaponSlot weaponSlot) const noexcept
{
    return (weaponSlot >= 0 && weaponSlot < WEAPONSLOT_MAX) ? m_weapons[weaponSlot].get() : nullptr;
}

void CPedSA::ClearWeapons()
{
    // Remove all the weapons
    for (auto& weapon : m_weapons)
    {
        weapon->SetAmmoInClip(0);
        weapon->SetAmmoTotal(0);
        weapon->Remove();
    }
}

void CPedSA::RemoveWeaponModel(std::uint32_t model)
{
    // void __thiscall CPed::RemoveWeaponModel(CPed *this, int modelID)
    ((void(__thiscall*)(CEntitySAInterface*, std::uint32_t))FUNC_RemoveWeaponModel)(m_pInterface, model);
}

void CPedSA::ClearWeapon(eWeaponType weaponType)
{
    // BYTE *__thiscall CPed::ClearWeapon(CPed *this, eWeaponType a2)
    ((std::uint8_t * (__thiscall*)(CEntitySAInterface*, eWeaponType)) FUNC_ClearWeapon)(m_pInterface, weaponType);
}

void CPedSA::SetIsStanding(bool standing)
{
    // int __thiscall CPed::SetIsStanding(CPed *this, unsigned __int8 a2)
    ((void(__thiscall*)(CEntitySAInterface*, bool))FUNC_SetIsStanding)(m_pInterface, standing);
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

    // set the new weapon slot
    GetPedInterface()->bCurrentWeaponSlot = weaponSlot;

    // is the player the local player?
    CPed* localPlayer = pGame->GetPedContext();

    std::uintptr_t changeWeaponFunc;

    if (localPlayer == this)
    {
        auto* playerInfo = static_cast<CPlayerInfoSA*>(pGame->GetPlayerInfo());
        if (!playerInfo)
            return;

        if (auto* infoInterface = playerInfo->GetInterface())
            infoInterface->PlayerPedData.m_nChosenWeapon = weaponSlot;

        // void __thiscall CPlayerPed::MakeChangesForNewWeapon(CPlayerPed *this, int a3)
        changeWeaponFunc = FUNC_MakeChangesForNewWeapon_Slot;
    }
    else
        // void __thiscall CPed::SetCurrentWeapon(CPed *this, int slot)
        changeWeaponFunc = FUNC_SetCurrentWeapon;

    ((void(__thiscall*)(CEntitySAInterface*, eWeaponSlot))changeWeaponFunc)(m_pInterface, weaponSlot);
}

CVector* CPedSA::GetBonePosition(eBone bone, CVector* position)
{
    ApplySwimAndSlopeRotations();
    CEntitySAInterface* entity = GetInterface();

    // NOTE(botder): A crash used to occur at 0x749B7B in RpClumpForAllAtomics, because the clump pointer might have been null
    // for a broken model.
    if (entity->m_pRwObject)
        // int __thiscall CPed::GetBonePosition(CPed *this, CVector *pPoint, int bone_id, bool bDynamic)
        ((void(__thiscall*)(CEntitySAInterface*, CVector*, eBone, bool))FUNC_GetBonePosition)(entity, position, bone, true);

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
        ((RwV3d*(__thiscall*)(CEntitySAInterface*, CVector*, eBone, bool))FUNC_GetTransformedBonePosition)(entity, position, bone, true);

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

bool CPedSA::IsInWater() const
{
    CTask* task = GetPedIntelligence()->GetTaskManager()->GetTask(TASK_PRIORITY_EVENT_RESPONSE_NONTEMP);
    return (task && task->GetTaskType() == TASK_COMPLEX_IN_WATER);
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
    CPedClothesDesc* clothes = GetPedInterface()->pPlayerData->m_pClothes;
    if (!clothes)
        return;

    // int __fastcall CPedClothesDesc::SetTextureAndModel(DWORD* this, int unknown, char* textureName, char* modelName, eClothesTexturePart texturePart)
    // Second argument is unused in CKeyGen::GetUppercaseKey
    ((void(__fastcall*)(CPedClothesDesc*, int, const char*, const char*, std::uint8_t))FUNC_CPedClothesDesc__SetTextureAndModel)(clothes, 0, texture, model, textureType);
}

void CPedSA::RebuildPlayer()
{
    // void __cdecl CClothes::RebuildPlayer(CPed *ped, char bIgnoreFatAndMuscle)
    ((void(__cdecl*)(CEntitySAInterface*, bool))FUNC_CClothes__RebuildPlayer)(m_pInterface, false);
}

void CPedSA::SetFightingStyle(eFightingStyle style, std::uint8_t styleExtra)
{
    CPedSAInterface* pedInterface = GetPedInterface();
    if (style == pedInterface->bFightingStyle)
        return;

    pedInterface->bFightingStyle = style;

    if (styleExtra > 0 && styleExtra <= 6)
        pedInterface->bFightingStyleExtra |= (1 << (styleExtra - 1));
}

CEntity* CPedSA::GetContactEntity() const
{
    CEntitySAInterface* contactInterface = GetPedInterface()->pContactEntity;
    if (!contactInterface)
        return nullptr;

    return pGame->GetPools()->GetEntity(reinterpret_cast<DWORD*>(contactInterface));
}

void CPedSA::RemoveBodyPart(std::uint8_t boneID, std::uint8_t direction)
{
    // char __thiscall CPed::RemoveBodyPart(CPed *this, int boneID, int localDir)
    // second argument is unused (direction)
    ((char(__thiscall*)(CEntitySAInterface*, std::uint8_t, std::uint8_t))FUNC_CPed_RemoveBodyPart)(m_pInterface, boneID, 0);
}

void CPedSA::SetFootBlood(std::uint32_t footBlood)
{
    CPedSAInterface* pedInterface = GetPedInterface();
    pedInterface->pedFlags.bDoBloodyFootprints = footBlood > 0;

    // Set the amount of foot blood
    pedInterface->timeWhenDead = footBlood;
}

std::uint32_t CPedSA::GetFootBlood() const
{
    CPedSAInterface* pedInterface = GetPedInterface();
    
    // If the foot blood flag is activated, return the amount of foot blood
    return pedInterface->pedFlags.bDoBloodyFootprints ? pedInterface->timeWhenDead : 0;
}

bool CPedSA::SetOnFire(bool onFire)
{
    CPedSAInterface* pInterface = GetPedInterface();
    if (onFire == !!pInterface->pFireOnPed)
        return false;

    auto* fireManager = static_cast<CFireManagerSA*>(pGame->GetFireManager());
    if (!fireManager)
        return false;

    if (onFire)
    {
        CFire* fire = fireManager->StartFire(this, nullptr, static_cast<float>(DEFAULT_FIRE_PARTICLE_SIZE));
        if (!fire)
            return false;

        // Start the fire
        fire->SetTarget(this);
        fire->Ignite();
        fire->SetStrength(1.0f);
        // Attach the fire only to the player, do not let it
        // create child fires when moving.
        fire->SetNumGenerationsAllowed(0);
        pInterface->pFireOnPed = fire->GetInterface();
    }
    else
    {
        CFire* fire = fireManager->GetFire(static_cast<CFireSAInterface*>(pInterface->pFireOnPed));
        if (!fire)
            return false;

        fire->Extinguish();
    }

    return true;
}

void CPedSA::GetVoice(std::int16_t* voiceType, std::int16_t* voiceID) const
{
    if (!m_pedSound)
        return;

    if (voiceType)
        *voiceType = m_pedSound->GetVoiceTypeID();

    if (voiceID)
        *voiceID = m_pedSound->GetVoiceID();
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
    if (!m_pedSound)
        return;

    m_pedSound->SetVoiceTypeID(voiceType);
    m_pedSound->SetVoiceID(voiceID);
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
CWeaponStat* CPedSA::GetCurrentWeaponStat() const
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

float CPedSA::GetCurrentWeaponRange() const
{
    CWeaponStat* weaponStat = GetCurrentWeaponStat();
    return weaponStat ? weaponStat->GetWeaponRange() : 1.0f;
}

void CPedSA::AddWeaponAudioEvent(EPedWeaponAudioEventType audioEventType)
{
    // void __thiscall CAEPedWeaponAudioEntity::AddAudioEvent(CAEPedWeaponAudioEntity *this, int audioEventId)
    ((void(__thiscall*)(CPedWeaponAudioEntitySAInterface*, std::uint16_t))FUNC_CAEPedWeaponAudioEntity__AddAudioEvent)(&GetPedInterface()->weaponAudioEntity, static_cast<std::uint16_t>(audioEventType));
}

bool CPedSA::IsDoingGangDriveby() const
{
    if (!m_pedIntelligence)
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

void CPedSA::Say(const ePedSpeechContext& speechId, float probability)
{
    // Call CPed::Say
    ((void(__thiscall*)(CPedSAInterface*, ePedSpeechContext, int, float, bool, bool, bool))FUNC_CPed_Say)(GetPedInterface(), speechId, 0, probability, false, false, false);
}

void CPedSA::GetAttachedSatchels(std::vector<SSatchelsData>& satchelsList) const
{
    // Array of projectiles objects
    auto** projectilesArray = reinterpret_cast<CProjectileSAInterface**>(ARRAY_CProjectile);
    CProjectileSAInterface*  projectileInterface = nullptr;

    // Array of projectiles infos
    auto* projectilesInfoArray = reinterpret_cast<CProjectileInfoSAInterface*>(ARRAY_CProjectileInfo);
    CProjectileInfoSAInterface* projectileInfoInterface = nullptr;

    satchelsList.reserve(PROJECTILE_COUNT);

    // Loop through all projectiles
    for (std::size_t i = 0; i < PROJECTILE_COUNT; i++)
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
        satchelsList.emplace_back(projectileInterface, &projectileInterface->m_vecAttachedOffset, &projectileInterface->m_vecAttachedRotation);
    }
}

void __fastcall CPedSA::PlayFootSteps(CPedSAInterface* ped)
{
    //auto firstAssoc = pGame->GetAnimManager()->RpAnimBlendClumpGetFirstAssociation(ped->m_pRwObject);
    auto* firstAssoc = ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*))0x4D15E0)(ped->m_pRwObject);
    if (!firstAssoc)
        return;

    //eAnimID animID = firstAssoc->GetAnimID();
    eAnimID animID = (eAnimID)firstAssoc->sAnimID;

    bool unk_v35 = false;
    if (animID == eAnimID::ANIM_ID_WALK || animID == eAnimID::ANIM_ID_RUN || animID == eAnimID::ANIM_ID_SPRINT)
        unk_v35 = true;

    if (ped->pedFlags.bDoBloodyFootprints && ped->timeWhenDead > 0 && ped->timeWhenDead < 300)
    {
        ped->timeWhenDead--;

        if (ped->timeWhenDead == 0)
            ped->pedFlags.bDoBloodyFootprints = false;
    }

    if (!ped->pedFlags.bIsStanding)
        return;

    CAnimBlendAssociationSAInterface* unk_v4 = nullptr;
    float                  unk_v30 = 0.0f;

    float unk_a3 = 0.0f;

    //for (auto& assoc = firstAssoc; assoc != nullptr; assoc = pGame->GetAnimManager()->RpAnimBlendGetNextAssociation(assoc))
    for (auto& assoc = firstAssoc; assoc != nullptr; assoc = ((CAnimBlendAssociationSAInterface * (__cdecl*)(CAnimBlendAssociationSAInterface*))0x4D6AB0)(assoc))
    {
        if (assoc->m_bf9)
        {
            unk_v4 = assoc;
            unk_v30 += assoc->fBlendAmount;
        }
        else if (!assoc->m_bAddAnimBlendToTotalBlend && assoc->sAnimID != 223 && (assoc->m_bPartial || !ped->pedFlags.bIsDucking))
            unk_a3 += assoc->fBlendAmount;

        /* if (assoc->GetInterface()->m_bf9)
        {
            unk_v4 = assoc.get();
            unk_v30 += assoc->GetBlendAmount();
        }
        else if (!assoc->GetInterface()->m_bAddAnimBlendToTotalBlend && assoc->GetAnimID() != eAnimID::ANIM_ID_FIGHT_IDLE && (assoc->GetInterface()->m_bPartial || !ped->pedFlags.bIsDucking))
        {
            unk_a3 += assoc->GetBlendAmount();
        }*/
    }

    auto handleLanding = [](CPedSAInterface* ped)
    {
        if (!ped) return;

        if (ped->pedFlags.bIsLanding)
        {
            // CTaskManager::GetSimplestActiveTask
            auto* task = ((CTaskSAInterface* (__thiscall*)(void*))0x6819D0)(&ped->pPedIntelligence->taskManager);
            if (!task) return;

            DWORD taskType = reinterpret_cast<int(__thiscall*)(void*)>(task->VTBL->GetTaskType)(task);

            if (taskType == 0xF2) // TASK_SIMPLE_LAND
            {
                // CTaskSimpleLand::RightFootLanded
                if (((bool(__thiscall*)(void*))0x678FE0)((void*)task))
                    ((void(__thiscall*)(CPedSAInterface*, int, bool))0x5E5380)(ped, 0, true); // CPed::DoFootLanded
                else if (((bool(__thiscall*)(void*))0x679010)((void*)task)) // CTaskSimpleLand::LeftFootLanded
                    ((void(__thiscall*)(CPedSAInterface*, int, bool))0x5E5380)(ped, 1, true); // CPed::DoFootLanded
            }
        }
    };

    auto triggerSoundQuietEvent = [](CPedSAInterface* ped, float soundLevel)
    {
        CVector pos{};

        void* mem = ((void*(__cdecl*)(int))0x72F420)(0x2C);

        ((void*(__thiscall*)(void*, CEntitySAInterface*, float, int, CVector*))0x5E05B0)(mem, ped, soundLevel, -1, &pos);

        void* eventGlobalGroup = ((void*(__cdecl*)())0x4ABA50)();
        ((void(__thiscall*)(void*, void*, int))0x4AB420)(eventGlobalGroup, mem, 0);

        ((void(__thiscall*)(void*))0x5DEA00)(mem);
    };

    auto DoFootstep = [unk_v35](CPedSAInterface* ped, bool leftFoot)
    {
        float unk_a3d = 0.0f;
        float unk_v34 = 0.0f;

        if (ped->pedFlags.bIsDucking)
        {
            unk_a3d = -18.0f;
            unk_v34 = 0.8f;
        }
        else
        {
            if (ped->moveState == PedMoveState::Enum::PEDMOVE_RUN)
            {
                unk_a3d = -6.0f;
                unk_v34 = 1.1f;
            }
            else if (ped->moveState == PedMoveState::Enum::PEDMOVE_SPRINT)
            {
                unk_v34 = 1.2f;
            }
            else
            {
                unk_a3d = -12.0f;
                unk_v34 = 0.9f;
            }

            if (ped->iMoveAnimGroup == 69)
            {
                unk_a3d -= 6.0f;
                unk_v34 -= 0.1f;
            }
        }

        if (ped->pedAudio.canAddEvent)
        {
            // CPedAudio::AddEvent
            ((void(__thiscall*)(CPedSoundEntitySAInterface*, int, float, float, CEntitySAInterface*, int, int, int))0x4E2BB0)(&ped->pedAudio, leftFoot ? 54 : 55, unk_a3d, unk_v34, nullptr, 0, 0, 0);
        }

        // CPed::DoFootLanded
        ((void(__thiscall*)(CPedSAInterface*, int, bool))0x5E5380)(ped, leftFoot ? 1 : 0, unk_v35);
    };


    if (!unk_v4 || unk_v30 <= 0.5f || unk_a3 >= 1.0f)
    {
        handleLanding(ped);
        return;
    }

    float totalTime = unk_v4->pAnimHierarchy->fTotalTime * 0.066f;
    float totalTime2 = unk_v4->pAnimHierarchy->fTotalTime * 0.5f + totalTime;

    //float totalTime = unk_v4->GetAnimHierarchy()->GetTotalTime() * 0.066f;
    //float totalTime2 = unk_v4->GetAnimHierarchy()->GetTotalTime() * 0.5f + totalTime;

    if (ped->pedFlags.bIsDucking)
    {
        totalTime += 0.2f;
        totalTime2 += 0.2f;
    }

    CPedStatSAInterface** ms_apPedStats = reinterpret_cast<CPedStatSAInterface**>(0xC0BBEC);
    float                 unk_a3a = 0.0f;

    float unk_v32 = 0.0f;

    if (ped->pPedStats == ms_apPedStats[40])
    {
        unk_a3a = 0.533f;
        if (unk_v4->sAnimID != 0)
            unk_a3a = 0.33f;

        int surface = ((int(__thiscall*)(void*, int))0x55E5C0)((void*)0xB79538, ped->m_ucCollisionContactSurfaceType) - 3;
        if (surface > 0)
        {
            int prevSurface = surface - 1;
            if (prevSurface == 0)
            {
                if (rand() % 64 > 0)
                {
                    ped->vecAnimMovingShiftLocal *= 0.2f;
                }

                handleLanding(ped);
                return;
            }

            if (prevSurface == 1)
            {
                ped->vecAnimMovingShiftLocal *= 0.3f;
                handleLanding(ped);
                return;
            }

            unk_v32 = 1.0f;
        }
        else
        {
            if (rand() % 128 > 0)
            {
                ped->vecAnimMovingShiftLocal *= 0.5f;
            }

            unk_v32 = 0.5f;
        }

        if (unk_v4->fCurrentTime <= 0.0f || unk_v4->fCurrentTime - unk_v4->fTimeStep > 0.0f)
        {
            if (unk_v32 > 0.2f && unk_v4->fCurrentTime > unk_a3a && unk_v4->fCurrentTime - unk_v4->fTimeStep <= unk_a3a && ped->pedAudio.canAddEvent)
            {
                ((void(__thiscall*)(CPedSoundEntitySAInterface*, int, float, float, CEntitySAInterface*, int, int, int))0x4E2BB0)(&ped->pedAudio, 57, std::log10(unk_v32), unk_v4->sAnimID == 0 ? 0.5 : 1.0f, nullptr, 0, 0, 0);
            }
            else if (ped->pedAudio.canAddEvent)
            {
                ((void(__thiscall*)(CPedSoundEntitySAInterface*, int, float, float, CEntitySAInterface*, int, int, int))0x4E2BB0)(&ped->pedAudio, 56, std::log10(unk_v32), unk_v4->sAnimID == 0 ? 0.5 : 1.0f, nullptr, 0, 0, 0);
            }
        }

        handleLanding(ped);
        return;
    }

    if (totalTime > unk_v4->fCurrentTime || unk_v4->fCurrentTime - unk_v4->fTimeStep >= totalTime)
    {
        if (unk_v4->fCurrentTime >= totalTime2 && unk_v4->fCurrentTime - unk_v4->fTimeStep < totalTime2)
        {
            DoFootstep(ped, false);
        }

        handleLanding(ped);
        return;
    }

    if (ped->bPedType < 2) // CPed::IsPlayer
    {
        CPlayerPedDataSAInterface* playerData = ped->pPlayerData;
        if (playerData)
        {
            bool wearingBaclava = ((bool(__fastcall*)(CPedClothesDesc*, int))0x5A7950)(playerData->m_pClothes, 0);
            if (ped->moveState >= PedMoveState::Enum::PEDMOVE_JOG)
            {
                if (ped->moveState <= PedMoveState::Enum::PEDMOVE_RUN)
                {
                    if (playerData->m_moveBlendRatio >= 2.0f)
                    {
                        triggerSoundQuietEvent(ped, wearingBaclava ? 55.0f : 45.0f);
                        // goto label 65
                    }

                    float unk_v18 = 0.0f;

                    if (wearingBaclava && playerData->m_moveBlendRatio > 1.1f)
                        unk_v18 = (playerData->m_moveBlendRatio - 1.0f) * 20.0f;
                    else
                    {
                        if (playerData->m_moveBlendRatio <= 1.5f)
                            // goto label 65

                            unk_v18 = (playerData->m_moveBlendRatio - 1.0f) * 15.0f;
                    }

                    float level = unk_v18 + 30.0f;
                    if (level > 0.0f)
                        triggerSoundQuietEvent(ped, level);
                }
                else if (ped->moveState == PedMoveState::Enum::PEDMOVE_SPRINT)
                {
                    triggerSoundQuietEvent(ped, wearingBaclava ? 65.0f : 55.0f);
                }
            }
        }

        DoFootstep(ped, true);
        handleLanding(ped);
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
static constexpr std::uintptr_t RETURN_CPed_PreRenderAfterTest = 0x5E65AF;
static constexpr std::uintptr_t RETURN_CPed_PreRenderAfterTestSkip = 0x5E6658;
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
static constexpr std::uintptr_t RETURN_CPed_PreRenderAfterTest_Mid = 0x5E666E;
static constexpr std::uintptr_t RETURN_CPed_PreRenderAfterTest_MidSkip = 0x5E766F;
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

    HookInstallCall(0x5E92C8, (DWORD)CPedSA::PlayFootSteps);
}
