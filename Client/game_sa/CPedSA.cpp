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
#include <game/TaskTypes.h>
#include "CFireManagerSA.h"
#include "CGameSA.h"
#include "CPedModelInfoSA.h"
#include "CPedSA.h"
#include "CPlayerInfoSA.h"
#include "CProjectileInfoSA.h"
#include "CStatsSA.h"
#include "CTaskManagerSA.h"
#include "CTasksSA.h"
#include "CWeaponInfoSA.h"
#include "CWeaponStatManagerSA.h"

extern CGameSA* pGame;

int g_bOnlyUpdateRotations = false;

CPedSA::CPedSA(CPedSAInterface* pPedInterface) noexcept
    : m_pPedInterface(pPedInterface)
{
    MemSetFast(m_pWeapons, 0, sizeof(CWeaponSA*) * WEAPONSLOT_MAX);
}

void CPedSA::SetInterface(CEntitySAInterface* intInterface)
{
    m_pInterface = intInterface;
}

CPedSA::~CPedSA()
{
    if (m_pPedIntelligence)
        delete m_pPedIntelligence;
    if (m_pPedSound)
        delete m_pPedSound;

    for (int i = 0; i < WEAPONSLOT_MAX; i++)
    {
        if (m_pWeapons[i])
            delete m_pWeapons[i];
    }

    // Make sure this ped is not refed in the flame shot info array
    CFlameShotInfo* pInfo = (CFlameShotInfo*)ARRAY_CFlameShotInfo;
    for (uint i = 0; i < MAX_FLAME_SHOT_INFOS; i++)
    {
        if (pInfo->pInstigator == m_pInterface)
        {
            pInfo->pInstigator = NULL;
            pInfo->ucFlag1 = 0;
        }
        pInfo++;
    }
}

// used to init weapons at the moment, called by CPlayerPedSA when its been constructed
// can't use constructor as thats called before the interface pointer has been set the the aforementioned constructor
/**
 * \todo Reimplement weapons and PedIK for SA
 */

void CPedSA::Init()
{
    CPedSAInterface* pedInterface = GetPedInterface();

    DWORD dwPedIntelligence = 0;
    DWORD dwFunc = 0x411DE0;
    DWORD dwInterface = (DWORD)pedInterface;
    _asm
    {
        mov     ecx, dwInterface
        call    dwFunc
        mov     dwPedIntelligence, eax
    }
    CPedIntelligenceSAInterface* m_pPedIntelligenceInterface = (CPedIntelligenceSAInterface*)(dwPedIntelligence);
    m_pPedIntelligence = new CPedIntelligenceSA(m_pPedIntelligenceInterface, this);
    m_pPedSound = new CPedSoundSA(&pedInterface->pedSound);

    m_sDefaultVoiceType = m_pPedSound->GetVoiceTypeID();
    m_sDefaultVoiceID = m_pPedSound->GetVoiceID();

    for (int i = 0; i < WEAPONSLOT_MAX; i++)
        m_pWeapons[i] = new CWeaponSA(&(pedInterface->Weapons[i]), this, (eWeaponSlot)i);

    // this->m_pPedIK = new Cm_pPedIKSA(&(pedInterface->m_pPedIK));
}

void CPedSA::SetModelIndex(DWORD dwModelIndex)
{
    // Delete any existing RwObject first
    GetPedInterface()->DeleteRwObject();

    // Set new model
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwFunction = FUNC_SetModelIndex;
    _asm
    {
        mov     ecx, dwThis
        push    dwModelIndex
        call    dwFunction
    }

    // Also set the voice gender
    CPedModelInfoSAInterface* pModelInfo = (CPedModelInfoSAInterface*)pGame->GetModelInfo(dwModelIndex)->GetInterface();
    if (pModelInfo)
    {
        DWORD dwType = pModelInfo->pedType;
        GetPedInterface()->pedSound.m_bIsFemale = (dwType == 5 || dwType == 22);
    }
}

bool CPedSA::IsInWater()
{
    CTask* pTask = m_pPedIntelligence->GetTaskManager()->GetTask(TASK_PRIORITY_EVENT_RESPONSE_NONTEMP);
    return (pTask && (pTask->GetTaskType() == TASK_COMPLEX_IN_WATER));
}

