/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPed.cpp
 *  PURPOSE:     Ped entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPed.h"
#include "CPedManager.h"
#include "CLogger.h"
#include "Utils.h"
#include "CStaticFunctionDefinitions.h"

char szBodyPartNameEmpty[] = "";
struct SBodyPartName
{
    const char szName[32];
};
SBodyPartName BodyPartNames[10] = {{"Unknown"},  {"Unknown"},   {"Unknown"},  {"Torso"},     {"Ass"},
                                   {"Left Arm"}, {"Right Arm"}, {"Left Leg"}, {"Right Leg"}, {"Head"}};

CPed::CPed(CPedManager* pPedManager, CElement* pParent, unsigned short usModel) : CElement(pParent)
{
    // Init
    m_pPedManager = pPedManager;

    m_iType = CElement::PED;
    SetTypeName("ped");

    m_usModel = usModel;
    m_bDucked = false;
    m_bIsChoking = false;
    m_bWearingGoggles = false;

    m_fHealth = 0.0f;
    m_fArmor = 0.0f;

    memset(&m_fStats[0], 0, sizeof(m_fStats));
    m_fStats[24] = 569.0f;            // default max_health

    m_pClothes = new CPlayerClothes;
    m_pClothes->DefaultClothes();
    m_bHasJetPack = false;
    m_bHeadless = false;
    m_bInWater = false;
    m_bOnGround = true;
    m_bIsPlayer = false;
    m_bFrozen = false;
    m_bIsOnFire = false;

    m_pTasks = new CPlayerTasks;

    m_ucWeaponSlot = 0;
    memset(&m_Weapons[0], 0, sizeof(m_Weapons));
    m_ucAlpha = 255;
    m_pContactElement = NULL;
    m_bIsDead = false;
    m_bSpawned = false;
    m_fRotation = 0.0f;
    m_pTargetedEntity = NULL;
    m_ucFightingStyle = 15;            // STYLE_GRAB_KICK
    m_iMoveAnim = MOVE_DEFAULT;
    m_fGravity = 0.008f;
    m_bDoingGangDriveby = false;
    m_bStealthAiming = false;
    m_bReloadingWeapon = false;

    m_pVehicle = NULL;
    m_uiVehicleSeat = INVALID_VEHICLE_SEAT;
    m_uiVehicleAction = CPed::VEHICLEACTION_NONE;

    m_vecVelocity.fX = m_vecVelocity.fY = m_vecVelocity.fZ = 0.0f;

    m_pSyncer = NULL;

    m_bCollisionsEnabled = true;

    m_pJackingVehicle = NULL;
    m_nearPlayersList.reserve(20);

    // Add us to the Ped manager
    if (pPedManager)
    {
        pPedManager->AddToList(this);
    }
}

CPed::~CPed()
{
    // Abort any jacking process
    if (m_pJackingVehicle)
    {
        if (m_uiVehicleAction == VEHICLEACTION_JACKING)
        {
            CPed* pOccupant = m_pJackingVehicle->GetOccupant(0);
            if (pOccupant)
            {
                m_pJackingVehicle->SetOccupant(NULL, 0);
                pOccupant->SetOccupiedVehicle(NULL, 0);
                pOccupant->SetVehicleAction(VEHICLEACTION_NONE);
            }
        }
        if (m_pJackingVehicle->GetJackingPed() == this)
            m_pJackingVehicle->SetJackingPed(NULL);
    }

    // Make sure we've no longer occupied any vehicle
    if (m_pVehicle)
    {
        m_pVehicle->SetOccupant(NULL, m_uiVehicleSeat);
    }

    SetSyncer(NULL);

    delete m_pClothes;
    delete m_pTasks;

    if (m_pContactElement)
        m_pContactElement->RemoveOriginSourceUser(this);

    // Remove us from the Ped manager
    Unlink();
}

