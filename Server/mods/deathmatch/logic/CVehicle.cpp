/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicle.cpp
 *  PURPOSE:     Vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVehicle.h"
#include "CVehicleManager.h"
#include "CHandlingManager.h"
#include "CElementRefManager.h"
#include "CGame.h"
#include "packets/CVehicleInOutPacket.h"
#include "CBandwidthSettings.h"
#include "Utils.h"

extern CGame* g_pGame;

CVehicle::CVehicle(CVehicleManager* pVehicleManager, CElement* pParent, unsigned short usModel, unsigned char ucVariant, unsigned char ucVariant2)
    : CElement(pParent)
{
    CElementRefManager::AddElementRefs(ELEMENT_REF_DEBUG(this, "CVehicle"), &m_pTowedVehicle, &m_pTowedByVehicle, &m_pSyncer, &m_pJackingPed, NULL);

    // Init
    m_pVehicleManager = pVehicleManager;
    m_usModel = usModel;
    m_pUpgrades = new CVehicleUpgrades(this);

    m_iType = CElement::VEHICLE;
    SetTypeName("vehicle");
    m_eVehicleType = CVehicleManager::GetVehicleType(m_usModel);
    m_fHealth = DEFAULT_VEHICLE_HEALTH;
    m_fLastSyncedHealthHealth = DEFAULT_VEHICLE_HEALTH;
    m_llIdleTime = CTickCount::Now();
    m_fTurretPositionX = 0;
    m_fTurretPositionY = 0;
    m_bSirenActive = false;
    m_bLandingGearDown = true;
    m_usAdjustableProperty = 0;
    m_bIsFrozen = false;
    m_bUnoccupiedSyncable = true;
    m_pSyncer = NULL;
    GetInitialDoorStates(m_ucDoorStates);
    memset(&m_ucWheelStates[0], 0, sizeof(m_ucWheelStates));
    memset(&m_ucPanelStates[0], 0, sizeof(m_ucPanelStates));
    memset(&m_ucLightStates[0], 0, sizeof(m_ucLightStates));
    m_ucOverrideLights = 0;
    m_pTowedVehicle = NULL;
    m_pTowedByVehicle = NULL;
    m_ucPaintjob = 3;
    m_ucMaxPassengersOverride = VEHICLE_PASSENGERS_UNDEFINED;
    m_HandlingEntry = nullptr;

    m_fRespawnHealth = DEFAULT_VEHICLE_HEALTH;
    m_bRespawnEnabled = false;
    m_ulBlowRespawnInterval = 10000;
    m_ulIdleRespawnInterval = 60000;

    m_bEngineOn = false;
    for (unsigned int i = 0; i < 6; ++i)
        m_fDoorOpenRatio[i] = 0.0f;
    m_bLocked = false;
    m_bDoorsUndamageable = false;
    m_bDamageProof = false;
    m_bGunsEnabled = false;
    m_bFuelTankExplodable = false;
    m_bOnGround = true;
    m_bSmokeTrail = false;
    m_ucAlpha = 255;
    m_pJackingPed = NULL;
    m_bInWater = false;
    m_bDerailed = false;
    m_bIsDerailable = true;
    m_fTrainSpeed = 0.0f;
    m_fTrainPosition = 0.0f;
    m_bTaxiLightState = false;
    m_bTrainDirection = false;
    m_HeadLightColor = SColorRGBA(255, 255, 255, 255);
    m_bHeliSearchLightVisible = false;
    m_bCollisionsEnabled = true;
    m_bHandlingChanged = false;
    m_ucVariant = ucVariant;
    m_ucVariant2 = ucVariant2;
    m_onFire = false;

    // Initialize the occupied Players
    for (int i = 0; i < MAX_VEHICLE_SEATS; i++)
    {
        m_pOccupants[i] = NULL;
    }

    // Add us to the vehicle manager
    pVehicleManager->AddToList(this);

    // Randomize our color
    RandomizeColor();

    // Generate a random reg plate
    GenerateRegPlate();

    // Generate the handling data
    GenerateHandlingData();

    // Prepare the sirens
    RemoveVehicleSirens();
    m_tSirenBeaconInfo.m_bOverrideSirens = false;
}