bool CPedSA::AddProjectile(eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity)
{
    return ((CProjectileInfoSA*)pGame->GetProjectileInfo())->AddProjectile((CEntitySA*)this, eWeapon, vecOrigin, fForce, target, targetEntity);
}

void CPedSA::DetachPedFromEntity()
{
    DWORD dwFunc = FUNC_DetachPedFromEntity;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

bool CPedSA::InternalAttachEntityToEntity(DWORD dwEntityInterface, const CVector* vecPosition, const CVector* vecRotation)
{
    AttachPedToEntity(dwEntityInterface, const_cast<CVector*>(vecPosition), 0, 0.0f, WEAPONTYPE_UNARMED, FALSE);
    return true;
}

void CPedSA::AttachPedToEntity(DWORD dwEntityInterface, CVector* vector, unsigned short sDirection, float fRotationLimit, eWeaponType weaponType,
                               bool bChangeCamera)
{
    // sDirection and fRotationLimit only apply to first-person shooting (bChangeCamera)
    DWORD dwFunc = FUNC_AttachPedToEntity;
    DWORD dwThis = (DWORD)GetInterface();
    float fX = vector->fX;
    float fY = vector->fY;
    float fZ = vector->fZ;
    BYTE  bPedType = ((CPedSAInterface*)GetInterface())->bPedType;

    // Hack the CPed type(?) to non-player so the camera doesn't get changed
    if (!bChangeCamera)
        ((CPedSAInterface*)GetInterface())->bPedType = 2;

    _asm
    {
        push    weaponType
        push    fRotationLimit
        movzx   ecx, sDirection
        push    ecx
        push    fZ
        push    fY
        push    fX
        push    dwEntityInterface
        mov     ecx, dwThis
        call    dwFunc
    }

    // Hack the CPed type(?) to whatever it was set to
    if (!bChangeCamera)((CPedSAInterface*)GetInterface())->bPedType = bPedType;
}

CVehicle* CPedSA::GetVehicle()
{
    if (((CPedSAInterface*)GetInterface())->pedFlags.bInVehicle)
    {
        CVehicleSAInterface* vehicle = (CVehicleSAInterface*)(((CPedSAInterface*)GetInterface())->pVehicle);
        if (vehicle)
        {
            SClientEntity<CVehicleSA>* pVehicleClientEntity = pGame->GetPools()->GetVehicle((DWORD*)vehicle);
            return pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        }
    }
    return NULL;
}

void CPedSA::Respawn(CVector* position, bool bCameraCut)
{
    CPed* pLocalPlayer = pGame->GetPools()->GetPedFromRef((DWORD)1);

    if (!bCameraCut)
    {
        // DISABLE call to CCamera__RestoreWithJumpCut when respawning
        MemSet((void*)0x4422EA, 0x90, 20);
    }

    float fX = position->fX;
    float fY = position->fY;
    float fZ = position->fZ;
    float fUnk = 1.0f;
    DWORD dwFunc = FUNC_RestorePlayerStuffDuringResurrection;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    fUnk
        push    fZ
        push    fY
        push    fX
        push    dwThis
        call    dwFunc
        add     esp, 20
    }
#if 0   // Removed to see if it reduces crashes
    dwFunc = 0x441440; // CGameLogic::SortOutStreamingAndMemory
    fUnk = 10.0f;
    _asm
    {
        push    fUnk
        push    position
        call    dwFunc
        add     esp, 8
    }
#endif
    dwFunc = FUNC_RemoveGogglesModel;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    if (!bCameraCut)
    {
        // B9 28 F0 B6 00 E8 4C 9A 0C 00 B9 28 F0 B6 00 E8 B2 97 0C 00
        unsigned char szCode[] = {0xB9, 0x28, 0xF0, 0xB6, 0x00, 0xE8, 0x4C, 0x9A, 0x0C, 0x00, 0xB9, 0x28, 0xF0, 0xB6, 0x00, 0xE8, 0xB2, 0x97, 0x0C, 0x00};
        // RE-ENABLE call to CCamera__RestoreWithJumpCut when respawning
        MemCpy((void*)0x4422EA, szCode, 20);
    }
    // OutputDebugString ( "Respawn!!!!" );
}