CElement* CPed::Clone(bool* bAddEntity, CResource* pResource)
{
    CPed* const pTemp = m_pPedManager->Create(GetModel(), GetParentEntity());

    if (pTemp)
    {
        pTemp->SetPosition(GetPosition());
        pTemp->SetRotation(GetRotation());
        pTemp->SetHealth(GetHealth());
        pTemp->SetArmor(GetArmor());
        pTemp->SetSyncable(IsSyncable());
        pTemp->SetSpawned(IsSpawned());
        pTemp->SetIsDead(IsDead());
    }

    return pTemp;
}

void CPed::Unlink()
{
    // Remove us from the Ped manager
    if (m_pPedManager)
    {
        m_pPedManager->RemoveFromList(this);
    }
}

void CPed::GetRotation(CVector& vecRotation)
{
    vecRotation = CVector(0, 0, GetRotation());
}

void CPed::GetMatrix(CMatrix& matrix)
{
    CVector vecRotation;
    vecRotation.fZ = GetRotation();
    matrix.SetRotation(vecRotation);
    matrix.vPos = GetPosition();
}

void CPed::SetMatrix(const CMatrix& matrix)
{
    // Set position and rotation from matrix
    SetPosition(matrix.vPos);
    CVector vecRotation = matrix.GetRotation();
    SetRotation(vecRotation.fZ);
}