CVehicle::~CVehicle()
{
    if (m_pJackingPed && m_pJackingPed->GetJackingVehicle() == this)
    {
        if (m_pJackingPed->GetVehicleAction() == CPed::VEHICLEACTION_JACKING)
        {
            m_pJackingPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
        }
        m_pJackingPed->SetJackingVehicle(NULL);
    }

    // loop through peds and fix their in out state
    for (int i = 0; i < MAX_VEHICLE_SEATS; i++)
    {
        CPed* pPed = m_pOccupants[i];
        if (pPed)
        {
            // Is he already getting out?
            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_EXITING)
            {
                // Does it have an occupant and is the occupant the requesting ped?
                unsigned char ucOccupiedSeat = pPed->GetOccupiedVehicleSeat();
                if (pPed == GetOccupant(ucOccupiedSeat))
                {
                    // Mark the ped/vehicle as empty
                    SetOccupant(NULL, ucOccupiedSeat);
                    pPed->SetOccupiedVehicle(NULL, 0);
                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);

                    // Tell everyone he has exited the vehicle
                    CVehicleInOutPacket Reply(pPed->GetID(), GetID(), ucOccupiedSeat, CGame::VEHICLE_NOTIFY_OUT_RETURN);
                    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(Reply);
                }
            }
        }
    }

    // Unset any tow links
    if (m_pTowedVehicle)
        m_pTowedVehicle->SetTowedByVehicle(NULL);

    if (m_pTowedByVehicle)
        m_pTowedByVehicle->SetTowedVehicle(NULL);

    // Make sure nobody's syncing us
    SetSyncer(NULL);

    // Unreference from our occupators
    for (unsigned int i = 0; i < MAX_VEHICLE_SEATS; i++)
    {
        if (m_pOccupants[i])
        {
            m_pOccupants[i]->SetOccupiedVehicle(NULL, 0);
            m_pOccupants[i]->SetVehicleAction(CPed::VEHICLEACTION_NONE);
        }
    }
    delete m_pUpgrades;

    CElementRefManager::RemoveElementRefs(ELEMENT_REF_DEBUG(this, "CVehicle"), &m_pTowedVehicle, &m_pTowedByVehicle, &m_pSyncer, &m_pJackingPed, NULL);

    // Notify the vehicle manager that we are not to be respawned anymore if neccessary
    if (m_bRespawnEnabled)
        m_pVehicleManager->GetRespawnEnabledVehicles().remove(this);

    // Remove us from the vehicle manager
    Unlink();
}

CElement* CVehicle::Clone(bool* bAddEntity, CResource* pResource)
{
    CVehicle* const pTemp = m_pVehicleManager->Create(GetParentEntity(), GetModel(), GetVariant(), GetVariant2());

    if (pTemp)
    {
        CVector vecRotationDegrees;
        GetRotationDegrees(vecRotationDegrees);
        pTemp->SetRotationDegrees(vecRotationDegrees);
        pTemp->SetBlowState(m_blowState);
        pTemp->SetHealth(GetHealth());
        pTemp->SetColor(GetColor());
        pTemp->SetUpgrades(GetUpgrades());
        pTemp->m_ucDoorStates = m_ucDoorStates;
        pTemp->m_ucWheelStates = m_ucWheelStates;
        pTemp->m_ucPanelStates = m_ucPanelStates;
        pTemp->m_ucLightStates = m_ucLightStates;
    }

    return pTemp;
}

void CVehicle::Unlink()
{
    // Remove us from the vehicle manager
    m_pVehicleManager->RemoveFromList(this);
}