float CPedSA::GetHealth()
{
    return GetPedInterface()->fHealth;
}

void CPedSA::SetHealth(float fHealth)
{
    GetPedInterface()->fHealth = fHealth;
}

float CPedSA::GetArmor() noexcept
{
    return GetPedInterface()->fArmor;
}

void CPedSA::SetArmor(float armor) noexcept
{
    GetPedInterface()->fArmor = armor;
}

float CPedSA::GetOxygenLevel()
{
    return GetPedInterface()->pPlayerData->m_fBreath;
}

void CPedSA::SetOxygenLevel(float fOxygen)
{
    GetPedInterface()->pPlayerData->m_fBreath = fOxygen;
}

void CPedSA::SetIsStanding(bool bStanding)
{
    DWORD dwFunc = FUNC_SetIsStanding;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    bStanding
        call    dwFunc
    }
}

DWORD CPedSA::GetType()
{
    return m_dwType;
}

void CPedSA::SetType(DWORD dwType)
{
    m_dwType = dwType;
}

void CPedSA::RemoveWeaponModel(int iModel)
{
    DWORD dwFunc = FUNC_RemoveWeaponModel;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iModel
        call    dwFunc
    }
}

void CPedSA::ClearWeapon(eWeaponType weaponType)
{
    DWORD dwFunc = FUNC_ClearWeapon;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    weaponType
        call    dwFunc
    }
}

CWeapon* CPedSA::GiveWeapon(eWeaponType weaponType, unsigned int uiAmmo, eWeaponSkill skill)
{
    if (weaponType != WEAPONTYPE_UNARMED)
    {
        CWeaponInfo* pInfo = pGame->GetWeaponInfo(weaponType, skill);
        if (pInfo)
        {
            int iModel = pInfo->GetModel();

            if (iModel)
            {
                CModelInfo* pWeaponModel = pGame->GetModelInfo(iModel);
                if (pWeaponModel)
                {
                    pWeaponModel->Request(BLOCKING, "CPedSA::GiveWeapon");
                    pWeaponModel->MakeCustomModel();
                }
            }
            // If the weapon is satchels, load the detonator too
            if (weaponType == WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
            {
                /*int iModel = pGame->GetWeaponInfo ( WEAPONTYPE_DETONATOR )->GetModel();
                if ( iModel )
                {
                    CModelInfo * pWeaponModel = pGame->GetModelInfo ( iModel );
                    if ( pWeaponModel )
                    {
                        pWeaponModel->Request ( true, true );
                    }
                }*/
                // Load the weapon and give it properly so getPedWeapon shows the weapon is there.
                GiveWeapon(WEAPONTYPE_DETONATOR, 1, WEAPONSKILL_STD);
            }
        }
    }

    DWORD dwReturn = 0;
    DWORD dwFunc = FUNC_GiveWeapon;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    1
        push    uiAmmo
        push    weaponType
        call    dwFunc
        mov     dwReturn, eax
    }

    CWeapon* pWeapon = GetWeapon((eWeaponSlot)dwReturn);

    return pWeapon;
}

CWeapon* CPedSA::GetWeapon(eWeaponType weaponType)
{
    if (weaponType < WEAPONTYPE_LAST_WEAPONTYPE)
    {
        CWeapon* pWeapon = GetWeapon(pGame->GetWeaponInfo(weaponType)->GetSlot());
        if (pWeapon->GetType() == weaponType)
            return pWeapon;
    }
    return NULL;
}

CWeapon* CPedSA::GetWeapon(eWeaponSlot weaponSlot)
{
    if (weaponSlot < WEAPONSLOT_MAX)
    {
        return m_pWeapons[weaponSlot];
    }
    else
        return NULL;
}

void CPedSA::ClearWeapons()
{
    // Remove all the weapons
    for (unsigned int i = 0; i < WEAPONSLOT_MAX; i++)
    {
        if (m_pWeapons[i])
        {
            m_pWeapons[i]->SetAmmoInClip(0);
            m_pWeapons[i]->SetAmmoTotal(0);
            m_pWeapons[i]->Remove();
        }
    }
}

void CPedSA::RestoreLastGoodPhysicsState()
{
    CPhysicalSA::RestoreLastGoodPhysicsState();
    SetCurrentRotation(0);
    SetTargetRotation(0);
}