bool CPed::ReadSpecialData(const int iLine)
{
    // Grab the "posX" data
    if (!GetCustomDataFloat("posX", m_vecPosition.fX, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posX' attribute in <ped> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posY" data
    if (!GetCustomDataFloat("posY", m_vecPosition.fY, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posY' attribute in <ped> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posZ" data
    if (!GetCustomDataFloat("posZ", m_vecPosition.fZ, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posZ' attribute in <ped> (line %d)\n", iLine);
        return false;
    }

    // Grab the "rotZ" data
    float fRotation = 0.0f;
    GetCustomDataFloat("rotZ", fRotation, true);
    m_fRotation = ConvertDegreesToRadians(fRotation);

    // Grab the "model" data
    int iTemp;
    if (GetCustomDataInt("model", iTemp, true))
    {
        // Is it valid?
        unsigned short usModel = static_cast<unsigned short>(iTemp);
        if (CPedManager::IsValidModel(usModel))
        {
            // Remember it and generate a new random color
            m_usModel = usModel;
        }
        else
        {
            CLogger::ErrorPrintf("Bad 'model' (%d) id specified in <ped> (line %d)\n", iTemp, iLine);
            return false;
        }
    }
    else
    {
        CLogger::ErrorPrintf("Bad/missing 'model' attribute in <ped> (line %d)\n", iLine);
        return false;
    }

    // Grab the "health" data
    if (GetCustomDataFloat("health", m_fHealth, true))
    {
        // Limit it to 0-100 (we can assume max health is 100 because they can't change stats here)
        if (m_fHealth > 100)
            m_fHealth = 100;
        else if (m_fHealth < 0)
            m_fHealth = 0;
    }
    else
        // Set health to 100 if not defined
        m_fHealth = 100.0f;

    // Grab the "armor" data
    GetCustomDataFloat("armor", m_fArmor, true);

    // Grab the "interior" data
    if (GetCustomDataInt("interior", iTemp, true))
        m_ucInterior = static_cast<unsigned char>(iTemp);

    // Grab the "dimension" data
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    // Grab the "collisions" data
    if (!GetCustomDataBool("collisions", m_bCollisionsEnabled, true))
        m_bCollisionsEnabled = true;

    // Grab the "alpha" data
    if (GetCustomDataInt("alpha", iTemp, true))
        m_ucAlpha = static_cast<unsigned char>(iTemp);

    // Grab the "frozen" data
    GetCustomDataBool("frozen", m_bFrozen, true);

    // Grab the "headless" data
    GetCustomDataBool("headless", m_bHeadless, true);

    // Grab the "walkingStyle" data
    GetCustomDataInt("walkingStyle", m_iMoveAnim, true);

    return true;
}

bool CPed::HasValidModel()
{
    return CPedManager::IsValidModel(m_usModel);
}

void CPed::SetWeaponSlot(unsigned char ucSlot)
{
    if (ucSlot < WEAPON_SLOTS)
    {
        m_ucWeaponSlot = ucSlot;
    }
}

CWeapon* CPed::GetWeapon(unsigned char ucSlot)
{
    if (ucSlot == 0xFF)
        ucSlot = m_ucWeaponSlot;
    if (ucSlot < WEAPON_SLOTS)
    {
        return &m_Weapons[ucSlot];
    }
    return NULL;
}

unsigned char CPed::GetWeaponType(unsigned char ucSlot)
{
    if (ucSlot == 0xFF)
        ucSlot = m_ucWeaponSlot;
    if (ucSlot < WEAPON_SLOTS)
    {
        return m_Weapons[ucSlot].ucType;
    }
    return 0;
}

void CPed::SetWeaponType(unsigned char ucType, unsigned char ucSlot)
{
    if (ucSlot == 0xFF)
        ucSlot = m_ucWeaponSlot;
    if (ucSlot < WEAPON_SLOTS)
    {
        m_Weapons[ucSlot].ucType = ucType;
    }
}

unsigned short CPed::GetWeaponAmmoInClip(unsigned char ucSlot)
{
    if (ucSlot == 0xFF)
        ucSlot = m_ucWeaponSlot;
    if (ucSlot < WEAPON_SLOTS)
    {
        return m_Weapons[ucSlot].usAmmoInClip;
    }
    return 0;
}

void CPed::SetWeaponAmmoInClip(unsigned short usAmmoInClip, unsigned char ucSlot)
{
    if (ucSlot == 0xFF)
        ucSlot = m_ucWeaponSlot;
    if (ucSlot < WEAPON_SLOTS)
    {
        m_Weapons[ucSlot].usAmmoInClip = usAmmoInClip;
    }
}

unsigned short CPed::GetWeaponTotalAmmo(unsigned char ucSlot)
{
    if (ucSlot == 0xFF)
        ucSlot = m_ucWeaponSlot;
    if (ucSlot < WEAPON_SLOTS)
    {
        return m_Weapons[ucSlot].usAmmo;
    }
    return 0;
}

void CPed::SetWeaponTotalAmmo(unsigned short usTotalAmmo, unsigned char ucSlot)
{
    if (ucSlot == 0xFF)
        ucSlot = m_ucWeaponSlot;
    if (ucSlot < WEAPON_SLOTS)
    {
        m_Weapons[ucSlot].usAmmo = usTotalAmmo;
    }
}

bool CPed::HasWeaponType(unsigned char ucWeaponType)
{
    for (unsigned char slot = 0; slot < WEAPON_SLOTS; slot++)
    {
        if (GetWeaponType(slot) == ucWeaponType)
            return true;
    }

    return false;
}

float CPed::GetMaxHealth()
{
    // TODO: Verify this formula

    // Grab his player health stat
    float fStat = GetPlayerStat(24 /*MAX_HEALTH*/);

    // Do a linear interpolation to get how much health this would allow
    // Assumes: 100 health = 569 stat, 200 health = 1000 stat.
    float fMaxHealth = 100.0f + (100.0f / 431.0f * (fStat - 569.0f));

    // Return the max health. Make sure it can't be below 1
    if (fMaxHealth < 1.0f)
        fMaxHealth = 1.0f;
    return fMaxHealth;
}

const char* CPed::GetBodyPartName(unsigned char ucID)
{
    if (ucID <= NUMELMS(BodyPartNames))
    {
        return BodyPartNames[ucID].szName;
    }

    return szBodyPartNameEmpty;
}

void CPed::SetContactElement(CElement* pElement)
{
    if (pElement != m_pContactElement)
    {
        if (m_pContactElement)
            m_pContactElement->RemoveOriginSourceUser(this);

        if (pElement)
            pElement->AddOriginSourceUser(this);

        m_pContactElement = pElement;
    }
}

void CPed::SetIsDead(bool bDead)
{
    m_bIsDead = bDead;
}

CVehicle* CPed::SetOccupiedVehicle(CVehicle* pVehicle, unsigned int uiSeat)
{
    static bool bAlreadyIn = false;
    if (!bAlreadyIn)
    {
        // Store it
        m_pVehicle = pVehicle;
        m_uiVehicleSeat = uiSeat;

        // Make sure the vehicle knows
        if (m_pVehicle)
        {
            bAlreadyIn = true;
            pVehicle->SetOccupant(this, uiSeat);
            bAlreadyIn = false;
        }
    }

    return m_pVehicle;
}

void CPed::SetVehicleAction(unsigned int uiAction)
{
    m_uiVehicleAction = uiAction;
}

bool CPed::IsAttachToable()
{
    // We're not attachable if we're inside a vehicle (that would get messy)
    if (!GetOccupiedVehicle())
    {
        return true;
    }
    return false;
}

void CPed::SetSyncer(CPlayer* pPlayer)
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if (!bAlreadyIn)
    {
        // Update the last Player if any
        bAlreadyIn = true;
        if (m_pSyncer)
        {
            m_pSyncer->RemoveSyncingPed(this);
        }

        // Update the vehicle
        if (pPlayer)
        {
            pPlayer->AddSyncingPed(this);
        }
        bAlreadyIn = false;

        // Set it
        m_pSyncer = pPlayer;

        // Check if we are in an enter/exit action
        // We need to complete the process by warping the ped in or out, because the syncer changed
        unsigned int uiAction = GetVehicleAction();
        switch (uiAction)
        {
            case VEHICLEACTION_ENTERING:
            {
                CVehicle*     pVehicle = GetOccupiedVehicle();
                unsigned char ucOccupiedSeat = static_cast<unsigned char>(GetOccupiedVehicleSeat());
                // Does it have an occupant and is the occupant us?
                if (pVehicle && (this == pVehicle->GetOccupant(ucOccupiedSeat)))
                {
                    // Warp us into vehicle
                    CStaticFunctionDefinitions::WarpPedIntoVehicle(this, pVehicle, ucOccupiedSeat);
                }
            }

            case VEHICLEACTION_EXITING:
            {
                CVehicle*     pVehicle = GetOccupiedVehicle();
                unsigned char ucOccupiedSeat = GetOccupiedVehicleSeat();
                // Does it have an occupant and is the occupant us?
                if (pVehicle && (this == pVehicle->GetOccupant(ucOccupiedSeat)))
                {
                    // Warp us out of vehicle
                    CStaticFunctionDefinitions::RemovePedFromVehicle(this);
                }
            }

            case VEHICLEACTION_JACKING:
            {
                CVehicle* pVehicle = GetJackingVehicle();
                if (pVehicle)
                {
                    // Warp us into vehicle in drivers seat
                    // This will warp the existing driver out and reset both our and the jacked peds vehicle action
                    CStaticFunctionDefinitions::WarpPedIntoVehicle(this, pVehicle, 0);
                }
            }
        }
    }
}

void CPed::SetJackingVehicle(CVehicle* pVehicle)
{
    if (pVehicle == m_pJackingVehicle)
        return;

    // Remove old
    if (m_pJackingVehicle)
    {
        CVehicle* pPrev = m_pJackingVehicle;
        m_pJackingVehicle = NULL;
        pPrev->SetJackingPed(NULL);
    }

    // Set new
    m_pJackingVehicle = pVehicle;

    if (m_pJackingVehicle)
        m_pJackingVehicle->SetJackingPed(this);
}