bool CVehicle::ReadSpecialData(const int iLine)
{
    // Grab the "posX" data
    if (!GetCustomDataFloat("posX", m_vecPosition.fX, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posX' attribute in <vehicle> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posY" data
    if (!GetCustomDataFloat("posY", m_vecPosition.fY, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posY' attribute in <vehicle> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posZ" data
    if (!GetCustomDataFloat("posZ", m_vecPosition.fZ, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posZ' attribute in <vehicle> (line %d)\n", iLine);
        return false;
    }

    // Grab the "rotX", "rotY" and "rotZ" data
    GetCustomDataFloat("rotX", m_vecRotationDegrees.fX, true);
    GetCustomDataFloat("rotY", m_vecRotationDegrees.fY, true);
    GetCustomDataFloat("rotZ", m_vecRotationDegrees.fZ, true);

    // Wrap them around 360
    m_vecRotationDegrees.fX = WrapAround(m_vecRotationDegrees.fX, 360.0f);
    m_vecRotationDegrees.fY = WrapAround(m_vecRotationDegrees.fY, 360.0f);
    m_vecRotationDegrees.fZ = WrapAround(m_vecRotationDegrees.fZ, 360.0f);

    // Set the respawn matrix
    SetRespawnPosition(m_vecPosition);
    SetRespawnRotationDegrees(m_vecRotationDegrees);

    // Grab the "model" data
    int iTemp;
    if (GetCustomDataInt("model", iTemp, true))
    {
        // Is it valid?
        if (CVehicleManager::IsValidModel(iTemp))
        {
            // Remember it and generate a new random color
            SetModel(static_cast<unsigned short>(iTemp));

            m_usAdjustableProperty = 0;
        }
        else
        {
            CLogger::ErrorPrintf("Bad 'model'(%d) id specified in <vehicle> (line %d)\n", iTemp, iLine);
            return false;
        }
    }
    else
    {
        CLogger::ErrorPrintf("Bad/missing 'model' attribute in <vehicle> (line %d)\n", iLine);
        return false;
    }

    // Grab the variant data
    if (GetCustomDataInt("variant1", iTemp, true))
        m_ucVariant = static_cast<unsigned char>(iTemp);
    if (GetCustomDataInt("variant2", iTemp, true))
        m_ucVariant2 = static_cast<unsigned char>(iTemp);
    if (m_ucVariant == 254 && m_ucVariant2 == 254)
        CVehicleManager::GetRandomVariation(m_usModel, m_ucVariant, m_ucVariant2);

    // Grab the "turretX" data
    if (GetCustomDataFloat("turretX", m_fTurretPositionX, true))
        m_fTurretPositionX = ConvertDegreesToRadians(m_fTurretPositionX);

    // Grab the "turretY" data
    if (GetCustomDataFloat("turretY", m_fTurretPositionY, true))
        m_fTurretPositionY = ConvertDegreesToRadians(m_fTurretPositionY);

    // Grab the "health" data
    if (GetCustomDataFloat("health", m_fHealth, true))
        if (m_fHealth < 0.0f)
            m_fHealth = 0.0f;

    // Grab the "sirens" data
    GetCustomDataBool("sirens", m_bSirenActive, true);

    // Grab the "landingGearDown" data
    if (!GetCustomDataBool("landingGearDown", m_bLandingGearDown, true))
        m_bLandingGearDown = true;

    // Grab the "locked" data
    GetCustomDataBool("locked", m_bLocked, true);

    // Grab the "specialState" data
    if (GetCustomDataInt("specialState", iTemp, true))
        m_usAdjustableProperty = static_cast<unsigned short>(iTemp);
    else
        m_usAdjustableProperty = 0;

    // Grab the "color" data
    char szTemp[256];
    if (GetCustomDataString("color", szTemp, 256, true))
    {
        uchar ucValues[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        char* sz1 = strtok(szTemp, ", ");
        if (sz1)
            ucValues[0] = atoi(sz1);

        int i;
        for (i = 1; i < 12; i++)
        {
            char* szn = strtok(NULL, ", ");
            if (!szn)
                break;
            ucValues[i] = atoi(szn);
        }

        if (i == 3 || i == 6 || i == 9 || i == 12)
        {
            m_Color.SetRGBColors(SColorRGBA(ucValues[0], ucValues[1], ucValues[2], 0), SColorRGBA(ucValues[3], ucValues[4], ucValues[5], 0),
                                 SColorRGBA(ucValues[6], ucValues[7], ucValues[8], 0), SColorRGBA(ucValues[9], ucValues[10], ucValues[11], 0));
        }
        else
        {
            m_Color.SetPaletteColors(ucValues[0], ucValues[1], ucValues[2], ucValues[3]);
        }
    }

    // Grab the "headLightColor" data
    if (GetCustomDataString("headLightColor", szTemp, 64, true))
    {
        // Convert it to RGBA
        if (!XMLColorToInt(szTemp, m_HeadLightColor.R, m_HeadLightColor.G, m_HeadLightColor.B, m_HeadLightColor.A))
        {
            CLogger::ErrorPrintf("Bad 'headLightColor' value specified in <vehicle> (line %u)\n", iLine);
            return false;
        }
    }

    // Grab the "paintjob" data
    if (GetCustomDataInt("paintjob", iTemp, true))
        m_ucPaintjob = static_cast<unsigned char>(iTemp);

    // Grab the "upgrades" data
    if (GetCustomDataString("upgrades", szTemp, 256, true))
    {
        if (m_pUpgrades)
        {
            if (stricmp(szTemp, "all") == 0)
                m_pUpgrades->AddAllUpgrades();
            else
            {
                bool bTemp = true;
                while (char* token = strtok((bTemp) ? szTemp : NULL, ", "))
                {
                    bTemp = false;
                    unsigned short usUpgrade = static_cast<unsigned short>(atoi(token));
                    if (CVehicleUpgrades::IsValidUpgrade(usUpgrade))
                        m_pUpgrades->AddUpgrade(usUpgrade);
                }
            }
        }
    }

    // Grab the "plate" data
    if (GetCustomDataString("plate", szTemp, 9, true))
        SetRegPlate(szTemp);

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
    GetCustomDataBool("frozen", m_bIsFrozen, true);

    // Grab the "taxiLightOn" data
    GetCustomDataBool("taxiLightOn", m_bTaxiLightState, true);

    // Grab the "engineOn" data
    GetCustomDataBool("engineOn", m_bEngineOn, true);

    // Grab the "lightsOn" data
    bool bLightsOn;
    if (GetCustomDataBool("lightsOn", bLightsOn, true))
        m_ucOverrideLights = bLightsOn ? 2 : 1;

    // Grab the "damageProof" data
    GetCustomDataBool("damageProof", m_bDamageProof, true);

    // Grab the "explodableFuelTank" data
    GetCustomDataBool("explodableFuelTank", m_bFuelTankExplodable, true);

    // Grab the "toggleRespawn" data
    bool bRespawnOn;
    if (GetCustomDataBool("toggleRespawn", bRespawnOn, true))
        SetRespawnEnabled(bRespawnOn);

    // Grab the "respawnDelay" data
    int iRespawnDelay;
    if (GetCustomDataInt("respawnDelay", iRespawnDelay, true))
        m_ulIdleRespawnInterval = iRespawnDelay;

    // Grab the respawn position and rotation data
    GetCustomDataFloat("respawnPosX", m_vecRespawnPosition.fX, true);
    GetCustomDataFloat("respawnPosY", m_vecRespawnPosition.fY, true);
    GetCustomDataFloat("respawnPosZ", m_vecRespawnPosition.fZ, true);
    GetCustomDataFloat("respawnRotX", m_vecRespawnRotationDegrees.fX, true);
    GetCustomDataFloat("respawnRotY", m_vecRespawnRotationDegrees.fY, true);
    GetCustomDataFloat("respawnRotZ", m_vecRespawnRotationDegrees.fZ, true);

    return true;
}

void CVehicle::GetMatrix(CMatrix& matrix)
{
    CVector vecRotation;
    GetRotation(vecRotation);
    matrix.SetRotation(vecRotation);
    matrix.vPos = GetPosition();
}

void CVehicle::SetMatrix(const CMatrix& matrix)
{
    // Set position and rotation from matrix
    SetPosition(matrix.vPos);
    CVector vecRotation = matrix.GetRotation();
    ConvertRadiansToDegreesNoWrap(vecRotation);
    SetRotationDegrees(vecRotation);
}

const CVector& CVehicle::GetPosition()
{
    // Are we attached to something?
    if (m_pAttachedTo)
        GetAttachedPosition(m_vecPosition);
    return m_vecPosition;
}

void CVehicle::SetPosition(const CVector& vecPosition)
{
    // If we're attached to something, dont allow any change
    if (m_pAttachedTo)
        return;

    // Different to where we are now?
    if (m_vecPosition != vecPosition)
    {
        // Update our stored vectors
        m_vecPosition = vecPosition;
        UpdateSpatialData();
    }
}

void CVehicle::GetRotation(CVector& vecRotation)
{
    if (m_pAttachedTo)
        GetAttachedRotation(vecRotation);
    else
    {
        GetRotationDegrees(vecRotation);
        ConvertDegreesToRadians(vecRotation);
    }
}

void CVehicle::GetRotationDegrees(CVector& vecRotation)
{
    if (m_pAttachedTo)
    {
        GetAttachedRotation(vecRotation);
        ConvertRadiansToDegrees(vecRotation);
    }
    else
        vecRotation = m_vecRotationDegrees;
}

void CVehicle::SetRotationDegrees(const CVector& vecRotation)
{
    if (m_pAttachedTo)
        return;
    m_vecRotationDegrees = vecRotation;
}

void CVehicle::SetModel(unsigned short usModel)
{
    if (usModel != m_usModel)
    {
        m_usModel = usModel;
        m_eVehicleType = CVehicleManager::GetVehicleType(m_usModel);
        RandomizeColor();
        ResetDoors();
        CVehicleManager::GetRandomVariation(m_usModel, m_ucVariant, m_ucVariant2);

        // Generate new handling data to fit the vehicle
        GenerateHandlingData();
    }
}

bool CVehicle::HasValidModel()
{
    return CVehicleManager::IsValidModel(m_usModel);
}

void CVehicle::SetVariants(unsigned char ucVariant, unsigned char ucVariant2)
{
    m_ucVariant = ucVariant;
    m_ucVariant2 = ucVariant2;
}

void CVehicle::SetHealth(float fHealth)
{
    if (fHealth < 0.0f || IsBlown())
        fHealth = 0.0f;

    m_fHealth = fHealth;
}

CVehicleColor& CVehicle::RandomizeColor()
{
    // Grab a random color for this vehicle and return it
    m_Color = m_pVehicleManager->GetRandomColor(m_usModel);
    return m_Color;
}

void CVehicle::SetDoorOpenRatio(unsigned char ucDoor, float fRatio)
{
    if (ucDoor <= 5)
        m_fDoorOpenRatio[ucDoor] = SharedUtil::Clamp(0.0f, fRatio, 1.0f);
}

float CVehicle::GetDoorOpenRatio(unsigned char ucDoor) const
{
    return (ucDoor <= 5) ? m_fDoorOpenRatio[ucDoor] : 0.0f;
}

void CVehicle::GetTurretPosition(float& fPositionX, float& fPositionY)
{
    fPositionX = m_fTurretPositionX;
    fPositionY = m_fTurretPositionY;
}

void CVehicle::SetTurretPosition(float fPositionX, float fPositionY)
{
    m_fTurretPositionX = fPositionX;
    m_fTurretPositionY = fPositionY;
}

CPed* CVehicle::GetOccupant(unsigned int uiSeat)
{
    if (uiSeat < MAX_VEHICLE_SEATS)
    {
        return m_pOccupants[uiSeat];
    }

    return NULL;
}

CPed* CVehicle::GetFirstOccupant()
{
    // Try finding a seat with a Player in it
    unsigned int i = 0;
    for (; i < MAX_VEHICLE_SEATS; i++)
    {
        if (m_pOccupants[i])
        {
            return m_pOccupants[i];
        }
    }

    // No seats with Players in it
    return NULL;
}

CPed* CVehicle::GetController()
{
    CPed* pController = m_pOccupants[0];

    if (!pController)
    {
        CVehicle* pCurrentVehicle = this;
        CVehicle* pTowedByVehicle = m_pTowedByVehicle;
        pController = pCurrentVehicle->GetOccupant(0);
        while (pTowedByVehicle)
        {
            pCurrentVehicle = pTowedByVehicle;
            pTowedByVehicle = pCurrentVehicle->GetTowedByVehicle();
            CPed* pCurrentDriver = pCurrentVehicle->GetOccupant(0);
            if (pCurrentDriver)
                pController = pCurrentDriver;
        }
    }

    return pController;
}

bool CVehicle::SetOccupant(CPed* pPed, unsigned int uiSeat)
{
    // CPlayer::SetOccupiedVehicle will also call this so we need to prevent an infinite recursive loop
    static bool bAlreadySetting = false;
    if (!bAlreadySetting)
    {
        if (uiSeat >= MAX_VEHICLE_SEATS)
            return false;

        // Set the Player
        if (m_pOccupants[uiSeat] != pPed)
        {
            if (g_pBandwidthSettings->bLightSyncEnabled)
            {
                if (uiSeat == 0 && m_pOccupants[0] && IS_PLAYER(m_pOccupants[0]))
                    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CVehicleResyncPacket(this));
            }
            m_pOccupants[uiSeat] = pPed;
        }

        // Make sure the Player record is up to date
        if (pPed)
        {
            bAlreadySetting = true;
            pPed->SetOccupiedVehicle(this, uiSeat);
            bAlreadySetting = false;
        }

        // Ensure idle timer is stopped when the vehicle has any occupants
        if (GetFirstOccupant())
            StopIdleTimer();

        return true;
    }

    return true;
}

void CVehicle::SetSyncer(CPlayer* pPlayer)
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if (!bAlreadyIn)
    {
        // Update the last Player if any
        bAlreadyIn = true;
        if (m_pSyncer)
        {
            m_pSyncer->RemoveSyncingVehicle(this);
        }

        // Update the vehicle
        if (pPlayer)
        {
            pPlayer->AddSyncingVehicle(this);
        }
        bAlreadyIn = false;

        // Set it
        m_pSyncer = pPlayer;
    }
}

unsigned char CVehicle::GetMaxPassengers()
{
    return ((m_ucMaxPassengersOverride == VEHICLE_PASSENGERS_UNDEFINED) ? CVehicleManager::GetMaxPassengers(m_usModel) : m_ucMaxPassengersOverride);
}

unsigned char CVehicle::GetFreePassengerSeat()
{
    // Grab the max passengers this vehicle can have and check the rage
    unsigned char ucMaxPassengers = GetMaxPassengers();
    if (ucMaxPassengers < MAX_VEHICLE_SEATS)
    {
        // Check for free slots
        for (int i = 1; i < ucMaxPassengers + 1; i++)
        {
            if (!m_pOccupants[i])
            {
                return static_cast<unsigned char>(i);
            }
        }
    }

    return 0xFF;
}

void CVehicle::SetUpgrades(CVehicleUpgrades* pUpgrades)
{
    delete m_pUpgrades;

    m_pUpgrades = new CVehicleUpgrades(this, pUpgrades);
}

bool CVehicle::SetTowedVehicle(CVehicle* pVehicle)
{
    if (m_pTowedVehicle)
    {
        m_pTowedVehicle->m_pTowedByVehicle = NULL;
        m_pTowedVehicle = NULL;
    }

    if (pVehicle)
    {
        // Are we trying to establish a circular loop? (would freeze everything up)
        CVehicle* pCircTestVehicle = pVehicle;
        while (pCircTestVehicle)
        {
            if (pCircTestVehicle == this)
                return false;
            pCircTestVehicle = pCircTestVehicle->m_pTowedVehicle;
        }

        pVehicle->m_pTowedByVehicle = this;
    }

    m_pTowedVehicle = pVehicle;
    return true;
}

bool CVehicle::SetTowedByVehicle(CVehicle* pVehicle)
{
    if (m_pTowedByVehicle)
    {
        m_pTowedByVehicle->m_pTowedVehicle = NULL;
        m_pTowedByVehicle = NULL;
    }

    if (pVehicle)
    {
        // Are we trying to establish a circular loop? (would freeze everything up)
        CVehicle* pCircTestVehicle = pVehicle;
        while (pCircTestVehicle)
        {
            if (pCircTestVehicle == this)
                return false;
            pCircTestVehicle = pCircTestVehicle->m_pTowedByVehicle;
        }

        pVehicle->m_pTowedVehicle = this;
    }

    m_pTowedByVehicle = pVehicle;
    return true;
}

void CVehicle::SpawnAt(const CVector& vecPosition, const CVector& vecRotation)
{
    SetBlowState(VehicleBlowState::INTACT);
    SetHealth(GetRespawnHealth());
    StopIdleTimer();
    ResetDoorsWheelsPanelsLights();
    SetLandingGearDown(true);
    SetAdjustableProperty(0);
    SetTowedByVehicle(NULL);
    AttachTo(NULL);

    m_vecTurnSpeed = CVector();
    m_vecVelocity = CVector();
    m_vecPosition = vecPosition;
    m_vecRotationDegrees = vecRotation;
    UpdateSpatialData();
}

void CVehicle::Respawn()
{
    SpawnAt(m_vecRespawnPosition, m_vecRespawnRotationDegrees);
}

void CVehicle::SetRegPlate(const char* szRegPlate)
{
    // Copy the text and make sure non-used chars are nulled.
    memset(m_szRegPlate, 0, 9);
    STRNCPY(m_szRegPlate, szRegPlate, 9);
}

void CVehicle::GenerateRegPlate()
{
    // For all our 8 letters
    for (int i = 0; i < 8; i++)
    {
        // Put a space in letter 5
        if (i != 4)
        {
            // Generate a random letter
            int iChar = rand() % 36 + 'A';
            if (iChar > 'Z')
            {
                iChar = iChar - 43;
            }

            // Put it in the plate
            m_szRegPlate[i] = iChar;
        }
    }

    // Null terminator and space in number 5
    m_szRegPlate[4] = ' ';
    m_szRegPlate[8] = 0;
}

void CVehicle::SetPaintjob(unsigned char ucPaintjob)
{
    m_ucPaintjob = ucPaintjob;
}

void CVehicle::GetInitialDoorStates(SFixedArray<unsigned char, MAX_DOORS>& ucOutDoorStates)
{
    switch (m_usModel)
    {
        case VT_BAGGAGE:
        case VT_BANDITO:
        case VT_BFINJECT:
        case VT_CADDY:
        case VT_DOZER:
        case VT_FORKLIFT:
        case VT_KART:
        case VT_MOWER:
        case VT_QUAD:
        case VT_RCBANDIT:
        case VT_RCCAM:
        case VT_RCGOBLIN:
        case VT_RCRAIDER:
        case VT_RCTIGER:
        case VT_TRACTOR:
        case VT_VORTEX:
        case VT_BLOODRA:
            memset(&ucOutDoorStates[0], DT_DOOR_MISSING, MAX_DOORS);

            // Keep the bonet and boot intact
            ucOutDoorStates[0] = ucOutDoorStates[1] = DT_DOOR_INTACT;
            break;
        default:
            memset(&ucOutDoorStates[0], DT_DOOR_INTACT, MAX_DOORS);
    }
}

void CVehicle::GenerateHandlingData() noexcept
{
    const auto* handlingManager = g_pGame->GetHandlingManager();

    // Make a new CHandlingEntry
    if (!m_HandlingEntry)
        m_HandlingEntry = handlingManager->CreateHandlingData();

    // Apply the model handling info
    m_HandlingEntry->ApplyHandlingData(handlingManager->GetModelHandlingData(m_usModel));

    m_bHandlingChanged = false;
}

void CVehicle::SetVehicleSirenPosition(unsigned char ucSirenID, CVector vecPos)
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions = vecPos;
}

void CVehicle::SetVehicleSirenMinimumAlpha(unsigned char ucSirenID, DWORD dwPercentage)
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_dwMinSirenAlpha = dwPercentage;
}

void CVehicle::SetVehicleSirenColour(unsigned char ucSirenID, SColor tVehicleSirenColour)
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour = tVehicleSirenColour;
}

void CVehicle::SetVehicleFlags(bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent)
{
    m_tSirenBeaconInfo.m_b360Flag = bEnable360;
    m_tSirenBeaconInfo.m_bDoLOSCheck = bEnableLOSCheck;
    m_tSirenBeaconInfo.m_bUseRandomiser = bEnableRandomiser;
    m_tSirenBeaconInfo.m_bSirenSilent = bEnableSilent;
}
void CVehicle::RemoveVehicleSirens()
{
    for (int i = 0; i <= 7; i++)
    {
        m_tSirenBeaconInfo.m_tSirenInfo[i] = SSirenBeaconInfo();
        SetVehicleSirenPosition(i, CVector(0, 0, 0));
        SetVehicleSirenMinimumAlpha(i, 0);
        SetVehicleSirenColour(i, SColor());
    }

    m_tSirenBeaconInfo.m_ucSirenCount = 0;
}

void CVehicle::ResetDoors()
{
    GetInitialDoorStates(m_ucDoorStates);
    for (unsigned int i = 0; i < 6; ++i)
        m_fDoorOpenRatio[i] = 0.0f;
}

void CVehicle::ResetDoorsWheelsPanelsLights()
{
    ResetDoors();
    memset(&m_ucWheelStates[0], 0, sizeof(m_ucWheelStates));
    memset(&m_ucPanelStates[0], 0, sizeof(m_ucPanelStates));
    memset(&m_ucLightStates[0], 0, sizeof(m_ucLightStates));
}

bool CVehicle::IsBlowTimerFinished()
{
    return (m_blowState == VehicleBlowState::BLOWN) && CTickCount::Now() > m_llBlowTime + CTickCount((long long)m_ulBlowRespawnInterval);
}

void CVehicle::ResetExplosionTimer()
{
    if (m_blowState == VehicleBlowState::BLOWN)
        m_llBlowTime = CTickCount::Now();
}

void CVehicle::SetBlowState(VehicleBlowState state)
{
    m_blowState = state;

    if (state == VehicleBlowState::BLOWN)
        m_llBlowTime = CTickCount::Now();
}

void CVehicle::StopIdleTimer()
{
    m_llIdleTime = CTickCount(0LL);
}

void CVehicle::RestartIdleTimer()
{
    m_llIdleTime = CTickCount::Now();
}

bool CVehicle::IsIdleTimerRunning()
{
    return m_llIdleTime.ToLongLong() != 0;
}

bool CVehicle::IsIdleTimerFinished()
{
    return IsIdleTimerRunning() && CTickCount::Now() > m_llIdleTime + CTickCount((long long)m_ulIdleRespawnInterval);
}

// Check if vehicle has not moved (much) since the last call
bool CVehicle::IsStationary()
{
    const CVector& vecPosition = GetPosition();
    if ((vecPosition - m_vecStationaryCheckPosition).LengthSquared() < 0.1f * 0.1f)
        return true;

    m_vecStationaryCheckPosition = vecPosition;
    return false;
}

void CVehicle::SetJackingPed(CPed* pPed)
{
    if (pPed == m_pJackingPed)
        return;

    // Remove old
    if (m_pJackingPed)
    {
        CPed* pPrev = m_pJackingPed;
        m_pJackingPed = NULL;
        pPrev->SetJackingVehicle(NULL);
    }

    // Set new
    m_pJackingPed = pPed;

    if (m_pJackingPed)
        m_pJackingPed->SetJackingVehicle(this);
}

void CVehicle::OnRelayUnoccupiedSync()
{
    // Detect dimension change
    m_bNeedsDimensionResync |= (GetDimension() != m_usLastUnoccupiedSyncDimension);
    m_usLastUnoccupiedSyncDimension = GetDimension();
}

void CVehicle::HandleDimensionResync()
{
    if (m_bNeedsDimensionResync)
    {
        // Unoccupied vehicle might be desynced because of dimension optimizations, so resync to players in new dimension
        g_pGame->GetPlayerManager()->BroadcastDimensionOnlyJoined(CVehicleResyncPacket(this), GetDimension());
        m_bNeedsDimensionResync = false;
    }
}

void CVehicle::SetRespawnEnabled(bool bEnabled)
{
    // If we changed the state, update the internal var and notify the vehicle manager
    if (bEnabled != m_bRespawnEnabled)
    {
        if (bEnabled)
            m_pVehicleManager->GetRespawnEnabledVehicles().push_back(this);
        else
            m_pVehicleManager->GetRespawnEnabledVehicles().remove(this);

        m_bRespawnEnabled = bEnabled;
    }
}