float CPedSA::GetCurrentRotation()
{
    return GetPedInterface()->fCurrentRotation;
}

float CPedSA::GetTargetRotation()
{
    return GetPedInterface()->fTargetRotation;
}

void CPedSA::SetCurrentRotation(float fRotation)
{
    GetPedInterface()->fCurrentRotation = fRotation;

    //  char szDebug[255] = {'\0'};
    //  sprintf(szDebug,"CurrentRotate Offset: %d", ((DWORD)&((CPedSAInterface *)GetInterface())->CurrentRotate) -  (DWORD)GetInterface());
    //  OutputDebugString(szDebug);
}

void CPedSA::SetTargetRotation(float fRotation)
{
    GetPedInterface()->fTargetRotation = fRotation;
}

eWeaponSlot CPedSA::GetCurrentWeaponSlot()
{
    return (eWeaponSlot)GetPedInterface()->bCurrentWeaponSlot;
}

void CPedSA::SetCurrentWeaponSlot(eWeaponSlot weaponSlot)
{
    if (weaponSlot < WEAPONSLOT_MAX)
    {
        eWeaponSlot currentSlot = GetCurrentWeaponSlot();
        if (weaponSlot != GetCurrentWeaponSlot())
        {
            CWeapon* weapon = GetWeapon(currentSlot);
            if (weapon)
            {
                CWeaponInfo* weaponInfo = weapon->GetInfo(WEAPONSKILL_STD);

                if (weaponInfo)
                    RemoveWeaponModel(weaponInfo->GetModel());
            }

            CPedSAInterface* thisPed = (CPedSAInterface*)GetInterface();

            // set the new weapon slot
            thisPed->bCurrentWeaponSlot = weaponSlot;

            // is the player the local player?
            CPed* pPed = pGame->GetPools()->GetPedFromRef((DWORD)1);
            // if ( pPed == this && thisPed->pPlayerInfo )
            //{

            DWORD dwThis = (DWORD)GetInterface();
            if (pPed == this)
            {
                ((CPlayerInfoSA*)pGame->GetPlayerInfo())->GetInterface()->PlayerPedData.m_nChosenWeapon = weaponSlot;

                DWORD dwFunc = FUNC_MakeChangesForNewWeapon_Slot;
                _asm
                {
                    mov     ecx, dwThis
                    push    weaponSlot
                    call    dwFunc
                }
            }
            else
            {
                DWORD dwFunc = FUNC_SetCurrentWeapon;
                _asm
                {
                    mov     ecx, dwThis
                    push    weaponSlot
                    call    dwFunc
                }
            }
        }
    }
}

CVector* CPedSA::GetBonePosition(eBone bone, CVector* vecPosition)
{
    ApplySwimAndSlopeRotations();
    CEntitySAInterface* entity = GetInterface();

    // NOTE(botder): A crash used to occur at 0x749B7B in RpClumpForAllAtomics, because the clump pointer might have been null
    // for a broken model.
    if (entity->m_pRwObject != nullptr)
    {
        // void __thiscall CPed::GetBonePosition(struct RwV3d &, unsigned int, bool)
        using Signature = void(__thiscall*)(CEntitySAInterface*, CVector*, unsigned int, bool);
        const auto GameFunction = reinterpret_cast<Signature>(FUNC_GetBonePosition);
        GameFunction(entity, vecPosition, bone, true);
    }

    // Clamp to a sane range as this function can occasionally return massive values,
    // which causes ProcessLineOfSight to effectively freeze
    if (!IsValidPosition(*vecPosition))
    {
        *vecPosition = *GetPosition();
    }

    return vecPosition;
}

CVector* CPedSA::GetTransformedBonePosition(eBone bone, CVector* vecPosition)
{
    ApplySwimAndSlopeRotations();
    CEntitySAInterface* entity = GetInterface();

    // NOTE(botder): A crash used to occur at 0x7C51A8 in RpHAnimIDGetIndex, because the clump pointer might have been null
    // for a broken model.
    if (entity->m_pRwObject != nullptr)
    {
        // void __thiscall CPed::GetTransformedBonePosition(struct RwV3d &, unsigned int, bool)
        using Signature = void(__thiscall*)(CEntitySAInterface*, CVector*, unsigned int, bool);
        const auto GameFunction = reinterpret_cast<Signature>(FUNC_GetTransformedBonePosition);
        GameFunction(entity, vecPosition, bone, true);
    }

    // Clamp to a sane range as this function can occasionally return massive values,
    // which causes ProcessLineOfSight to effectively freeze
    if (!IsValidPosition(*vecPosition))
    {
        *vecPosition = *GetPosition();
    }

    return vecPosition;
}

//
// Apply the extra ped rotations for slope pitch and swimming.
// Achieved by calling the code at the start of CPed::PreRenderAfterTest
//
void CPedSA::ApplySwimAndSlopeRotations()
{
    CPedSAInterface* pPedInterface = GetPedInterface();
    if (pPedInterface->pedFlags.bCalledPreRender)
        return;

    g_bOnlyUpdateRotations = true;

    DWORD dwFunc = FUNC_PreRenderAfterTest;
    _asm
    {
        mov     ecx, pPedInterface
        call    dwFunc
    }

    g_bOnlyUpdateRotations = false;
}

bool CPedSA::IsDucking()
{
    return ((CPedSAInterface*)GetInterface())->pedFlags.bIsDucking;
}

void CPedSA::SetDucking(bool bDuck)
{
    ((CPedSAInterface*)GetInterface())->pedFlags.bIsDucking = bDuck;
}

int CPedSA::GetCantBeKnockedOffBike()
{
    return ((CPedSAInterface*)GetInterface())->pedFlags.CantBeKnockedOffBike;
}

void CPedSA::SetCantBeKnockedOffBike(int iCantBeKnockedOffBike)
{
    ((CPedSAInterface*)GetInterface())->pedFlags.CantBeKnockedOffBike = iCantBeKnockedOffBike;
}

void CPedSA::QuitEnteringCar(CVehicle* vehicle, int iSeat, bool bUnknown)
{
    CVehicleSA* pVehicleSA = dynamic_cast<CVehicleSA*>(vehicle);
    if (!pVehicleSA)
        return;

    DWORD dwFunc = FUNC_QuitEnteringCar;
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwVehicle = (DWORD)pVehicleSA->GetInterface();
    _asm
    {
        push    bUnknown
        push    iSeat
        push    dwVehicle
        push    dwThis
        call    dwFunc
        add     esp, 16
    }
}

bool CPedSA::IsWearingGoggles()
{
    DWORD dwFunc = FUNC_IsWearingGoggles;
    DWORD dwThis = (DWORD)GetInterface();
    bool  bReturn = false;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CPedSA::SetGogglesState(bool bIsWearingThem)
{
    DWORD dwFunc = FUNC_TakeOffGoggles;
    if (bIsWearingThem)
        dwFunc = FUNC_PutOnGoggles;

    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CPedSA::SetClothesTextureAndModel(const char* szTexture, const char* szModel, int textureType)
{
    DWORD dwFunc = FUNC_CPedClothesDesc__SetTextureAndModel;
    // DWORD dwThis = (DWORD)this->GetInterface()->PlayerPedData.m_pClothes;
    DWORD dwThis = (DWORD)((CPedSAInterface*)GetInterface())->pPlayerData->m_pClothes;
    _asm
    {
        mov     ecx, dwThis
        push    textureType
        push    szModel
        push    szTexture
        call    dwFunc
    }
}

void CPedSA::RebuildPlayer()
{
    DWORD dwFunc = FUNC_CClothes__RebuildPlayer;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    0
        push    dwThis
        call    dwFunc
        add     esp, 8
    }
}

eFightingStyle CPedSA::GetFightingStyle()
{
    return (eFightingStyle)((CPedSAInterface*)GetInterface())->bFightingStyle;
}

void CPedSA::SetFightingStyle(eFightingStyle style, BYTE bStyleExtra)
{
    BYTE  bStyle = (BYTE)style;
    BYTE* pFightingStyle = &((CPedSAInterface*)GetInterface())->bFightingStyle;
    BYTE* pFightingStyleExtra = &((CPedSAInterface*)GetInterface())->bFightingStyleExtra;
    if (bStyle != *pFightingStyle)
    {
        *pFightingStyle = bStyle;
        if (bStyleExtra > 0 && bStyleExtra <= 6)
        {
            bStyleExtra--;
            BYTE bTemp = *pFightingStyleExtra;
            switch (bStyleExtra)
            {
                case 0:
                    break;
                case 1:
                    bTemp |= 1;
                    break;
                case 2:
                    bTemp |= 2;
                    break;
                case 3:
                    bTemp |= 4;
                    break;
                case 4:
                    bTemp |= 8;
                    break;
                case 5:
                    bTemp |= 16;
                    break;
            }
            *pFightingStyleExtra = bTemp;
        }
    }
}

CEntity* CPedSA::GetContactEntity()
{
    CEntitySAInterface* pInterface = ((CPedSAInterface*)GetInterface())->pContactEntity;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        switch (pInterface->nType)
        {
            case ENTITY_TYPE_VEHICLE:
            {
                SClientEntity<CVehicleSA>* pVehicleClientEntity = pPools->GetVehicle((DWORD*)pInterface);
                if (pVehicleClientEntity)
                {
                    return pVehicleClientEntity->pEntity;
                }
                break;
            }
            case ENTITY_TYPE_OBJECT:
            {
                SClientEntity<CObjectSA>* pObjectClientEntity = pPools->GetObject((DWORD*)pInterface);
                if (pObjectClientEntity)
                {
                    return pObjectClientEntity->pEntity;
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
    return nullptr;
}

unsigned char CPedSA::GetRunState()
{
    return *(unsigned char*)(((DWORD)(GetInterface()) + 1332));
}

CEntity* CPedSA::GetTargetedEntity()
{
    CEntitySAInterface* pInterface = ((CPedSAInterface*)GetInterface())->pTargetedEntity;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        return pPools->GetEntity((DWORD*)pInterface);
    }
    return nullptr;
}

void CPedSA::SetTargetedEntity(CEntity* pEntity)
{
    CEntitySAInterface* pInterface = NULL;
    if (pEntity)
    {
        CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(pEntity);
        if (pEntitySA)
            pInterface = pEntitySA->GetInterface();
    }

    ((CPedSAInterface*)GetInterface())->pTargetedEntity = pInterface;
}

bool CPedSA::GetCanBeShotInVehicle()
{
    return GetPedInterface()->pedFlags.bCanBeShotInVehicle;
}

bool CPedSA::GetTestForShotInVehicle()
{
    return GetPedInterface()->pedFlags.bTestForShotInVehicle;
}

void CPedSA::SetCanBeShotInVehicle(bool bShot)
{
    GetPedInterface()->pedFlags.bCanBeShotInVehicle = bShot;
}

void CPedSA::SetTestForShotInVehicle(bool bTest)
{
    GetPedInterface()->pedFlags.bTestForShotInVehicle = bTest;
}

void CPedSA::RemoveBodyPart(int i, char c)
{
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwFunc = FUNC_CPed_RemoveBodyPart;
    _asm
    {
        mov     ecx, dwThis
        push    c
        push    i
        call    dwFunc
    }
}

void CPedSA::SetFootBlood(unsigned int uiFootBlood)
{
    DWORD dwThis = (DWORD)GetInterface();
    // Check if the ped is to have foot blood
    if (uiFootBlood > 0)
    {
        // Make sure the foot blood flag is activated
        MemOrFast<unsigned short>(dwThis + 0x46F, 16);
    }
    else if (*(unsigned short*)(dwThis + 0x46F) & 16)
    {
        // If the foot blood flag is activated, deactivate it
        MemSubFast<unsigned short>(dwThis + 0x46F, 16);
    }
    // Set the amount of foot blood
    MemPutFast<unsigned int>(dwThis + 0x750, uiFootBlood);
}

unsigned int CPedSA::GetFootBlood()
{
    DWORD dwThis = (DWORD)GetInterface();
    // Check if the ped has the foot blood flag
    if (*(unsigned short*)(dwThis + 0x46F) & 16)
    {
        // If the foot blood flag is activated, return the amount of foot blood
        return *(unsigned int*)(dwThis + 0x750);
    }
    // Otherwise, return zero as there is no foot blood
    return 0;
}

bool CPedSA::IsBleeding()
{
    return GetPedInterface()->pedFlags.bPedIsBleeding;
}

void CPedSA::SetBleeding(bool bBleeding)
{
    GetPedInterface()->pedFlags.bPedIsBleeding = bBleeding;
}

bool CPedSA::SetOnFire(bool onFire)
{
    CPedSAInterface* pInterface = GetPedInterface();
    if (onFire == !!pInterface->pFireOnPed)
        return false;

    auto* fireManager = static_cast<CFireManagerSA*>(pGame->GetFireManager());

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

void CPedSA::SetStayInSamePlace(bool bStay)
{
    GetPedInterface()->pedFlags.bStayInSamePlace = bStay;
}

void CPedSA::GetVoice(short* psVoiceType, short* psVoiceID)
{
    if (psVoiceType)
        *psVoiceType = m_pPedSound->GetVoiceTypeID();
    if (psVoiceID)
        *psVoiceID = m_pPedSound->GetVoiceID();
}

void CPedSA::GetVoice(const char** pszVoiceType, const char** pszVoice)
{
    short sVoiceType, sVoiceID;
    GetVoice(&sVoiceType, &sVoiceID);
    if (pszVoiceType)
        *pszVoiceType = CPedSoundSA::GetVoiceTypeNameFromID(sVoiceType);
    if (pszVoice)
        *pszVoice = CPedSoundSA::GetVoiceNameFromID(sVoiceType, sVoiceID);
}

void CPedSA::SetVoice(short sVoiceType, short sVoiceID)
{
    m_pPedSound->SetVoiceTypeID(sVoiceType);
    m_pPedSound->SetVoiceID(sVoiceID);
}

void CPedSA::SetVoice(const char* szVoiceType, const char* szVoice)
{
    short sVoiceType = CPedSoundSA::GetVoiceTypeIDFromName(szVoiceType);
    if (sVoiceType < 0)
        return;
    short sVoiceID = CPedSoundSA::GetVoiceIDFromName(sVoiceType, szVoice);
    if (sVoiceID < 0)
        return;
    SetVoice(sVoiceType, sVoiceID);
}

void CPedSA::ResetVoice()
{
    SetVoice(m_sDefaultVoiceType, m_sDefaultVoiceID);
}

// GetCurrentWeaponStat will only work if the game ped context is currently set to this ped
CWeaponStat* CPedSA::GetCurrentWeaponStat()
{
    if (pGame->GetPedContext() != this)
    {
        OutputDebugLine("WARNING: GetCurrentWeaponStat ped context mismatch");
        return NULL;
    }

    CWeapon* pWeapon = GetWeapon(GetCurrentWeaponSlot());

    if (!pWeapon)
        return NULL;

    eWeaponType  eWeapon = pWeapon->GetType();
    ushort       usStat = pGame->GetStats()->GetSkillStatIndex(eWeapon);
    float        fSkill = pGame->GetStats()->GetStatValue(usStat);
    CWeaponStat* pWeaponStat = pGame->GetWeaponStatManager()->GetWeaponStatsFromSkillLevel(eWeapon, fSkill);
    return pWeaponStat;
}

float CPedSA::GetCurrentWeaponRange()
{
    CWeaponStat* pWeaponStat = GetCurrentWeaponStat();
    if (!pWeaponStat)
        return 1;

    return pWeaponStat->GetWeaponRange();
}

void CPedSA::AddWeaponAudioEvent(EPedWeaponAudioEventType audioEventType)
{
    DWORD                             dwFunc = FUNC_CAEPedWeaponAudioEntity__AddAudioEvent;
    CPedWeaponAudioEntitySAInterface* pThis = &GetPedInterface()->weaponAudioEntity;
    _asm
    {
        mov     ecx, pThis
        push    audioEventType
        call    dwFunc
    }
}

int CPedSA::GetCustomMoveAnim()
{
    return m_iCustomMoveAnim;
}

bool CPedSA::IsDoingGangDriveby()
{
    auto   pTaskManager = m_pPedIntelligence->GetTaskManager();
    CTask* pTask = pTaskManager->GetTask(TASK_PRIORITY_PRIMARY);
    if (pTask && pTask->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY)
    {
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////
//
// CPed_PreRenderAfterTest
//
// Code at start of CPed::PreRenderAfterTest applies extra rotations for slope pitch and swimming.
// Check if they have already been applied.
//
////////////////////////////////////////////////////////////////
__declspec(noinline) int _cdecl OnCPed_PreRenderAfterTest(CPedSAInterface* pPedInterface)
{
    if (pPedInterface->pedFlags.bCalledPreRender)
        return 1;            // Skip slope and swim rotations
    return 0;
}

// Hook info
#define HOOKPOS_CPed_PreRenderAfterTest        0x05E65A0
#define HOOKSIZE_CPed_PreRenderAfterTest       15
DWORD RETURN_CPed_PreRenderAfterTest = 0x05E65AF;
DWORD RETURN_CPed_PreRenderAfterTestSkip = 0x05E6658;
void _declspec(naked) HOOK_CPed_PreRenderAfterTest()
{
    _asm
    {
        pushad
        push    ecx                 // this
        call    OnCPed_PreRenderAfterTest
        mov     [esp+0],eax         // Put result temp
        add     esp, 4*1
        popad

        mov     eax,[esp-32-4*1]    // Get result temp

        // Replaced code
        sub         esp,70h
        push        ebx
        push        ebp
        push        esi
        mov         ebp,ecx
        mov         ecx,dword ptr [ebp+47Ch]
        push        edi

        // Check what to do
        cmp     eax,0
        jnz     skip_rotation_update

        // Run code at start of CPed::PreRenderAfterTest
        jmp     RETURN_CPed_PreRenderAfterTest

skip_rotation_update:
        // Skip code at start of CPed::PreRenderAfterTest
        jmp     RETURN_CPed_PreRenderAfterTestSkip
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

// Hook info
#define HOOKPOS_CPed_PreRenderAfterTest_Mid        0x05E6669
#define HOOKSIZE_CPed_PreRenderAfterTest_Mid       5
DWORD RETURN_CPed_PreRenderAfterTest_Mid = 0x05E666E;
DWORD RETURN_CPed_PreRenderAfterTest_MidSkip = 0x05E766F;
void _declspec(naked) HOOK_CPed_PreRenderAfterTest_Mid()
{
    _asm
    {
        // Check what to do
        mov     eax, g_bOnlyUpdateRotations
        cmp     eax,0
        jnz     skip_tail

        // Replaced code
        mov     al,byte ptr ds:[00B7CB89h]
        // Run code at mid of CPed::PreRenderAfterTest
        jmp     RETURN_CPed_PreRenderAfterTest_Mid

skip_tail:
        // Skip code at mid of CPed::PreRenderAfterTest
        jmp     RETURN_CPed_PreRenderAfterTest_MidSkip
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CPedSA::StaticSetHooks()
{
    EZHookInstall(CPed_PreRenderAfterTest);
    EZHookInstall(CPed_PreRenderAfterTest_Mid);
}

void CPedSA::GetAttachedSatchels(std::vector<SSatchelsData>& satchelsList) const
{
    // Array of projectiles objects
    CProjectileSAInterface** projectilesArray = (CProjectileSAInterface**)ARRAY_CProjectile;
    CProjectileSAInterface*  pProjectileInterface;

    // Array of projectiles infos
    CProjectileInfoSAInterface* projectilesInfoArray = (CProjectileInfoSAInterface*)ARRAY_CProjectileInfo;
    CProjectileInfoSAInterface* pProjectileInfoInterface;

    // Loop through all projectiles
    for (size_t i = 0; i < PROJECTILE_COUNT; i++)
    {
        pProjectileInterface = projectilesArray[i];

        // is attached to our ped?
        if (!pProjectileInterface || pProjectileInterface->m_pAttachedEntity != m_pInterface)
            continue;

        // index is always the same for both arrays
        pProjectileInfoInterface = &projectilesInfoArray[i];

        // We are only interested in satchels
        if (!pProjectileInfoInterface || pProjectileInfoInterface->dwProjectileType != eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
            continue;

        // Push satchel into the array. There is no need to check the counter because for satchels it restarts until the player detonates the charges
        satchelsList.push_back({pProjectileInterface, &pProjectileInterface->m_vecAttachedOffset, &pProjectileInterface->m_vecAttachedRotation});
    }
}
